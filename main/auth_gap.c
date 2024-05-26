/**
 * @file auth.c
 * @brief Implementation of the authentication of the ESP32C6 with a generic HID
 *        device.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#include "auth_gap.h"
#include "globalconst.c"
#include "gattc.h"

// Variable shared between the GAP profile and GATTC profile indicating 
// whether GAP has successfully found the device to connect to
bool connect = false;

/**
 * @brief The scanning parameters for the ESP32C6 to discover generic HID
 *        devices and initiate authentication.
*/
esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_RPA_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

void esp_auth_init(void) {
    // set the security iocap & auth_req & key size & init key response key
    // parameters to the stack
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;
    uint8_t key_size = 16;
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req,
                                   sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap,
                                   sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size,
                                   sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support,
                                   sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key,
                                   sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key,
                                   sizeof(uint8_t));
}

const char *esp_key_type_to_str(esp_ble_key_type_t key_type) {
    const char *key_str = NULL;
    switch(key_type) {
        case ESP_LE_KEY_NONE:
            key_str = "ESP_LE_KEY_NONE";
            break;
        case ESP_LE_KEY_PENC:
            key_str = "ESP_LE_KEY_PENC";
            break;
        case ESP_LE_KEY_PID:
            key_str = "ESP_LE_KEY_PID";
            break;
        case ESP_LE_KEY_PCSRK:
            key_str = "ESP_LE_KEY_PCSRK";
            break;
        case ESP_LE_KEY_PLK:
            key_str = "ESP_LE_KEY_PLK";
            break;
        case ESP_LE_KEY_LLK:
            key_str = "ESP_LE_KEY_LLK";
            break;
        case ESP_LE_KEY_LENC:
            key_str = "ESP_LE_KEY_LENC";
            break;
        case ESP_LE_KEY_LCSRK:
            key_str = "ESP_LE_KEY_LCSRK";
            break;
        default:
            key_str = "UNKNOWN";
            break;
    }
    return key_str;
}

char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req) {
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
   }
   return auth_str;
}

void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    
    // Discovered device name string and size
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;

    // Dispatch based on event type
    switch (event) {

        // Response to setting privacy mode on the ESP32C6. Ensures success of 
        // setting privacy mode before scanning for devices.
        case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
            if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
                ESP_LOGE(GATTC_TAG,
                         "config local privacy failed, error code =%x",
                         param->local_privacy_cmpl.status);
                break;
            }
            // Privacy mode set successfully, prepare for scanning by setting 
            // scan parameters
            esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
            if (scan_ret){
                ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x",
                         scan_ret);
            }
            break;

        // Verify that the scan parameters were set successfully
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
            uint32_t duration = 30; // Seconds
            esp_ble_gap_start_scanning(duration);
            break;
        }

        // Response to the start of scanning for devices
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
            // Report scan start status
            if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTC_TAG, "scan start failed, error status = %x",
                         param->scan_start_cmpl.status);
                break;
            }
            ESP_LOGI(GATTC_TAG, "Scan start success");
            break;

        // Response to a request for a passkey to pair with a device. Stadia
        // controller does not require a passkey, but to pair with HID devices
        // that does, the reply line should be uncommented. 
        case ESP_GAP_BLE_PASSKEY_REQ_EVT:
            // Call the following function to input the passkey which is
            // displayed on the remote device
            //esp_ble_passkey_reply(gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
            //                       true, 0x00);
            ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_PASSKEY_REQ_EVT");
            break;

        // Response to a request for an out-of-band key to pair with a device.
        // Stadia controller does not require an out-of-band key, but to pair
        // with a generic HID device this code is necessary. In OOB, both
        // devices need to use the same tk.
        case ESP_GAP_BLE_OOB_REQ_EVT: {
            ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
            uint8_t tk[16] = {1};
            esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk,
                                  sizeof(tk));
            break;
        }

        // ER/IR events. Simply log the event for debugging purposes.
        case ESP_GAP_BLE_LOCAL_IR_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_LOCAL_IR_EVT");
            break;
        case ESP_GAP_BLE_LOCAL_ER_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_LOCAL_ER_EVT");
            break;

        // Respond to a security requirement from a discovered device.
        case ESP_GAP_BLE_SEC_REQ_EVT:
            // Accept the security request from the device
            esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
            break;
        
        // Respond to a request for numeric comparison from a discovered device.
        case ESP_GAP_BLE_NC_REQ_EVT:
            /* The app will receive this evt when the IO has DisplayYesNO
               capability and the peer device IO also has DisplayYesNo
               capability. show the passkey number to the user to confirm it
               with the number displayed by peer device. *Should not be used 
               for Google Stadia Controller.* */
            esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
            ESP_LOGI(GATTC_TAG,
                     "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%"
                     PRIu32, param->ble_security.key_notif.passkey);
            break;

        // A discovered device requires a passkey to pair and the peer device is
        // displaying the passkey. This is not used for the Google Stadia
        // Controller, but is necessary for pairing with some HID devices.
        case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
            // show the passkey number to the user to input it in the peer
            // device.
            ESP_LOGI(GATTC_TAG, "The passkey Notify number:%06" PRIu32,
                     param->ble_security.key_notif.passkey);
            break;

        // The reverse circumstance of the ESP_GAP_BLE_PASSKEY_NOTIF_EVT.
        case ESP_GAP_BLE_KEY_EVT:
            //shows the ble key info share with peer device to the user.
            ESP_LOGI(GATTC_TAG, "key type = %s",
                     esp_key_type_to_str(param->ble_security.ble_key.key_type));
            break;

        // Authentication has been completed. Report the pairing status and 
        // authentication mode. For Google Stadia controller, this should report 
        // ESP_LE_AUTH_REQ_SC_BOND for 1001 secure connection with the band. 
        case ESP_GAP_BLE_AUTH_CMPL_EVT: {
            esp_bd_addr_t bd_addr;
            memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr,
                   sizeof(esp_bd_addr_t));
            ESP_LOGI(GATTC_TAG, "remote BD_ADDR: %08x%04x",\
                    (bd_addr[0] << 24) + (bd_addr[1] << 16) +
                    (bd_addr[2] << 8) + bd_addr[3],
                    (bd_addr[4] << 8) + bd_addr[5]);
            ESP_LOGI(GATTC_TAG, "address type = %d",
                     param->ble_security.auth_cmpl.addr_type);
            ESP_LOGI(GATTC_TAG, "pair status = %s",
                     param->ble_security.auth_cmpl.success ? "success"
                     : "fail");
            if (!param->ble_security.auth_cmpl.success) {
                ESP_LOGI(GATTC_TAG, "fail reason = 0x%x",
                         param->ble_security.auth_cmpl.fail_reason);
            } else {
                ESP_LOGI(GATTC_TAG, "auth mode = %s",
                         esp_auth_req_to_str(param->ble_security.
                                             auth_cmpl.auth_mode));
            }
            break;
        }

        // A discovered device has been authenticated and connected to
        case ESP_GAP_BLE_SCAN_RESULT_EVT: {
            esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
            // Dispatch on the type of scan result
            switch (scan_result->scan_rst.search_evt) {
                case ESP_GAP_SEARCH_INQ_RES_EVT:
                    esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
                    ESP_LOGI(GATTC_TAG,
                            "Searched Adv Data Len %d, Scan Response Len %d",
                            scan_result->scan_rst.adv_data_len,
                            scan_result->scan_rst.scan_rsp_len);
                    adv_name = esp_ble_resolve_adv_data(scan_result->
                                                        scan_rst.ble_adv,
                                                        ESP_BLE_AD_TYPE_NAME_CMPL,
                                                        &adv_name_len);
                    ESP_LOGI(GATTC_TAG, "Searched Device Name Len %d",
                             adv_name_len);
                    esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
                    ESP_LOGI(GATTC_TAG, "\n");
                    if (adv_name != NULL) {
                        if (strlen(remote_device_name) == adv_name_len &&
                            strncmp((char *)adv_name, remote_device_name, 
                            adv_name_len) == 0) {
                            ESP_LOGI(GATTC_TAG, "searched device %s\n",
                                    remote_device_name);
                            if (connect == false) {
                                connect = true;
                                ESP_LOGI(GATTC_TAG,
                                        "connect to the remote device.");
                                esp_ble_gap_stop_scanning();
                                esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].
                                                gattc_if,
                                                scan_result->scan_rst.bda,
                                                scan_result->
                                                scan_rst.ble_addr_type,
                                                true);
                            }
                        }
                    }
                    break;
                case ESP_GAP_SEARCH_INQ_CMPL_EVT:
                    break;
                default:
                    break;
                }
                break;
        }

        // Response to the end of a scan. Resport the status of the scan.
        case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
            if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
                ESP_LOGE(GATTC_TAG, "Scan stop failed, error status = %x",
                         param->scan_stop_cmpl.status);
                break;
            }
            ESP_LOGI(GATTC_TAG, "Stop scan successfully");
            break;

        default:
            break;
    }
}
