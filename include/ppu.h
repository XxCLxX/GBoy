#pragma once
#include <common.h>
// https://gbdev.io/pandocs/OAM.html

static const int LINES_PER_FRAME = 154;
static const int DOTS_PER_LINE = 456;
static const int Y_RES = 144;
static const int X_RES = 160;

// Pixel FIFO -State Machine
typedef enum
{
    FS_TILE,
    FS_DATA_LOW,
    FS_DATA_HIGH,
    FS_SLEEP,
    FS_PUSH
} fetch_state;

typedef struct _fifo_node
{
    struct _fifo_node *next;
    u32 value;
} fifo_node;

typedef struct
{
    fifo_node *front;
    fifo_node *back;
    u32 size;
} fifo;

typedef struct
{
    fetch_state cur_fetch_state;
    fifo pixel_fifo;
    u8 line_x;
    u8 push_x;
    u8 fetch_x;
    u8 bgw_fetch_data[3];
    u8 fetch_oam_data[6];
    u8 map_y;
    u8 map_x;
    u8 tile_y;
    u8 fifo_x;
} pixel_fifo_context;

typedef struct
{
    u8 y;
    u8 x;
    u8 tile;

    // Attributes-Flags
    u8 flag_cgb_palette_num : 3; // CGB Mode Only
    u8 flag_cgb_vram : 1;        // CGB Model Only
    u8 flag_x_flip : 1;
    u8 flag_y_flip : 1;
    u8 flag_palette_num : 1;
    u8 flag_bg : 1;
} oam_context;

typedef struct _oam_line_node
{
    oam_context entry;
    struct _oam_line_node *next;
} oam_line_node;

typedef struct
{
    oam_context oam_ram[40];
    u8 vram[0x2000];

    u8 line_sprite_count;
    oam_line_node *line_sprites;
    oam_line_node line_entry_array[10];

    u8 fetched_entry_count;
    oam_context fetched_entries[3];
    u8 window_line;

    u32 current_frame;
    u32 line_ticks;
    u32 *video_buffer;

    pixel_fifo_context pfc;
} ppu_context;

void ppu_init();
void ppu_run();

void oam_write(u16 address, u8 value);
u8 oam_read(u16 address);

void vram_write(u16 address, u8 value);
u8 vram_read(u16 address);

ppu_context *get_ppu_context();