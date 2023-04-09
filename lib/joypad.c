#include <string.h>
#include <joypad.h>
//https://gbdev.io/pandocs/Joypad_Input.html\

typedef struct
{
    bool button_select;
    bool direction_select;
    joypad_state controller;
} joypad_context;

static joypad_context ctx = {0};

bool joypad_button_sel()
{
    return ctx.button_select;
}

bool joypad_direction_sel()
{
    return ctx.direction_select;
}

void joypad_set_sel(u8 value)
{
    ctx.button_select = value & 0x20;
    ctx.direction_select = value & 0x10;
}

joypad_state *get_joypad_state()
{
    return &ctx.controller;
}

u8 joypad_get_output()
{
    u8 output = 0xCF;

    if (!joypad_button_sel())
    {
        if (get_joypad_state()->start)
        {
            output &= ~(1 << 3);
        }
        else if (get_joypad_state()->select)
        {
            output &= ~(1 << 2);
        }
        else if (get_joypad_state()->a)
        {
            output &= ~(1 << 0);
        }
        else if (get_joypad_state()->b)
        {
            output &= ~(1 << 1);
        }
    }

    if (!joypad_direction_sel())
    {
        if (get_joypad_state()->left)
        {
            output &= ~(1 << 1);
        }
        else if (get_joypad_state()->right)
        {
            output &= ~(1 << 0);
        }
        else if (get_joypad_state()->up)
        {
            output &= ~(1 << 2);
        }
        else if (get_joypad_state()->down)
        {
            output &= ~(1 << 3);
        }
    }
    return output;
}