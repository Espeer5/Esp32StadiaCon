/**
 * @file    auth.h
 * @brief   Definitions and method prototypes for handling the authentication of 
 *          the ESP32C6 with a generic HID device and running the GAP event
 *          handler.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#ifndef _AUTH_GAP_H_
#define _AUTH_GAP_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

// Variable shared between the GAP profile and GATTC profile
// Indicates if GAP has succesfully found the device to connect to
extern bool connect;

/**
 * @brief Initialize the security parameters for the ESP32C6 to authenticate
 *       with a generic HID device.
*/
void esp_auth_init(void);

/**
 * 
 * @brief Conversion of authentication key types to strings.
 * 
 * Converts the key type to a string representation for debugging or logging 
 * purposes.
 * 
 * @param key_type The key type to convert to a string.
 * @return A string representation of the key type.
*/
const char *esp_key_type_to_str(esp_ble_key_type_t key_type);

/**
 * 
 * @brief Conversion of authentication requirements to strings.
 * 
 * Converts the authentication requirements to a string representation for 
 * debugging or logging purposes.
 * 
 * @param auth_req The authentication requirements to convert to a string.
 * @return A string representation of the authentication requirements.
*/
char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req);

/**
 * 
 * @brief Callback function for the GAP event handler to handle the
 *        authentication of the ESP32C6 with a generic HID device as it is 
 *        discovered and paired.
 * 
 * @param event The event that was triggered by the GAP event handler.
 * @param param A pointer to the parameters for the event.
 * @return void
*/
void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

#endif /* #ifndef _AUTH_H_ */
