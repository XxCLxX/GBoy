#pragma once
#include <common.h>
// https://gbdev.io/pandocs/OAM.html

typedef struct
{
    u8 y;
    u8 x;
    u8 tile;

    // Attributes-Flags
    unsigned flag_palette : 3; // CGB Mode Only
    unsigned flag_vram : 1;    // CGB Model Only
    unsigned flag_x_flip : 1;
    unsigned flag_y_flip : 1;
    unsigned flag_bg : 1;
} oam_context;

typedef struct
{
    oam_context oam_ram[40];
    u8 vram[0x2000];
} ppu_context;

void ppu_init();
void ppu_run();

void oam_write(u16 address, u8 value);
u8 oam_read(u16 address);

void vram_write(u16 address, u8 value);
u8 vram_read(u16 address);