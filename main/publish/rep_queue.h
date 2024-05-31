/**
 * @file rep_queue.h
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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Load in the global counting sempahore for the report queue
extern SemaphoreHandle_t repSem;

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
 *  - Byte 10: 3 bits for volume, play/pause, 5 bits of padding. (Used when a
 *             headset is connected directly to the controller. Not used in this
 *             project.)
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

/**
 * @brief A queue element for a queue of stadia reports.
 * 
 * This struct is used to store a StadiaRep and a pointer to the next element
 * in the queue.
*/
typedef struct RepQueueElement {
    StadiaRep_t *rep;
    struct RepQueueElement *next;
} RepQueueElement_t;

/*
 * @brief A queue of Stadia reports.
 * 
 * This struct is used to store a queue of Stadia reports. The queue is a 
 * singly linked list of StadiaQueueElements.
*/
typedef struct RepQueue {
    RepQueueElement_t *head;
    RepQueueElement_t *tail;
    size_t size;
} RepQueue_t;

// The global report queue
extern RepQueue_t *repQueue;

/**
 * @brief Create a new StadiaQueue.
 * 
 * @return a pointer to the new StadiaQueue.
*/
RepQueue_t *create_stadia_rep_queue();

/**
 * @brief Insert a StadiaRep into the queue.
 * 
 * @param queue The queue to insert into.
 * @param rep The StadiaRep to insert.
*/
void insert_stadia_rep(RepQueue_t *queue, StadiaRep_t *rep);

/**
 * @brief Dequeue a StadiaRep from the queue.
 * 
 * @param queue The queue to remove from.
 * @return a pointer to the StadiaRep that was removed.
*/
StadiaRep_t *dequeue_stadia_rep(RepQueue_t *queue);

/**
 * @brief Print a RepQueue to the console for debugging.
 * 
 * @param queue The RepQueue to print.
*/
void print_rep_queue(RepQueue_t *queue);

#endif /* #ifndef _REP_QUEUE_H_ */
