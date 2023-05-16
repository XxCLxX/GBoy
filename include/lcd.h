#pragma once
#include <common.h>
// https://gbdev.io/pandocs/STAT.html

typedef struct
{
    u8 lcdc;       // FF40 - Control
    u8 lcds;       // FF41 - STAT
    u8 scroll_y;   // FF42
    u8 scroll_x;   // FF43
    u8 ly;         // FF44
    u8 ly_compare; // FF45
    u8 dma;        // FF46
    u8 bgp;        // FF47 - BG palette data
    u8 obp[2];     // FF48 - Object palette data
    u8 WY;         // Window Y position
    u8 WX;         // Window X position

    //unsigned long
    u32 bg_colour[4];
    u32 sprite1_colour[4];
    u32 sprite2_colour[4];

    //unsigned int
    u32 current_color_scheme;
} lcd_context;

typedef enum
{
    MODE_HBLANK,
    MODE_VBLANK,
    MODE_OAM,
    MODE_DRAW
} lcd_mode;

lcd_context *get_lcd_context();

#define LCDC_BGW_ENABLE (BIT(get_lcd_context()->lcdc, 0))
#define LCDC_OBJ_ENABLE (BIT(get_lcd_context()->lcdc, 1))
#define LCDC_OBJ_SIZE (BIT(get_lcd_context()->lcdc, 2) ? 16 : 8)
#define LCDC_BG_MAP_AREA (BIT(get_lcd_context()->lcdc, 3) ? 0x9C00 : 0x9800)
#define LCDC_BGW_DATA_AREA (BIT(get_lcd_context()->lcdc, 4) ? 0x8000 : 0x8800)
#define LCDC_WIN_ENABLE (BIT(get_lcd_context()->lcdc, 5))
#define LCDC_WIN_MAP_AREA (BIT(get_lcd_context()->lcdc, 6) ? 0x9C00 : 0x9800)
#define LCDC_LCD_ENABLE (BIT(get_lcd_context()->lcdc, 7))

#define LCDS_MODE ((lcd_mode)(get_lcd_context()->lcds & 0b11))
#define LCDS_MODE_SET(mode)               \
    {                                     \
        get_lcd_context()->lcds &= ~0b11; \
        get_lcd_context()->lcds |= mode;  \
    }

#define LCDS_LYC (BIT(get_lcd_context()->lcds, 2))
#define LCDS_LYC_SET(b) (BIT_SET(get_lcd_context()->lcds, 2, b))

typedef enum
{
    STAT_HBLANK = (1 << 3),
    STAT_VBLANK = (1 << 4),
    STAT_OAM = (1 << 5),
    STAT_LYC = (1 << 6)
} stat_source;

#define LCDS_STAT_INT(source) (get_lcd_context()->lcds & source)

void lcd_init();

u8 lcd_read(u16 address);
void lcd_write(u16 address, u8 value);