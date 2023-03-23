#include <stdio.h>
#include <gboy.h>
#include <cpu.h>
#include <cartridge.h>
#include <interface.h>
#include <pthread.h>
#include <unistd.h>
#include <timer.h>
#include <dma.h>
#include <ppu.h>

static gboy_context ctx;
gboy_context *gboy_get_context()
{
    return &ctx;
}

void *cpu_running(void *p)
{
    timer_init();
    cpu_init();
    ppu_init();

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    while (ctx.running)
    {

        if (ctx.paused)
        {
            delay(10);
            continue;
        }

        if (!cpu_run())
        {
            printf("CPU Stopped...\n");
            return 0;
        }
        // ctx.ticks++;
    }
    return 0;
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

    interface_init();

    pthread_t t1;

    if (pthread_create(&t1, NULL, cpu_running, NULL))
    {
        fprintf(stderr, "CPU Thread: Failed\n");
        return -1;
    }

    u32 prev_frame = 0;

    while (!ctx.close)
    {
        usleep(1000);
        interface_handle_events();

        if (prev_frame != get_ppu_context()->current_frame)
        {
            interface_update();
        }
        prev_frame = get_ppu_context()->current_frame;
    }
    return 0;
}

void gboy_cycles(int cpu_cycles)
{
    for (int i = 0; i < cpu_cycles; i++)
    {
        for (int n = 0; n < 4; n++)
        {
            ctx.ticks++;
            timer_tick();
            ppu_run();
        }
        dma_tick();
    }
}
