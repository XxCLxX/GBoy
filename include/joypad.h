#pragma once
#include <common.h>

/*
Bit 7 - Not used
Bit 6 - Not used
Bit 5 - P15 Select Action buttons    (0=Select)
Bit 4 - P14 Select Direction buttons (0=Select)
Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)
*/

typedef struct
{
    bool start;  // Bit 3
    bool select; // Bit 2
    bool a;      // Bit 0
    bool b;      // Bit 1
    bool up;     // Bit 2
    bool down;   // Bit 3
    bool left;   // Bit 1
    bool right;  // Bit 0
} joypad_state;

void joypad_init();
bool joypad_button_sel();
bool joypad_direction_sel();
void joypad_set_sel(u8 value);

joypad_state *get_joypad_state();
u8 joypad_get_output();