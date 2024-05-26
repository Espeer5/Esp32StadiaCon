/**
 * @file rep_queue.c
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

#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/semphr.h"

// Synchronize access to the queue
SemaphoreHandle_t repQueueMutex;

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

RepQueue_t *create_stadia_rep_queue() {
    RepQueue_t *newQueue = malloc(sizeof(RepQueue_t));
    if (newQueue == NULL) {
        return NULL;
    }
    repQueueMutex = xSemaphoreCreateMutex();
    if (repQueueMutex == NULL) {
        free(newQueue);
        return NULL;
    }
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    return newQueue;
}

void insert_stadia_rep(RepQueue_t *queue, StadiaRep_t *rep) {
    xSemaphoreTake(repQueueMutex, portMAX_DELAY);
    RepQueueElement_t *newElement = malloc(sizeof(RepQueueElement_t));
    if (newElement == NULL) {
        xSemaphoreGive(repQueueMutex);
        return;
    }
    newElement->rep = rep;
    newElement->next = NULL;
    if (queue->size == 0) {
        queue->head = newElement;
        queue->tail = newElement;
    } else {
        queue->tail->next = newElement;
        queue->tail = newElement;
    }
    queue->size++;
    xSemaphoreGive(repQueueMutex);
}

StadiaRep_t *dequeue_stadia_rep(RepQueue_t *queue) {
    xSemaphoreTake(repQueueMutex, portMAX_DELAY);
    if (queue->size == 0) {
        xSemaphoreGive(repQueueMutex);
        return NULL;
    }
    RepQueueElement_t *element = queue->head;
    StadiaRep_t *rep = element->rep;
    queue->head = element->next;
    free(element);
    queue->size--;
    xSemaphoreGive(repQueueMutex);
    return rep;
}

void print_rep_queue(RepQueue_t *queue) {
    xSemaphoreTake(repQueueMutex, portMAX_DELAY);
    RepQueueElement_t *current = queue->head;
    while (current != NULL) {
        print_stadia_rep(current->rep);
        current = current->next;
    }
    xSemaphoreGive(repQueueMutex);
}
