#include <cpu.h>
#include <ppu.h>
#include <lcd.h>
#include <interrupt.h>
// https://gbdev.io/pandocs/pixel_fifo.html

void increment_ly()
{
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