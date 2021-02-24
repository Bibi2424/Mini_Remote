#include "joystick.h"


static void read_one_axis(axis_t *axis, int16_t *range, uint16_t dead_sapce) {
    uint16_t raw_value = analogRead(axis->pin);
    
    if (axis->inverted) { raw_value = 1024 - raw_value; }

    raw_value = map(raw_value, 0, 1024, range[0], range[1]);
    if(raw_value > 0 && raw_value < dead_sapce) { raw_value = 0; }
    if(raw_value < 0 && raw_value > -dead_sapce) { raw_value = 0; }

    axis->value = raw_value;
}


extern void read_joystick(joystick_t *joystick) {
    read_one_axis(&joystick->vertical_axis, joystick->output_range, joystick->dead_space);
    read_one_axis(&joystick->horizontal_axis, joystick->output_range, joystick->dead_space);
}
