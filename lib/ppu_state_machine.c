#include <string.h>
#include <cpu.h>
#include <ppu.h>
#include <lcd.h>
#include <interrupt.h>
// https://gbdev.io/pandocs/pixel_fifo.html

void fifo_reset();
void fifo_process();
bool window_visible();

void increment_ly()
{
    if (window_visible() && get_lcd_context()->ly >= get_lcd_context()->WY &&
        get_lcd_context()->ly < get_lcd_context()->WY + Y_RES)
    {
        get_ppu_context()->window_line++;
    }

    get_lcd_context()->ly++;

    if (get_lcd_context()->ly == get_lcd_context()->ly_compare)
    {
        LCDS_LYC_SET(1);

        if (LCDS_STAT_INT(STAT_LYC))
        {
            request_interrupt(IF_LCD_STAT);
        }
    }
    else
    {
        LCDS_LYC_SET(0);
    }
}

void load_line_sprites()
{
    int current_y = get_lcd_context()->ly;

    u8 sprite_size = LCDC_OBJ_SIZE;
    memset(get_ppu_context()->line_entry_array, 0,
           sizeof(get_ppu_context()->line_entry_array));

    for (int i = 0; i < 40; i++)
    {
        oam_context ent = get_ppu_context()->oam_ram[i];

        if (!ent.x)
        {
            continue;
        }

        if (get_ppu_context()->line_sprite_count >= 10)
        {
            break;
        }

        if (ent.y <= current_y + 16 && ent.y + sprite_size > current_y + 16)
        {
            oam_line_node *entry = &get_ppu_context()->line_entry_array[get_ppu_context()->line_sprite_count++];

            entry->entry = ent;
            entry->next = NULL;

            if (!get_ppu_context()->line_sprites || get_ppu_context()->line_sprites->entry.x > ent.x)
            {
                entry->next = get_ppu_context()->line_sprites;
                get_ppu_context()->line_sprites = entry;
                continue;
            }

            // Sorting
            oam_line_node *le = get_ppu_context()->line_sprites;
            oam_line_node *prev = le;

            while (le)
            {
                if (le->entry.x > ent.x)
                {
                    prev->next = entry;
                    entry->next = le;
                    break;
                }

                if (!le->next)
                {
                    le->next = entry;
                    break;
                }

                prev = le;
                le = le->next;
            }
        }
    }
}

static u32 target_frame_time = 1000 / 60;
static long prev_frame_time = 0;
static long start_timer = 0;
static long frame_count = 0;

void state_mode_hblank()
{
    if (get_ppu_context()->line_ticks >= DOTS_PER_LINE)
    {
        increment_ly();

        if (get_lcd_context()->ly >= Y_RES)
        {
            LCDS_MODE_SET(MODE_VBLANK);
            request_interrupt(IF_VBlank);

            if (LCDS_STAT_INT(STAT_VBLANK))
            {
                request_interrupt(IF_LCD_STAT);
            }
            get_ppu_context()->current_frame++;

            u32 end_timer = get_ticks();
            u32 frame_time = end_timer - prev_frame_time;

            if (frame_time < target_frame_time)
            {
                delay((target_frame_time - frame_time));
            }

            if (end_timer - start_timer >= 1000)
            {
                u32 fps = frame_count;
                start_timer = end_timer;
                frame_count = 0;

                printf("FPS: %d\n", fps);
            }
            frame_count++;
            prev_frame_time = get_ticks();
        }
        else
        {
            LCDS_MODE_SET(MODE_OAM);
        }
        get_ppu_context()->line_ticks = 0;
    }
}

void state_mode_vblank()
{
    if (get_ppu_context()->line_ticks >= DOTS_PER_LINE)
    {
        increment_ly();
        if (get_lcd_context()->ly >= LINES_PER_FRAME)
        {
            LCDS_MODE_SET(MODE_OAM);
            get_lcd_context()->ly = 0;
            get_ppu_context()->window_line = 0;
        }
        get_ppu_context()->line_ticks = 0;
    }
}

void state_mode_oam()
{
    if (get_ppu_context()->line_ticks >= 80)
    {
        LCDS_MODE_SET(MODE_DRAW);

        get_ppu_context()->pfc.cur_fetch_state = FS_TILE;
        get_ppu_context()->pfc.line_x = 0;
        get_ppu_context()->pfc.fetch_x = 0;
        get_ppu_context()->pfc.push_x = 0;
        get_ppu_context()->pfc.fifo_x = 0;
    }

    if (get_ppu_context()->line_ticks == 1)
    {
        get_ppu_context()->line_sprites = 0;
        get_ppu_context()->line_sprite_count = 0;

        load_line_sprites();
    }
}

void state_mode_draw()
{
    fifo_process();
    if (get_ppu_context()->pfc.push_x >= X_RES)
    {
        fifo_reset();
        LCDS_MODE_SET(MODE_HBLANK);

        if (LCDS_STAT_INT(STAT_HBLANK))
        {
            request_interrupt(IF_LCD_STAT);
        }
    }
}