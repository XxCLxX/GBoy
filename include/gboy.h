#pragma once
#include <common.h>

typedef struct 
{
    bool paused;
    bool running;
    u64 ticks;
} 
gboy_context;

int gboy_run(int argc, char **argv);

gboy_context *gboy_get_context();
