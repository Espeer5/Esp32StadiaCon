/**
 * @file main.c
 * @brief Main file for the ESP32C6 to connect to a Google Stadia Controller
 *        over BLE.
*/

#include "ble/bt_init.h"
#include "ble/auth_gap.h"
#include "publish/rep_queue.h"
#include "publish/con_state.h"
#include "globalconst.h"

#include "freertos/freeRTOS.h"
#include "freertos/semphr.h"
#include "driver/uart.h"

// The incoming bluetooth report queue
RepQueue_t *repQueue;

// A counting semaphore used to notify the main thread that a new report is
// available in the queue
SemaphoreHandle_t repSem;

// The controller state
ConState_t state;

// The UART communication parameters
uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
};

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
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));
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