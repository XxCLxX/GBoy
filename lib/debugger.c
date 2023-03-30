#include <debugger.h>
#include <bus.h>

static char result[1024] = {0};
static int result_size = 0;

void debugger_update()
{
    if(bus_read(0xFF02) == 0x81)
    {
        char c = bus_read(0xFF01);
        result[result_size++] = c;
        bus_write(0xFF02, 0);
    }
}

void debugger_print()
{
    if(result[0])
    {
        //printf("DEBUG: %s\n", result);
    }
}