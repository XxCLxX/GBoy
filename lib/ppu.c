#include <ppu.h>

static ppu_context ctx;

void ppu_init()
{
}

void ppu_run()
{
}

void oam_write(u16 address, u8 value)
{
    if (address >= 0xFE00)
    {
        address -= 0xFE00;
    }

    u8 *byte = (u8 *)ctx.oam_ram;
    byte[address] = value;
}

u8 oam_read(u16 address)
{
    if (address >= 0xFE00)
    {
        address -= 0xFE00;
    }

    u8 *byte = (u8 *)ctx.oam_ram;
    return byte[address];
}

void vram_write(u16 address, u8 value)
{
    ctx.vram[address - 0x8000] = value;
}

u8 vram_read(u16 address)
{
    return ctx.vram[address - 0x8000];
}