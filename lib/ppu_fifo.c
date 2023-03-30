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

bool fifo_add()
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

        if (x >= 0)
        {
            fifo_push(colour);
            get_ppu_context()->pfc.fifo_x++;
        }
    }
    return true;
}

/*
    5 States:
    + Get tile
    + Get tile data low
    + Get tile data high
    + Sleep
    + Push
*/
void fifo_fetch()
{
    switch (get_ppu_context()->pfc.cur_fetch_state)
    {
    case FS_TILE:
    {
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
        get_ppu_context()->pfc.cur_fetch_state = FS_DATA_LOW;
        get_ppu_context()->pfc.fetch_x += 8;
    }
    break;

    case FS_DATA_LOW:
    {
        get_ppu_context()->pfc.bgw_fetch_data[1] = bus_read(LCDC_BGW_DATA_AREA +
                                                            (get_ppu_context()->pfc.bgw_fetch_data[0] * 16) +
                                                            get_ppu_context()->pfc.tile_y);

        get_ppu_context()->pfc.cur_fetch_state = FS_DATA_HIGH;
    }
    break;

    case FS_DATA_HIGH:
    {
        get_ppu_context()->pfc.bgw_fetch_data[2] = bus_read(LCDC_BGW_DATA_AREA +
                                                            (get_ppu_context()->pfc.bgw_fetch_data[0] * 16) + 
                                                            get_ppu_context()->pfc.tile_y + 1);

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
        if (fifo_add())
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
        fifo_fetch();
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