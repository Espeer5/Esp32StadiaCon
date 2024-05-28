# StadiaCon

This package uses the esp32 Bluedroid stack to connect esp32C6 development board to a Google Stadia controller with the generic ble firmware installed on it.

## Structure

The ble utility functions are organized into modules:
 - auth_gap: Sets up low-level GAP profile for connecting to the Google Stadia controller using proper security and authentication
 - gattc.c: Sets up a GATT client profile which establishes notification on and reads HID reports from the Stadia controller
 - bt_init: Methods used to fully intialize the bluetooth hardware, bluetooth stack, GATT and GAP profiles, and ble authentication
