/**
 * @file RepQueue.h
 * @brief Method prototypes for the RepQueue class.
 * 
 * A RepQueue is a queue of reports that are received from the Google Stadia 
 * Controller. The queue is used to store reports until they can be processed 
 * and sent out on UART.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/26/24
*/

#ifndef _REP_QUEUE_H_
#define _REP_QUEUE_H_

#include <stdint.h>
#include <stddef.h>

/**
 * STADIA HID REPORT STRUCTURE
 * The HID report from the Stadia controller is a 10 byte sequence. The bytes 
 * are as follows:
 *  - Byte 1: 4 bits of D-pad, 4 bits of padding
 *  - Byte 2: 8 bits of buttons
 *  - Byte 3: 7 bits of buttons, 1 bit of padding
 *  - Byte 4: 8 bits pointer X on stick
 *  - Byte 5: 8 bits pointer Y on stick
 *  - Byte 6: 8 bits pointer Z on stick
 *  - Byte 7: 8 bits pointer Rz on stick
 *  - Byte 8: 8 bits of Brake trigger
 *  - Byte 9: 8 bits of Throttle trigger
 *  - Byte 10: 3 bits for volume, play/pause, 5 bits of padding
*/

typedef struct StadiaRep {
    uint8_t dpad;
    uint8_t buttons1;
    uint8_t buttons2;
    uint8_t stickX;
    uint8_t stickY;
    uint8_t stickZ;
    uint8_t stickRz;
    uint8_t brake;
    uint8_t throttle;
    uint8_t volume;
} StadiaRep_t;

/**
 * @brief Load a StadiaRep from a buffer.
 * 
 * @param rep The StadiaRep to load into.
 * @param buffer The buffer to load from.
 * @param len The length of the buffer.
 * @return a pointer to the StadiaRep if successful, NULL otherwise.
*/
StadiaRep_t *load_stadia_rep(uint8_t* buffer, size_t len);

/**
 * @brief Print a StadiaRep to the console for debugging.
 * 
 * @param rep A pointer to the StadiaRep to print.
*/
void print_stadia_rep(StadiaRep_t* rep);

#endif /* #ifndef _REP_QUEUE_H_ */
