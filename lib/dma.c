#include <dma.h>
#include <bus.h>
#include <ppu.h>
// https://gbdev.io/pandocs/OAM_DMA_Transfer.html

typedef struct 
{
    bool active;
    u8 byte;
    u8 value;
    u8 delay;
} dma_context;

static dma_context ctx;

void dma_start_address(u8 start)
{
    ctx.active = true;
    ctx.byte = 0;
    ctx.delay = 2;
    ctx.value = start;
}

void dma_tick()
{
    if(!ctx.active)
    {
        return;
    }

    if(ctx.delay)
    {
        ctx.delay--;
        return;
    }
    oam_write(ctx.byte, bus_read((ctx.value * 0x100) + ctx.byte));
    ctx.byte++;
    ctx.active = ctx.byte < 0xA0;
}

bool dma_transfer()
{
    return ctx.active;
}