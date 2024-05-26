/**
 * @file    gattc.h
 * @brief   Definitions and setup method prototypes for using ESP32C6 as a GATT
 *          client to connect to a Google Stadia Controller over BLE.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
 */

#ifndef _GATTC_H_
#define _GATTC_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define HID_SERVICE_UUID 0x1812  // HID Service UUID
#define HID_RPT_CHAR_UUID 0x2A4D // HID Report Characteristic UUID
#define PROFILE_NUM 1            // Profile number
#define PROFILE_A_APP_ID 0       // Application ID for the profile

/**
 * @brief The profile instance for the GATT client to connect to the Google
 *        Stadia Controller. Each profile instance contains a profile callback,
 *        as well as the handles for the service and characteristics to notify 
 *        the ESP32C6 of changes in the controller's state.
*/
struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t notify_char_handle;
    esp_bd_addr_t remote_bda;
};

extern struct gattc_profile_inst gl_profile_tab[PROFILE_NUM];

/**
 * Prototypes for the callback functions used by the GATT client.
*/

/**
 * @brief Callback function for the GATT client to handle events from the ESP32C6
 *        BLE stack.
 * 
 * @param event The event that was triggered by the BLE stack.
 * @param gatts_if The interface that the event was triggered on.
 * @param param The parameters for the event.
 */
void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gatts_if,
                  esp_ble_gattc_cb_param_t *param);

/**
 * @brief Callback function for the GATT profile to handle events triggered by 
 *        the remote device.
 * 
 * @param event The event that was triggered by the BLE stack.
 * @param gatts_if The interface that the event was triggered on.
 * @param param The parameters for the event.
*/
void gattc_profile_event_handler(esp_gattc_cb_event_t event,
                                 esp_gatt_if_t gattc_if,
                                 esp_ble_gattc_cb_param_t *param);

#endif /* #ifndef _GATTC_H_ */
