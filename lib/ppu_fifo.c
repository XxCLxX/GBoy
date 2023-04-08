#include <ppu.h>
#include <lcd.h>
#include <bus.h>

void fifo_push(u32 value)
{
    fifo_node *next = malloc(sizeof(fifo_node));
    next->next = NULL;
    next->value = value;

    if (!get_ppu_context()->pfc.pixel_fifo.front)
    {
        get_ppu_context()->pfc.pixel_fifo.front = get_ppu_context()->pfc.pixel_fifo.back = next;
    }
    else
    {
        get_ppu_context()->pfc.pixel_fifo.back->next = next;
        get_ppu_context()->pfc.pixel_fifo.back = next;
    }
    get_ppu_context()->pfc.pixel_fifo.size++;
}

u32 fifo_pop()
{
    if (get_ppu_context()->pfc.pixel_fifo.size <= 0)
    {
        fprintf(stderr, "FIFO ERROR\n");
        exit(-8);
    }

    fifo_node *pop = get_ppu_context()->pfc.pixel_fifo.front;
    get_ppu_context()->pfc.pixel_fifo.front = pop->next;
    get_ppu_context()->pfc.pixel_fifo.size--;

    u32 value = pop->value;
    free(pop);

    return value;
}

u32 fetch_sprite_pixels(int bit, u32 colour, u8 bg_colour)
{
    for (int i = 0; i < get_ppu_context()->fetched_entry_count; i++)
    {
        int sprite_x = (get_ppu_context()->fetched_entries[i].x - 8) +
                       ((get_lcd_context()->scroll_x % 8));

        if (sprite_x + 8 < get_ppu_context()->pfc.fifo_x)
        {
            continue;
        }

        int offset = get_ppu_context()->pfc.fifo_x - sprite_x;

        if (offset < 0 || offset > 7)
        {
            continue;
        }

        bit = (7 - offset);

        if (get_ppu_context()->fetched_entries[i].flag_x_flip)
        {
            bit = offset;
        }

        u8 high = !!(get_ppu_context()->pfc.fetch_oam_data[i * 2] & (1 << bit));
        u8 low = !!(get_ppu_context()->pfc.fetch_oam_data[(i * 2) + 1] & (1 << bit)) << 1;

        bool bg_priority = get_ppu_context()->fetched_entries[i].flag_bg;

        if (!(high | low))
        {
            continue;
        }

        if (!bg_priority || bg_colour == 0)
        {
            colour = (get_ppu_context()->fetched_entries[i].flag_palette_num) ? get_lcd_context()->sprite2_colour[high | low] : get_lcd_context()->sprite1_colour[high | low];

            if (high | low)
            {
                break;
            }
        }
    }
    return colour;
}

bool fifo_enqueue()
{
    if (get_ppu_context()->pfc.pixel_fifo.size > 8)
    {
        return false;
    }

    int x = get_ppu_context()->pfc.fetch_x - (8 - (get_lcd_context()->scroll_x % 8));

    for (int i = 0; i < 8; i++)
    {
        int bit = 7 - i;
        u8 high = !!(get_ppu_context()->pfc.bgw_fetch_data[1] & (1 << bit));
        u8 low = !!(get_ppu_context()->pfc.bgw_fetch_data[2] & (1 << bit)) << 1;
        u32 colour = get_lcd_context()->bg_colour[high | low];

        if (!LCDC_BGW_ENABLE)
        {
            colour = get_lcd_context()->bg_colour[0];
        }

        if (LCDC_OBJ_ENABLE)
        {
            colour = fetch_sprite_pixels(bit, colour, high | low);
        }

        if (x >= 0)
        {
            fifo_push(colour);
            get_ppu_context()->pfc.fifo_x++;
        }
    }
    return true;
}

void fifo_load_sprite_tile()
{
    oam_line_node *le = get_ppu_context()->line_sprites;

    while (le)
    {
        int sprite_x = (le->entry.x - 8) + (get_lcd_context()->scroll_x % 8);

        if ((sprite_x >= get_ppu_context()->pfc.fetch_x && sprite_x < get_ppu_context()->pfc.fetch_x + 8) ||
            ((sprite_x + 8) >= get_ppu_context()->pfc.fetch_x && (sprite_x + 8) < get_ppu_context()->pfc.fetch_x + 8))
        {
            get_ppu_context()->fetched_entries[get_ppu_context()->fetched_entry_count++] = le->entry;
        }
        le = le->next;
        if (!le || get_ppu_context()->fetched_entry_count >= 3)
        {
            break;
        }
    }
}

void fifo_load_sprite_data(u8 offset)
{
    int current_y = get_lcd_context()->ly;
    u8 sprite_size = LCDC_OBJ_SIZE;

    for (int i = 0; i < get_ppu_context()->fetched_entry_count; i++)
    {
        u8 ty = ((current_y + 16) - get_ppu_context()->fetched_entries[i].y) * 2;

        if (get_ppu_context()->fetched_entries[i].flag_y_flip)
        {
            ty = ((sprite_size * 2) - 2) - ty;
        }

        u8 tile_index = get_ppu_context()->fetched_entries[i].tile;

        if (sprite_size == 16)
        {
            tile_index &= ~(1);
        }

        get_ppu_context()->pfc.fetch_oam_data[(i * 2) + offset] = bus_read(0x8000 + (tile_index * 16) + ty + offset);
    }
}

/*
    5 States:
    + Get tile
    + Get tile data low
    + Get tile data high
    + Sleep
    + Push
*/
void fifo_fetcher()
{
    switch (get_ppu_context()->pfc.cur_fetch_state)
    {
    case FS_TILE:
    {
        get_ppu_context()->fetched_entry_count = 0;
        if (LCDC_BGW_ENABLE)
        {
            get_ppu_context()->pfc.bgw_fetch_data[0] = bus_read(LCDC_BG_MAP_AREA +
                                                                (get_ppu_context()->pfc.map_x / 8) +
                                                                (((get_ppu_context()->pfc.map_y / 8)) * 32));

            if (LCDC_BGW_DATA_AREA == 0x8800)
            {
                get_ppu_context()->pfc.bgw_fetch_data[0] += 128;
            }
        }

        if (LCDC_OBJ_ENABLE && get_ppu_context()->line_sprites)
        {
            fifo_load_sprite_tile();
        }

        get_ppu_context()->pfc.cur_fetch_state = FS_DATA_LOW;
        get_ppu_context()->pfc.fetch_x += 8;
    }
    break;

    case FS_DATA_LOW:
    {
        get_ppu_context()->pfc.bgw_fetch_data[1] = bus_read(LCDC_BGW_DATA_AREA +
                                                            (get_ppu_context()->pfc.bgw_fetch_data[0] * 16) +
                                                            get_ppu_context()->pfc.tile_y);

        fifo_load_sprite_data(0);
        get_ppu_context()->pfc.cur_fetch_state = FS_DATA_HIGH;
    }
    break;

    case FS_DATA_HIGH:
    {
        get_ppu_context()->pfc.bgw_fetch_data[2] = bus_read(LCDC_BGW_DATA_AREA +
                                                            (get_ppu_context()->pfc.bgw_fetch_data[0] * 16) +
                                                            get_ppu_context()->pfc.tile_y + 1);

        fifo_load_sprite_data(1);
        get_ppu_context()->pfc.cur_fetch_state = FS_SLEEP;
    }
    break;

    case FS_SLEEP:
    {
        get_ppu_context()->pfc.cur_fetch_state = FS_PUSH;
    }
    break;

    case FS_PUSH:
    {
        if (fifo_enqueue())
        {
            get_ppu_context()->pfc.cur_fetch_state = FS_TILE;
        }
    }
    break;
    }
}

void pixel_push()
{
    if (get_ppu_context()->pfc.pixel_fifo.size > 8)
    {
        u32 pixel_data = fifo_pop();

        if (get_ppu_context()->pfc.line_x >= (get_lcd_context()->scroll_x % 8))
        {
            get_ppu_context()->video_buffer[get_ppu_context()->pfc.push_x +
                                            (get_lcd_context()->ly * X_RES)] = pixel_data;

            get_ppu_context()->pfc.push_x++;
        }
        get_ppu_context()->pfc.line_x++;
    }
}

void fifo_process()
{
    get_ppu_context()->pfc.map_y = (get_lcd_context()->ly + get_lcd_context()->scroll_y);
    get_ppu_context()->pfc.map_x = (get_ppu_context()->pfc.fetch_x + get_lcd_context()->scroll_x);
    get_ppu_context()->pfc.tile_y = ((get_lcd_context()->ly + get_lcd_context()->scroll_y) % 8) * 2;

    if (!(get_ppu_context()->line_ticks & 1))
    {
        fifo_fetcher();
    }
    pixel_push();
}

void fifo_reset()
{
    while (get_ppu_context()->pfc.pixel_fifo.size)
    {
        fifo_pop();
    }
    get_ppu_context()->pfc.pixel_fifo.front = 0;
}