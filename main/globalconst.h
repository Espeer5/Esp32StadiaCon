/**
 * @file globalconst.h
 * @brief Global constants and definitions for the ESP32C6 to connect to a
 *        Google Stadia Controller over BLE.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#ifndef _GLOBALCONST_H_
#define _GLOBALCONST_H_

#include <stdbool.h>
#include "driver/uart.h"

// One entry for each control on the controller indicating whether or not to
// publish notifications for the control state
extern bool publish_controls[20];

// The UART port to output notifications on
extern const uart_port_t uart_num;

// Tag for the GATT client. Used for logging globally, not just within gatt
// client files.
#define GATTC_TAG "STADIA_CON_CLIENT"

// Device name for the Google Stadia Controller.
#define remote_device_name "StadiaBWVQ-855f"

// Toggle debug logging for the GATT client
#define GATTC_DEBUG false

// Toggle debug logging for UART output
#define UART_DEBUG false

#endif /* #ifndef _GLOBALCONST_H_ */
