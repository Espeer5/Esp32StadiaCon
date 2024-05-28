/**
 * @file    gattc.c
 * @brief   Implementation of the GATT client for the ESP32C6 to connect to a
 *          Google Stadia Controller over BLE.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
 * 
 * Much of this code is based on the ESP-IDF GATT client example code, which 
 * can be found at 
 * https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/bluedroid/ble/gatt_security_client
*/

#include "rep_queue.h"
#include "gattc.h"
#include "auth_gap.h"
#include "globalconst.c"

// Placeholder for an empty char handle when searching all chars in service
#define INVALID_HANDLE   0

extern bool connect;

// Shared with the GAP profile to share bluetooth external device information
struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,
    },
};

/** 
 * These fields are used to store information about the connected service's 
 * characteristics and descriptors.
*/
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;

// Indicates if the service has been found
static bool get_service = false;

// Filters discovered services by the HID service UUID
static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = HID_SERVICE_UUID,},
};

/**
 * Callback functions for the GATT client to handle events from the ESP32C6
*/

void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                         esp_ble_gattc_cb_param_t *param) {
    if (GATTC_DEBUG) {
        ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d", event, gattc_if);
    }

    // If event is register event, store the gattc_if for each profile
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "Reg app failed, app_id %04x, status %d",
                        param->reg.app_id, param->reg.status);
            }
            return;
        }
    }

    // Call the appropriate profile callback function for the event based on 
    // the gattc_if
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            // If gattc_if unspecified, call the callback for all profiles
            if (gattc_if == ESP_GATT_IF_NONE ||
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

void gattc_profile_event_handler(esp_gattc_cb_event_t event,
                                 esp_gatt_if_t gattc_if,
                                 esp_ble_gattc_cb_param_t *param) {

    // Store the data from the event
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    // Dispatch off the type of event
    switch (event) {
        // Resiters the GATT client with the BLE stack. Set the local privacy
        case ESP_GATTC_REG_EVT:
            esp_ble_gap_config_local_privacy(true);
            break;

        // Connection opened to an external device. Ensure success.
        case ESP_GATTC_OPEN_EVT:
            if (param->open.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "open failed, error status = %x",
                         p_data->open.status);
                break;
            }
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "open success");
            }

            // Insert the connection ID and remote BDA into the profile table
            gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->open.conn_id;
            memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                   p_data->open.remote_bda, sizeof(esp_bd_addr_t));
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
                esp_log_buffer_hex(GATTC_TAG, 
                               gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                               sizeof(esp_bd_addr_t));
            }
            
            // Start the MTU request. We need to negotiate the MTU rate. For
            // the Stadia controller, we need to set the MTU to 23 since it uses 
            // ble 4.0
            esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req(gattc_if,
                                                          p_data->open.conn_id);
            if (mtu_ret){
                ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x",
                         mtu_ret);
            }
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "POST MTU request\n");
            }
            break;

        // MTU request event. Ensure success.
        case ESP_GATTC_CFG_MTU_EVT:
            if (param->cfg_mtu.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG,"config mtu failed, error status = %x",
                         param->cfg_mtu.status);
            }
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG,
                     "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d",
                     param->cfg_mtu.status, param->cfg_mtu.mtu,
                     param->cfg_mtu.conn_id);
            }
            esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id,
                                         &remote_filter_service_uuid);
            break;
        
        // Service search returned result. Store the service information found.
        case ESP_GATTC_SEARCH_RES_EVT: {
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG,
                         "SEARCH RES: conn_id = %x is primary service %d",
                         p_data->search_res.conn_id,
                         p_data->search_res.is_primary);
                ESP_LOGI(GATTC_TAG,
                     "start handle %d end handle %d current handle value %d",
                     p_data->search_res.start_handle,
                     p_data->search_res.end_handle,
                     p_data->search_res.srvc_id.inst_id);
            }

            // If this is the service we're looking for, store it in the profile
            if (p_data->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16 &&
                p_data->search_res.srvc_id.uuid.uuid.uuid16 == 
                HID_SERVICE_UUID) {
                get_service = true;
                gl_profile_tab[PROFILE_A_APP_ID].service_start_handle =
                                                p_data->search_res.start_handle;
                gl_profile_tab[PROFILE_A_APP_ID].service_end_handle =
                                                  p_data->search_res.end_handle;
            }
            break;
        }

        // Service search is completed. Log the status of the search.
        case ESP_GATTC_SEARCH_CMPL_EVT:
            if (p_data->search_cmpl.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "search service failed, error status = %x",
                         p_data->search_cmpl.status);
                break;
            }
            if(p_data->search_cmpl.searched_service_source ==
               ESP_GATT_SERVICE_FROM_REMOTE_DEVICE) {
                if (GATTC_DEBUG) {
                    ESP_LOGI(GATTC_TAG,
                            "Get service information from remote device");
                }
            } else if (p_data->search_cmpl.searched_service_source ==
                       ESP_GATT_SERVICE_FROM_NVS_FLASH) {
                if (GATTC_DEBUG) {
                    ESP_LOGI(GATTC_TAG, "Get service information from flash");
                }
            } else {
                if (GATTC_DEBUG) {
                    ESP_LOGI(GATTC_TAG, "unknown service source");
                }
            }

            // If we found the HID service, search for the HID report
            // characteristic to enable notifications on.
            if (get_service){
                uint16_t count  = 0;
                uint16_t offset = 0;
                esp_gatt_status_t ret_status =
                          esp_ble_gattc_get_attr_count(gattc_if,
                          gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                          ESP_GATT_DB_CHARACTERISTIC,
                          gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                          gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                          INVALID_HANDLE, &count);
                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG,
                             "esp_ble_gattc_get_attr_count error, %d",
                             __LINE__);
                }
                if (count > 0){
                    char_elem_result = (esp_gattc_char_elem_t *)
                                  malloc(sizeof(esp_gattc_char_elem_t) * count);
                    if (!char_elem_result){
                        ESP_LOGE(GATTC_TAG, "gattc no mem");
                    }else{
                        // Get all characteristics of the HID service
                        ret_status = esp_ble_gattc_get_all_char(gattc_if,
                                       gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                       gl_profile_tab[PROFILE_A_APP_ID].
                                           service_start_handle,
                                       gl_profile_tab[PROFILE_A_APP_ID].
                                           service_end_handle,
                                       char_elem_result, &count, offset);
                        if (ret_status != ESP_GATT_OK){
                            ESP_LOGE(GATTC_TAG,
                                     "esp_ble_gattc_get_all_char error, %d",
                                     __LINE__);
                        }
                        // Find the report characteristic and register for 
                        // notifications
                        if (count > 0){
                            for (int i = 0; i < count; ++i)
                            {
                                if (char_elem_result[i].uuid.len ==
                                    ESP_UUID_LEN_16
                                    && char_elem_result[i].uuid.uuid.uuid16 ==
                                    HID_RPT_CHAR_UUID &&
                                    (char_elem_result[i].properties &
                                    ESP_GATT_CHAR_PROP_BIT_NOTIFY))
                                {
                                    gl_profile_tab[PROFILE_A_APP_ID].notify_char_handle =
                                    char_elem_result[i].char_handle;
                                    esp_ble_gattc_register_for_notify(gattc_if,
                                    gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                               char_elem_result[i].char_handle);
                                    break;
                                }
                            }
                        }
                    }
                    free(char_elem_result);
                }
            }
            break;

        // Register for notifications on the HID report characteristic
        case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
            if (p_data->reg_for_notify.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "reg for notify failed, error status = %x",
                         p_data->reg_for_notify.status);
                break;
            }

                uint16_t count = 0;
                uint16_t offset = 0;
                uint16_t notify_en = 0x1;
                esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count(
                          gattc_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                          ESP_GATT_DB_DESCRIPTOR,
                          gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                          gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                          p_data->reg_for_notify.handle, &count);
                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG,
                             "esp_ble_gattc_get_attr_count error, %d",
                             __LINE__);
                }
                // Find the client characteristic configuration descriptor
                // and write to it to enable notifications
                if (count > 0){
                    descr_elem_result =
                                 malloc(sizeof(esp_gattc_descr_elem_t) * count);
                    if (!descr_elem_result){
                        ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                    }else{
                        ret_status = esp_ble_gattc_get_all_descr(gattc_if,
                                       gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                       p_data->reg_for_notify.handle,
                                       descr_elem_result, &count, offset);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG,
                                 "esp_ble_gattc_get_all_descr error, %d",
                                 __LINE__);
                    }

                        for (int i = 0; i < count; ++i)
                        {
                            // Write 0x1 to notify descriptor to enable 
                            // notifications
                            if (descr_elem_result[i].uuid.len == ESP_UUID_LEN_16
                                && descr_elem_result[i].uuid.uuid.uuid16 == 
                                ESP_GATT_UUID_CHAR_CLIENT_CONFIG)
                            {
                                esp_ble_gattc_write_char_descr (gattc_if,
                                       gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                       descr_elem_result[i].handle,
                                       sizeof(notify_en),
                                       (uint8_t *)&notify_en,
                                       ESP_GATT_WRITE_TYPE_NO_RSP,
                                       ESP_GATT_AUTH_REQ_NONE);

                                break;
                            }
                        }
                    }
                    free(descr_elem_result);
                }

            break;
        }

        // Notification received from the HID report characteristic.
        case ESP_GATTC_NOTIFY_EVT:
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, receive notify value:");
                esp_log_buffer_hex(GATTC_TAG, p_data->notify.value,
                                   p_data->notify.value_len);
            }
            StadiaRep_t *rep = load_stadia_rep(p_data->notify.value,
                                               p_data->notify.value_len);
            if (rep != NULL) {
                insert_stadia_rep(repQueue, rep);
            }
            break;
        
        // Response to writing to a characteristic descriptor. Ensure success.
        case ESP_GATTC_WRITE_DESCR_EVT:
            if (p_data->write.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x",
                         p_data->write.status);
                break;
            }
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "write descr success");
            }
            break;

        // Service change event. Log the remote device's address. Reload the
        // services after the change. On Stadia controller, this occurs after
        // the controller is initially conected and a button is pressed. This
        // is the time to re-enable notifications.
        case ESP_GATTC_SRVC_CHG_EVT: {
            esp_bd_addr_t bda;
            memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:");
                esp_log_buffer_hex(GATTC_TAG, bda, sizeof(esp_bd_addr_t));
            }
            // Log all of the available services after change
            esp_ble_gattc_search_service(gattc_if,
                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                         &remote_filter_service_uuid);
            break;
        }

        // Write to a characteristic. Ensure success.
        case ESP_GATTC_WRITE_CHAR_EVT:
            if (p_data->write.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "write char failed, error status = %x",
                         p_data->write.status);
                break;
            }
            if (GATTC_DEBUG) {
                ESP_LOGI(GATTC_TAG, "Write char success ");
            }
            break;

        // Disconnect event. Log the reason for the disconnection.
        case ESP_GATTC_DISCONNECT_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_DISCONNECT_EVT, reason = 0x%x",
                     p_data->disconnect.reason);
            connect = false;
            get_service = false;
            break;
        default:
            break;
    }
}
