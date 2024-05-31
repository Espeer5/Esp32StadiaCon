/**
 * @file    con_state.c
 * @brief   Method implementations for the tracking the controller state over time
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/26/24
*/

#include "con_state.h"
#include "driver/uart.h"
#include "globalconst.h"
#include "rom/ets_sys.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Convert a uint8_t value to a signed percentage for a joystick
 * 
 * This function takes a uint8_t value and converts it to a signed percentage
 * value. The input value is assumed to be in the range of 0 to 255, with 128
 * representing 0%. The output value is in the range of -100% to 100%.
 * 
 * @param val The uint8_t value to convert to a percentage.
 * @return The signed percentage value.
*/
float sign_pct(uint8_t val);

/**
 * @brief Convert a uint8_t value to an unsigned percentage for a trigger
 * 
 * This function takes a uint8_t value and converts it to an unsigned percentage
 * value. The input value is assumed to be in the range of 0 to 255, with 0
 * representing 0%. The output value is in the range of 0% to 100%.
 * 
 * @param val The uint8_t value to convert to a percentage.
 * @return The unsigned percentage value.
*/
float unsign_pct(uint8_t val);

float sign_pct(uint8_t val) {
    return (float) ((int)val - 128) / 128.0 * 100.0;
}

float unsign_pct(uint8_t val) {
    return (float) val / 255.0 * 100.0;
}

char *str_of_button(Button_t* button) {
    // button messages are in the format "ID;PRESSED\n"
    char *str_bldr = malloc(sizeof(char) * 7);
    char *place = str_bldr;
    strncpy(str_bldr, button->id, 3);
    place += 3;
    strcpy(place, ";");
    place += 1;
    strcpy(place, button->pressed ? "1" : "0");
    place += 1;
    strcpy(place, "\n");
    return str_bldr;
}

char *str_of_joystick(Joystick_t* joystick) {
    // joystick messages are in the format "ID;X;Y\n"
    char *str_bldr = malloc(sizeof(char) * 19);
    char *place = str_bldr;
    strncpy(str_bldr, joystick->id, 3);
    place += strlen(joystick->id);
    strcpy(place, ";");
    place += 1;
    char *x_str = malloc(sizeof(char) * 6);
    snprintf(x_str, 6, "%.2f", joystick->x);
    strcpy(place, x_str);
    place += strlen(x_str);
    free(x_str);
    strcpy(place, ";");
    place += 1;
    char *y_str = malloc(sizeof(char) * 6);
    snprintf(y_str, 6, "%.2f", joystick->y);
    strcpy(place, y_str);
    place += strlen(y_str);
    free(y_str);
    strcpy(place, "\n");
    return str_bldr;
}

char *str_of_trigger(Trigger_t* trigger) {
    // trigger messages are in the format "ID;VAL\n"
    char *str_bldr = malloc(sizeof(char) * 11);
    char *place = str_bldr;
    char *extnt_str = malloc(sizeof(char) * 5);
    strncpy(str_bldr, trigger->id, 3);
    place += 3;
    strcpy(place, ";");
    place += 1;
    snprintf(extnt_str, 6, "%.2f", trigger->val);
    strcpy(place, extnt_str);
    place += strlen(extnt_str);
    free(extnt_str);
    strcpy(place, "\n");
    return str_bldr;
}

char *str_of_dpad_dir(DPadDir_t dir) {
    // DPad directions as strings are compass directions.
    switch (dir) {
        case N: {
            return "N";
            break;
        }
        case NE: {
            return "NE";
            break;
        }
        case E: {
            return "E";
            break;
        }
        case SE: {
            return "SE";
            break;
        }
        case S: {
            return "S";
            break;
        }
        case SW: {
            return "SW";
            break;
        }
        case W: {
            return "W";
            break;
        }
        case NW: {
            return "NW";
            break;
        }
        default: {
            return "NO";
            break;
        }
    }
}

char *str_of_dpad(DPad_t* dpad) {
    // dpad messages are in the format "ID;DIR\n"
    char *str_bldr = malloc(sizeof(char) * 7);
    char *place = str_bldr;
    strncpy(str_bldr, dpad->id, 3);
    place += strlen(dpad->id);
    strcpy(place, ";");
    place += 1;
    char *dir_str = str_of_dpad_dir(dpad->dir);
    strcpy(place, dir_str);
    place += strlen(dir_str);
    strcpy(place, "\n");
    return str_bldr;
}

void update_button(Button_t* button, bool value, bool publish) {
    if (button->pressed == value) {
        return;
    }
    button->pressed = value;
    if (publish) {
        char *msg = str_of_button(button);
        if (UART_DEBUG) {
            ets_printf("%s", msg);
        }
        uart_write_bytes(uart_num, msg, strlen(msg));
        free(msg);
    }
    return;
}

void update_joystick(Joystick_t* joystick, float x, float y, bool publish) {
    if (joystick->x == x && joystick->y == y) {
        return;
    }
    joystick->x = x;
    joystick->y = y;
    if (publish) {
        char *msg = str_of_joystick(joystick);
        if (UART_DEBUG) {
            ets_printf("%s", msg);
        }
        uart_write_bytes(uart_num, msg, strlen(msg));
        free(msg);
    }
    return;
}

void update_trigger(Trigger_t* trigger, float value, bool publish) {
    if (trigger->val == value) {
        return;
    }
    trigger->val = value;
    if (publish) {
        char *msg = str_of_trigger(trigger);
        if (UART_DEBUG) {
            ets_printf("%s", msg);
        }
        uart_write_bytes(uart_num, msg, strlen(msg));
        free(msg);
    }
    return;
}

void update_dpad(DPad_t* dpad, DPadDir_t dir, bool publish) {
    if (dpad->dir == dir) {
        return;
    }
    dpad->dir = dir;
    if (publish) {
        char *msg = str_of_dpad(dpad);
        if (UART_DEBUG) {
            ets_printf("%s", msg);
        }
        uart_write_bytes(uart_num, msg, strlen(msg));
        free(msg);
    }
    return;
}

void init_controller(ConState_t *state) {
    // Assign IDs to all components of the controller state
    state->LAB = (Button_t) {"LAB", false};
    state->LBB = (Button_t) {"LBB", false};
    state->LXB = (Button_t) {"LXB", false};
    state->LYB = (Button_t) {"LYB", false};
    state->LTB = (Button_t) {"LTB", false};
    state->RTB = (Button_t) {"RTB", false};
    state->RSB = (Button_t) {"RSB", false};
    state->LSB = (Button_t) {"LSB", false};
    state->STB = (Button_t) {"STB", false};
    state->MEN = (Button_t) {"MEN", false};
    state->CPT = (Button_t) {"CPT", false};
    state->GAS = (Button_t) {"GAS", false};
    state->OPT = (Button_t) {"OPT", false};
    state->RBP = (Button_t) {"RBP", false};
    state->LBP = (Button_t) {"LBP", false};
    state->LJS = (Joystick_t) {"LJS", 0.0, 0.0};
    state->RJS = (Joystick_t) {"RJS", 0.0, 0.0};
    state->LTR = (Trigger_t) {"LTR", 0.0};
    state->RTR = (Trigger_t) {"RTR", 0.0};
    state->DPD = (DPad_t) {"DPD", (DPadDir_t) (0x08)};
}

void update_controller(ConState_t* state, StadiaRep_t* rep) {
    // Store an array indicating whether each component of the state changed
    // We need to update every component of the state with correct report data
    // We will go in order of the report structure

    // DPAD UPDATE
    assert (rep->dpad <= 8);
    update_dpad(&state->DPD, (DPadDir_t) rep->dpad, publish_controls[0]);

    // BUTTONS UPDATE
    update_button(&state->RSB, (rep->buttons1 & 0x80) != 0, publish_controls[1]);
    update_button(&state->OPT, (rep->buttons1 & 0x40) != 0, publish_controls[2]);
    update_button(&state->MEN, (rep->buttons1 & 0x20) != 0, publish_controls[3]);
    update_button(&state->STB, (rep->buttons1 & 0x10) != 0, publish_controls[4]);
    update_button(&state->RTB, (rep->buttons1 & 0x08) != 0, publish_controls[5]);
    update_button(&state->LTB, (rep->buttons1 & 0x04) != 0, publish_controls[6]);
    update_button(&state->GAS, (rep->buttons1 & 0x02) != 0, publish_controls[7]);
    update_button(&state->CPT, (rep->buttons1 & 0x01) != 0, publish_controls[8]);
    update_button(&state->LAB, (rep->buttons2 & 0x40) != 0, publish_controls[9]);
    update_button(&state->LBB, (rep->buttons2 & 0x20) != 0, publish_controls[10]);
    update_button(&state->LXB, (rep->buttons2 & 0x10) != 0, publish_controls[11]);
    update_button(&state->LYB, (rep->buttons2 & 0x08) != 0, publish_controls[12]);
    update_button(&state->LBP, (rep->buttons2 & 0x04) != 0, publish_controls[13]);
    update_button(&state->RBP, (rep->buttons2 & 0x02) != 0, publish_controls[14]);
    update_button(&state->LSB, (rep->buttons2 & 0x01) != 0, publish_controls[15]);

    // JOYSTICKS UPDATE
    update_joystick(&state->LJS, sign_pct(rep->stickX), -sign_pct(rep->stickY),
                    publish_controls[16]);
    update_joystick(&state->RJS, sign_pct(rep->stickZ), -sign_pct(rep->stickRz),
                    publish_controls[17]);
    
    // TRIGGERS UPDATE
    update_trigger(&state->LTR, unsign_pct(rep->brake), publish_controls[18]);
    update_trigger(&state->RTR, unsign_pct(rep->throttle), publish_controls[19]);

    return;
}

void print_controller(ConState_t* state) {
    ets_printf("==============================\n");
    ets_printf("      Controller State:\n");
    ets_printf("LAB: %d\n", state->LAB.pressed);
    ets_printf("LBB: %d\n", state->LBB.pressed);
    ets_printf("LXB: %d\n", state->LXB.pressed);
    ets_printf("LYB: %d\n", state->LYB.pressed);
    ets_printf("LTB: %d\n", state->LTB.pressed);
    ets_printf("RTB: %d\n", state->RTB.pressed);
    ets_printf("RSB: %d\n", state->RSB.pressed);
    ets_printf("LSB: %d\n", state->LSB.pressed);
    ets_printf("STB: %d\n", state->STB.pressed);
    ets_printf("MEN: %d\n", state->MEN.pressed);
    ets_printf("CPT: %d\n", state->CPT.pressed);
    ets_printf("GAS: %d\n", state->GAS.pressed);
    ets_printf("OPT: %d\n", state->OPT.pressed);
    ets_printf("RBP: %d\n", state->RBP.pressed);
    ets_printf("LBP: %d\n", state->LBP.pressed);
    ets_printf("LJS: (%f, %f)\n", state->LJS.x, state->LJS.y);
    ets_printf("RJS: (%f, %f)\n", state->RJS.x, state->RJS.y);
    ets_printf("LTR: %f\n", state->LTR.val);
    ets_printf("RTR: %f\n", state->RTR.val);
    ets_printf("DPD: %d\n", state->DPD.dir);
    ets_printf("==============================\n");
}
