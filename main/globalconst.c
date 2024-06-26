/**
 * @file globalconst.c
 * @brief Global constants and definitions for the ESP32C6 to connect to a
 *        Google Stadia Controller over BLE.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#include <stdbool.h>
#include "globalconst.h"
#include "driver/uart.h"

// The UART port to output notifications on
const uart_port_t uart_num = UART_NUM_0;

// One entry for each control on the controller indicating whether or not to
// publish notifications for the control state
bool publish_controls[20] = {
    true,   // DPD
    false,  // RSB
    false,  // OPT
    false,  // MEN
    false,  // STB
    false,  // RTB
    false,  // LTB
    false,  // GAS
    false,  // CPT
    true,   // LAB
    true,   // LBB
    true,   // LXB
    true,   // LYB
    true,   // LBP
    true,   // RBP
    false,  // LSB
    true,   // LJS
    true,   // RJS
    true,   // LTR
    true    // RTR
};
