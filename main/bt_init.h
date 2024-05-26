/**
 * @file bt_init.h
 * @brief Definitions and setup method prototypes for initializing the ESP32C6
 *       as a Bluetooth device and setting up the GAP and GATT client profiles.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#ifndef _BT_INIT_H_
#define _BT_INIT_H_

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

/**
 * @brief Initialize NVM storage for the Bluetooth controller
 * 
 * @return 0 if successful, -1 if failed
*/
void bt_nvs_init(void);

/**
 * @brief Initialize the Bluetooth controller
 * 
 * @return 0 if successful, -1 if failed
*/
void bt_controller_init(void);

/**
 * @brief Initialize the Bluetooth stack
 * 
 * @return 0 if successful, -1 if failed
*/
void bt_stack_init(void);

/**
 * @brief Set the MTU size for the Bluetooth controller
 * 
 * @return 0 if successful, -1 if failed
*/
void bt_mtu_set(void);

/**
 * @brief Initialize the GAP profile
 * 
 * @return 0 if successful, -1 if failed
*/
void gap_profile_init(void);

/**
 * @brief Initialize the GATT client profile
 * 
 * @return 0 if successful, -1 if failed
*/
void gattc_profile_init(void);

#endif /* #ifndef _BT_INIT_H_ */
