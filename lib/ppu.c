#include <ppu.h>
#include <lcd.h>
#include <string.h>
#include <ppu_state_machine.h>

void fifo_reset();
void fifo_process();

static ppu_context ctx;

ppu_context *get_ppu_context()
{
    return &ctx;
}

void ppu_init()
{
    ctx.current_frame = 0;
    ctx.line_ticks = 0;
    ctx.video_buffer = malloc(Y_RES * X_RES * sizeof(32));

    ctx.pfc.line_x = 0;
    ctx.pfc.push_x = 0;
    ctx.pfc.fetch_x = 0;
    ctx.pfc.pixel_fifo.size = 0;
    ctx.pfc.pixel_fifo.front = ctx.pfc.pixel_fifo.back = NULL;
    ctx.pfc.cur_fetch_state = FS_TILE;

    ctx.line_sprites = 0;
    ctx.fetched_entry_count = 0;

    lcd_init();
    LCDS_MODE_SET(MODE_OAM);

    memset(ctx.oam_ram, 0, sizeof(ctx.oam_ram));
    memset(ctx.video_buffer, 0, Y_RES * X_RES * sizeof(u32));
}

void ppu_run()
{
    ctx.line_ticks++;

    switch (LCDS_MODE)
    {
    case MODE_OAM:
        state_mode_oam();
        break;

    case MODE_DRAW:
        state_mode_draw();
        break;

    case MODE_VBLANK:
        state_mode_vblank();
        break;

    case MODE_HBLANK:
        state_mode_hblank();
        break;
    }
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