/**
 * @file main.c
 * @brief Main file for the ESP32C6 to connect to a Google Stadia Controller
 *        over BLE.
*/

#include "bt_init.h"
#include "auth_gap.h"

void app_main(void) {
    // Initialize the NVS storage for the Bluetooth controller
    bt_nvs_init();
    // Initialize the Bluetooth controller
    bt_controller_init();
    // Initialize the Bluetooth stack
    bt_stack_init();
    // Set the MTU size for the Bluetooth controller
    bt_mtu_set();
    // Initialize the GAP profile
    gap_profile_init();
    // Initialize the GATT client profile
    gattc_profile_init();
    // Initialize the security and authentication parameters
    esp_auth_init();
}