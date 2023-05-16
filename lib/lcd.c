#include <lcd.h>
#include <ppu.h>
#include <dma.h>

static lcd_context ctx;

// static int current_color_scheme = 0;
// static unsigned long default_colour[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

static unsigned long default_colour[][4] = {
    {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000}, // Monochrome
    {0xFFFFF6D3, 0xFFF9A875, 0xFFEB6B6F, 0xFF7C3F58}, // Orange Sunset
    {0xFFD0D058, 0xFFA0A840, 0xFF708028, 0xFF405010}, // Nostalgia Green
    {0xFFCCA66E, 0xFF99683D, 0xFF664930, 0xFF332920}  // Brown Palette
};

void lcd_init()
{
    ctx.lcdc = 0x91;
    // ctx.lcds = 0;
    ctx.scroll_y = 0;
    ctx.scroll_x = 0;
    ctx.ly = 0;
    ctx.ly_compare = 0;
    ctx.bgp = 0xFC;
    ctx.obp[0] = 0xFF;
    ctx.obp[1] = 0xFF;
    ctx.WY = 0;
    ctx.WX = 0;
    ctx.current_color_scheme;

    for (int i = 0; i < 4; i++)
    {
        // ctx.bg_colour[i] = &default_colour[ctx.current_color_scheme][i];
        // ctx.sprite1_colour[i] = &default_colour[ctx.current_color_scheme][i];
        // ctx.sprite2_colour[i] = &default_colour[ctx.current_color_scheme][i];

        ctx.bg_colour[i] = default_colour[ctx.current_color_scheme][i];
        ctx.sprite1_colour[i] = default_colour[ctx.current_color_scheme][i];
        ctx.sprite2_colour[i] = default_colour[ctx.current_color_scheme][i];
    }
}

lcd_context *get_lcd_context()
{
    return &ctx;
}

u8 lcd_read(u16 address)
{
    u8 offset = (address - 0xFF40);
    u8 *p = (u8 *)&ctx;

    return p[offset];
}

/*
    Bit 7-6 - Color for index 3
    Bit 5-4 - Color for index 2
    Bit 3-2 - Color for index 1
    Bit 1-0 - Color for index 0
*/
void update_palette_data(u8 palette_data, u8 col)
{
    u32 *colour = ctx.bg_colour;

    switch (col)
    {
    case 1:
        colour = ctx.sprite1_colour;
        break;

    case 2:
        colour = ctx.sprite2_colour;
        break;
    }

    colour[0] = default_colour[get_lcd_context()->current_color_scheme][palette_data & 0b11];
    colour[1] = default_colour[get_lcd_context()->current_color_scheme][(palette_data >> 2) & 0b11];
    colour[2] = default_colour[get_lcd_context()->current_color_scheme][(palette_data >> 4) & 0b11];
    colour[3] = default_colour[get_lcd_context()->current_color_scheme][(palette_data >> 6) & 0b11];
}

void lcd_write(u16 address, u8 value)
{
    u8 offset = (address - 0xFF40);
    u8 *p = (u8 *)&ctx;
    p[offset] = value;

    // 0xFF46
    if (offset == 6)
    {
        dma_start_address(value);
    }

    if (address == 0xFF47)
    {
        update_palette_data(value, 0);
    }
    else if (address == 0xFF48)
    {
        update_palette_data(value & 0b11111100, 1);
    }
    else if (address == 0xFF49)
    {
        update_palette_data(value & 0b11111100, 2);
    }
    if (address == 0xFF50)
    {
        // Handle color scheme changes
        ctx.current_color_scheme = value;
    }
}