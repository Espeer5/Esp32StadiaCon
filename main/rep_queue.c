/**
 * @file RepQueue.c
 * @brief Method implementations for the RepQueue class.
 * 
 * A RepQueue is a queue of reports that are received from the Google Stadia
 * Controller. The queue is used to store reports until they can be processed
 * and sent out on UART.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/26/24
*/

#include "rep_queue.h"
#include <stdlib.h>
#include <stdio.h>

StadiaRep_t *load_stadia_rep(uint8_t* buffer, size_t len) {
    // Expexted length of controller report is 10 bytes
    if (len != 10) {
        return NULL;
    }
    StadiaRep_t *newRep = malloc(sizeof(StadiaRep_t));
    if (newRep == NULL) {
        return NULL;
    }
    newRep->dpad = buffer[0];
    newRep->buttons1 = buffer[1];
    newRep->buttons2 = buffer[2];
    newRep->stickX = buffer[3];
    newRep->stickY = buffer[4];
    newRep->stickZ = buffer[5];
    newRep->stickRz = buffer[6];
    newRep->brake = buffer[7];
    newRep->throttle = buffer[8];
    newRep->volume = buffer[9];
    return newRep;
}

void print_stadia_rep(StadiaRep_t* rep) {
    printf("Dpad: %x\n", rep->dpad);
    printf("Buttons1: %x\n", rep->buttons1);
    printf("Buttons2: %x\n", rep->buttons2);
    printf("StickX: %x\n", rep->stickX);
    printf("StickY: %x\n", rep->stickY);
    printf("StickZ: %x\n", rep->stickZ);
    printf("StickRz: %x\n", rep->stickRz);
    printf("Brake: %x\n", rep->brake);
    printf("Throttle: %x\n", rep->throttle);
    printf("Volume: %x\n", rep->volume);
}
