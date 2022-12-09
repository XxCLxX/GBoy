#include <stdio.h>
#include <gboy.h>
#include <cpu.h>
#include <cartridge.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static gboy_context ctx;
gboy_context *gboy_get_context() 
{
    return &ctx;
}

void delay(u32 ms) 
{
    SDL_Delay(ms);
}

int gboy_run(int argc, char **argv) 
{
    if (argc < 2) 
    {
        printf("Usage: gboy <rom_file>\n");
        return -1;
    }

    if (!load_cartridge(argv[1])) 
    {
        printf("Failed to load ROM file: %s\n", argv[1]);
        return -2;
    }

    printf("Cart loaded...\n");

    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    TTF_Init();
    printf("TTF INIT\n");

    cpu_init();
    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    while(ctx.running) 
    {
        if (ctx.paused) 
        {
            delay(10);
            continue;
        }

        if (!cpu_run()) 
        {
            printf("CPU Stopped.\n");
            return -3;
        }
        ctx.ticks++;
    }

    return 0;
}
