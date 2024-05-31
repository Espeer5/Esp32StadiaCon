/**
 * @file main.c
 * @brief Main file for the ESP32C6 to connect to a Google Stadia Controller
 *        over BLE.
*/

#include "bt_init.h"
#include "auth_gap.h"
#include "rep_queue.h"
#include "con_state.h"

#include "freertos/freeRTOS.h"
#include "freertos/semphr.h"

// The incoming bluetooth report queue
RepQueue_t *repQueue;

// A counting semaphore used to notify the main thread that a new report is
// available in the queue
SemaphoreHandle_t repSem;

// The controller state
ConState_t state;


void app_main(void) {
    // Initialize the NVS storage for the Bluetooth controller
    bt_nvs_init();
    // Initialize the Stadia Report Queue
    repQueue = create_stadia_rep_queue();
    // Initialize the controller state
    init_controller(&state);
    // Create the counting semaphore
    repSem = xSemaphoreCreateCounting(INT8_MAX, 0);
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
    // Start updating the controller state with incoming reports
    while (1) {
        // Wait for a new report to be available
        xSemaphoreTake(repSem, portMAX_DELAY);
        // Dequeue the report from the queue
        StadiaRep_t *rep = dequeue_stadia_rep(repQueue);
        // Update the controller state with the new report
        update_controller(&state, rep);
        // Free the report
        free(rep);
    }
}