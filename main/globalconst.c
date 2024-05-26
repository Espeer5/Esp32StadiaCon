/**
 * @file globalconst.c
 * @brief Global constants and definitions for the ESP32C6 to connect to a
 *        Google Stadia Controller over BLE.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/25/24
*/

#ifndef _GLOBALCONST_H_
#define _GLOBALCONST_H_

// Tag for the GATT client. Used for logging globally, not just within gatt
// client files.
#define GATTC_TAG "STADIA_CON_CLIENT"

// Device name for the Google Stadia Controller.
#define remote_device_name "StadiaBWVQ-855f"

#endif /* #ifndef _GLOBALCONST_H_ */