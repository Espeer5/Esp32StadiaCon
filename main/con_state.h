/**
 * @file    con_state.h
 * @brief   Definitions and method prototypes of ConState, a struct representing 
 *          the previous saved state of the controller.
 * 
 * @version V1.0
 * @author  Edward Speer
 * @date    05/26/24
*/
#ifndef CON_STATE_H
#define CON_STATE_H

#include <stdint.h>
#include <stddef.h>
#include "rep_queue.h"

/**
 * @brief Represents a button on the controller.
 * 
 * This struct is used to represent a button on the controller. It contains a
 * boolean value to represent if the button is pressed or not. Each button also
 * has a unique string identifier of length 2 to represent the button in ASCII.
*/
typedef struct Button {
    char id[3];   // The unique identifier of the button.
    bool pressed; // True if the Button is pressed, false otherwise.
} Button_t;

/**
 * @brief Returns the string representation of a button.
 * 
 * This function is used to return the string representation of a button. The
 * function takes a pointer to a Button_t struct and returns a pointer to a string
 * representation of the button.
 * 
 * @param button A pointer to the Button_t struct to get the string representation of.
 * @return A pointer to a string representation of the button.
*/
char *str_of_button(Button_t* button);

/**
 * @brief Represents a joystick on the controller.
 * 
 * This struct is used to represent a joystick on the controller. It contains
 * two values, x and y, to represent the position of the joystick. Since we want 
 * to output joystick positions as ASCII, we store the x and y values as
 * perentages of maximum values. For example, a stick all the way left would be 
 * -100% along the x-axis, and a stick all the way up would be 100% along the
 * y-axis. Each stick also has a unique string identifier of length 2 to
 * represent the stick in ASCII.
*/
typedef struct Joystick {
    char id[3]; // The unique identifier of the joystick.
    float x;    // The x position of the joystick as a percentage.
    float y;    // The y position of the joystick as a percentage.
} Joystick_t;

/**
 * @brief Returns the string representation of a joystick.
 * 
 * This function is used to return the string representation of a joystick. The
 * function takes a pointer to a Joystick_t struct and returns a pointer to a
 * string representation of the joystick.
 * 
 * @param joystick A pointer to the Joystick_t struct to get the string representation of.
 * @return A pointer to a string representation of the joystick.
*/
char *str_of_joystick(Joystick_t* joystick);

/**
 * @brief Represents a trigger on the controller.
 * 
 * This struct is used to represent a trigger on the controller. It contains a
 * value to represent the position of the trigger. Since we want to output
 * trigger positions as ASCII, we store the trigger value as a percentage of the
 * maximum value. For example, a trigger pressed all the way down would be 100%.
 * Each trigger also has a unique string identifier of length 2 to represent the
 * trigger in ASCII.
*/
typedef struct Trigger {
    char id[3];   // The unique identifier of the trigger.
    float val;    // The position of the trigger as a percentage.
} Trigger_t;

/**
 * @brief Returns the string representation of a trigger.
 * 
 * This function is used to return the string representation of a trigger. The
 * function takes a pointer to a Trigger_t struct and returns a pointer to a
 * string representation of the trigger.
 * 
 * @param trigger A pointer to the Trigger_t struct to get the string representation of.
 * @return A pointer to a string representation of the trigger.
*/
char *str_of_trigger(Trigger_t* trigger);

/**
 * @brief Represents the possible compass directions of a D-pas.
 * 
 * This enum is used to represent the possible compass directions of a D-pad.
 * The directions are as on a compass, with NORTH being up, EAST being right,
 * SOUTH being down, and WEST being left.
*/
typedef enum DPadDir {
    N,  // The D-pad is pressed up.
    NE, // The D-pad is pressed up and right.
    E,  // The D-pad is pressed right.
    SE, // The D-pad is pressed down and right.
    S,  // The D-pad is pressed down.
    SW, // The D-pad is pressed down and left.
    W,  // The D-pad is pressed left.
    NW, // The D-pad is pressed up and left.
    NO  // The D-pad is not pressed.
} DPadDir_t;

/**
 * @brief Returns the string representation of a D-pad direction.
 * 
 * This function is used to return the string representation of a D-pad dir.
 * The function takes a DPadDir_t value and returns a pointer to a string
 * representation of the direction.
 * 
 * @param dir The DPadDir_t value to get the string representation of.
 * @return A pointer to a string representation of the D-pad direction.
*/
char *str_of_dpad_dir(DPadDir_t dir);

/**
 * @brief Represents a D-pad on the controller.
 * 
 * This struct is used to represent a D-pad on the controller. It contains a 
 * compass direction to represent the position of the D-pad. The D-pad also has
 * a unique string identifier of length 2 to represent the D-pad in ASCII.
*/
typedef struct DPad {
    char id[3];     // The unique identifier of the D-pad.
    DPadDir_t dir;  // The direction the D-pad is pressed.
} DPad_t;

/**
 * @brief Returns the string representation of a D-pad.
 * 
 * This function is used to return the string representation of a D-pad. The
 * function takes a pointer to a DPad_t struct and returns a pointer to a string
 * representation of the D-pad.
 * 
 * @param dpad A pointer to the DPad_t struct to get the string representation of.
 * @return A pointer to a string representation of the D-pad.
*/
char *str_of_dpad(DPad_t* dpad);

/**
 * @brief Represents the state of the Google Stadia controller.
 * 
 * This struct is used to represent the state of the Google Stadia controller.
 * It contains all the buttons, joysticks, triggers, and D-pad of the controller,
 * with all of the unique identifiers for each control on the controller.
 * The identifiers go as follows:
 *  - D-pad: DP
 *  - Joysticks:
 *     - LJS
 *     - RJS
 *  - Triggers:
 *     - LTR
 *     - RTR
 *  - Letter buttons:
 *     - LAB
 *     - LBB
 *     - LXB
 *     - LYB
 *  - Other buttons:
 *     - LTB (Left Digital Trigger) 
 *     - RTB (Right Digital Trigger)
 *     - RSB (Right stick button)
 *     - LSB (Left stick button)
 *     - STB (Stadia button/ on/off button)
 *     - MEN (Menu button)
 *     - CPT (Capture button)
 *     - GAS (Google Assistant button)
 *     - OPT (Option button)
 *     - RBP (Right bumper)
 *     - LBP (Left bumper)
*/
typedef struct ConState {
    Button_t LAB;   // The A button.
    Button_t LBB;   // The B button.
    Button_t LXB;   // The X button.
    Button_t LYB;   // The Y button.
    Button_t LTB;   // The left digital trigger.
    Button_t RTB;   // The right digital trigger.
    Button_t RSB;   // The right stick button.
    Button_t LSB;   // The left stick button.
    Button_t STB;   // The Stadia button.
    Button_t MEN;   // The menu button.
    Button_t CPT;   // The capture button.
    Button_t GAS;   // The Google Assistant button.
    Button_t OPT;   // The option button.
    Button_t RBP;   // The right bumper.
    Button_t LBP;   // The left bumper.
    Joystick_t LJS; // The left joystick.
    Joystick_t RJS; // The right joystick.
    Trigger_t LTR;  // The left trigger.
    Trigger_t RTR;  // The right trigger.
    DPad_t DPD;     // The D-pad.
} ConState_t;

/**
 * @brief Update the state of a button with a new value fetched from a report.
 * 
 * This function is used to update the state of a button with a new value fetched
 * from a report. The function takes a pointer to a Button_t struct and a boolean
 * value to update the button with. The function then updates the button's pressed
 * value with the new value.
 * 
 * @param button A pointer to the Button_t struct to update.
 * @param val The new value to update the button with.
*/
void update_button(Button_t* button, bool val, bool publish);

/**
 * @brief Update the state of a joystick with new values fetched from a report.
 * 
 * This function is used to update the state of a joystick with new values fetched
 * from a report. The function takes a pointer to a Joystick_t struct and two float
 * values to update the joystick with. The function then updates the joystick's x
 * and y values with the new values.
 * 
 * @param joystick A pointer to the Joystick_t struct to update.
 * @param x The new x value to update the joystick with.
 * @param y The new y value to update the joystick with.
*/
void update_joystick(Joystick_t* joystick, float x, float y, bool publish);

/**
 * @brief Update the state of a trigger with a new value fetched from a report.
 * 
 * This function is used to update the state of a trigger with a new value fetched
 * from a report. The function takes a pointer to a Trigger_t struct and a float
 * value to update the trigger with. The function then updates the trigger's val
 * value with the new value.
 * 
 * @param trigger A pointer to the Trigger_t struct to update.
 * @param val The new value to update the trigger with.
 * @param pressed The new pressed value to update the trigger with.
*/
void update_trigger(Trigger_t* trigger, float val, bool publish);

/**
 * @brief Update the state of a D-pad with a new value fetched from a report.
 * 
 * This function is used to update the state of a D-pad with a new value fetched
 * from a report. The function takes a pointer to a DPad_t struct and a DPadDir_t
 * value to update the D-pad with. The function then updates the D-pad's dir value
 * with the new value.
 * 
 * @param dpad A pointer to the DPad_t struct to update.
 * @param dir The new value to update the D-pad with.
*/
void update_dpad(DPad_t* dpad, DPadDir_t dir, bool publish);

/**
 * @brief Initialize the controller state representation with default values.
 * 
 * This function is used to initialize the controller state representation with
 * default values. The function takes a pointer to a ConState_t struct and sets
 * the default values for all the buttons, joysticks, triggers, and D-pad on the
 * controller.
 * 
 * @param state A pointer to the ConState_t struct to initialize.
*/
void init_controller(ConState_t* state);

/**
 * @brief Update the state of a controller with a new report.
 * 
 * This function is used to update the state of a controller with a new report.
 * The function takes a pointer to a ConState_t struct and a pointer to a StadiaRep_t
 * struct to update the controller with. The function then updates the controller's
 * buttons, joysticks, triggers, and D-pad with the new values from the report.
 * 
 * @param state A pointer to the ConState_t struct to update.
 * @param rep A pointer to the StadiaRep_t struct to update the controller with.
*/
void update_controller(ConState_t* state, StadiaRep_t* rep);

/**
 * @brief Print a controller state to the console for debugging puposes.
 * 
 * This function is used to print a controller state to the console for debugging
 * purposes. The function takes a pointer to a ConState_t struct and prints the
 * state of all the buttons, joysticks, triggers, and D-pad on the controller.
 * 
 * @param state A pointer to the ConState_t struct to print.
*/
void print_controller(ConState_t* state);

#endif /* #ifndef CON_STATE_H */
