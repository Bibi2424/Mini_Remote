#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

typedef struct {
    uint8_t pin;
    int16_t value;
    bool inverted;
}axis_t;


typedef struct {
    axis_t vertical_axis;
    axis_t horizontal_axis;
    int16_t output_range[2];
    uint16_t dead_space;
} joystick_t;


extern void read_joystick(joystick_t *joystick);


#endif