#include <stdio.h>
#include <gboy.h>
#include <cpu.h>
#include <cartridge.h>
#include <interface.h>
#include <pthread.h>
#include <unistd.h>
#include <timer.h>

static gboy_context ctx;
gboy_context *gboy_get_context()
{
    return &ctx;
}

void *cpu_running(void *p)
{
    timer_init();
    cpu_init();

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
        //ctx.ticks++;
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
    while (!ctx.close)
    {
        usleep(1000);
        interface_handle_events();
    }
    return 0;
}

void gboy_cycles(int cpu_cycles)
{
    int n = cpu_cycles * 4;

    for(int i=0; i<n; i++)
    {
        ctx.ticks++;
        timer_tick();
    }
}
