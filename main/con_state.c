/**
 * @file    con_state.c
 * @brief   Method implementations for the tracking the controller state over time
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/26/24
*/

#include "con_state.h"
#include <stdlib.h>
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

int update_button(Button_t* button, bool value) {
    if (button->pressed == value) {
        return 0;
    }
    button->pressed = value;
    return 1;
}

int update_joystick(Joystick_t* joystick, float x, float y) {
    if (joystick->x == x && joystick->y == y) {
        return 0;
    }
    joystick->x = x;
    joystick->y = y;
    return 1;
}

int update_trigger(Trigger_t* trigger, float value) {
    if (trigger->val == value) {
        return 0;
    }
    trigger->val = value;
    return 1;
}

int update_dpad(DPad_t* dpad, DPadDir_t dir) {
    if (dpad->dir == dir) {
        return 0;
    }
    dpad->dir = dir;
    return 1;
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

int *update_controller(ConState_t* state, StadiaRep_t* rep) {
    // Store an array indicating whether each component of the state changed
    int *changed = malloc(sizeof(int) * 20);
    // We need to update every component of the state with correct report data
    // We will go in order of the report structure

    // DPAD UPDATE
    assert (rep->dpad <= 8);
    changed[0]  = update_dpad(&state->DPD, (DPadDir_t) rep->dpad);

    // BUTTONS UPDATE
    changed[1]  = update_button(&state->RSB, (rep->buttons1 & 0x80) != 0);
    changed[2]  = update_button(&state->OPT, (rep->buttons1 & 0x40) != 0);
    changed[3]  = update_button(&state->MEN, (rep->buttons1 & 0x20) != 0);
    changed[4]  = update_button(&state->STB, (rep->buttons1 & 0x10) != 0);
    changed[5]  = update_button(&state->RTB, (rep->buttons1 & 0x08) != 0);
    changed[6]  = update_button(&state->LTB, (rep->buttons1 & 0x04) != 0);
    changed[7]  = update_button(&state->GAS, (rep->buttons1 & 0x02) != 0);
    changed[8]  = update_button(&state->CPT, (rep->buttons1 & 0x01) != 0);
    changed[9]  = update_button(&state->LAB, (rep->buttons2 & 0x40) != 0);
    changed[10] = update_button(&state->LBB, (rep->buttons2 & 0x20) != 0);
    changed[11] = update_button(&state->LXB, (rep->buttons2 & 0x10) != 0);
    changed[12] = update_button(&state->LYB, (rep->buttons2 & 0x08) != 0);
    changed[13] = update_button(&state->LBP, (rep->buttons2 & 0x04) != 0);
    changed[14] = update_button(&state->RBP, (rep->buttons2 & 0x02) != 0);
    changed[15] = update_button(&state->LSB, (rep->buttons2 & 0x01) != 0);

    // JOYSTICKS UPDATE
    changed[16] = update_joystick(&state->LJS, sign_pct(rep->stickX),
                                  -sign_pct(rep->stickY));
    changed[17] = update_joystick(&state->RJS, sign_pct(rep->stickZ),
                                    -sign_pct(rep->stickRz));
    
    // TRIGGERS UPDATE
    changed[18] = update_trigger(&state->LTR, unsign_pct(rep->brake));
    changed[19] = update_trigger(&state->RTR, unsign_pct(rep->throttle));

    return changed;
}

void print_controller(ConState_t* state) {
    printf("==============================\n");
    printf("      Controller State:\n");
    printf("LAB: %d\n", state->LAB.pressed);
    printf("LBB: %d\n", state->LBB.pressed);
    printf("LXB: %d\n", state->LXB.pressed);
    printf("LYB: %d\n", state->LYB.pressed);
    printf("LTB: %d\n", state->LTB.pressed);
    printf("RTB: %d\n", state->RTB.pressed);
    printf("RSB: %d\n", state->RSB.pressed);
    printf("LSB: %d\n", state->LSB.pressed);
    printf("STB: %d\n", state->STB.pressed);
    printf("MEN: %d\n", state->MEN.pressed);
    printf("CPT: %d\n", state->CPT.pressed);
    printf("GAS: %d\n", state->GAS.pressed);
    printf("OPT: %d\n", state->OPT.pressed);
    printf("RBP: %d\n", state->RBP.pressed);
    printf("LBP: %d\n", state->LBP.pressed);
    printf("LJS: (%f, %f)\n", state->LJS.x, state->LJS.y);
    printf("RJS: (%f, %f)\n", state->RJS.x, state->RJS.y);
    printf("LTR: %f\n", state->LTR.val);
    printf("RTR: %f\n", state->RTR.val);
    printf("DPD: %d\n", state->DPD.dir);
    printf("==============================\n");
}
