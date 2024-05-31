/**
 * @file bt_init.c
 * @brief Definitions and setup method implementations for initializing the
 *        ESP32C6 as a Bluetooth device and setting up the GAP and GATT client
 *        profiles.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "bt_init.h"
#include "auth_gap.h"
#include "gattc.h"
#include "globalconst.h"

void bt_nvs_init(void) {
    // Initialize NVS.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void bt_controller_init(void) {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    // Initialize the Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s initialize controller failed: %s\n", __func__,
                 esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
}

void bt_stack_init(void) {
    esp_err_t ret;
    // Initialize the Bluetooth stack
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
}

void bt_mtu_set(void) {
    // Set the MTU size for the Bluetooth controller
    esp_err_t ret = esp_ble_gatt_set_local_mtu(23);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", ret);
    }
}

void gap_profile_init(void) {
    // Assign the GAP callback function
    esp_err_t ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "gap register error, error code = %x", ret);
        return;
    }
}

void gattc_profile_init(void) {
    // Assign the GATT client callback function
    esp_err_t ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "gattc register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gattc app register error, error code = %x\n", __func__, ret);
    }
}
