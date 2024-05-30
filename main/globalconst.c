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

// One entry for each control on the controller indicating whether or not to
// publish notifications for the control state
bool publish_controls[20] = {
    true,   // LAB
    true,   // LBB
    true,   // LXB
    true,   // LYB
    false,  // LTB
    false,  // RTB
    false,  // RSB
    false,  // LSB
    true,   // STB
    false,  // MEN
    false,  // CPT
    false,  // GAS
    false,  // OPT
    true,   // RBP
    true,   // LBP
    true,   // LJS
    true,   // RJS
    true,   // LTR
    true,   // RTR
    true    // DPD
};
