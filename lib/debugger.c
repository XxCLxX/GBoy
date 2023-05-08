#include <debugger.h>
#include <bus.h>
#include <string.h>

#define DEBUG_SIZE 1024

static char buffer[DEBUG_SIZE] = {0};
static int read_index = 0;
static int write_index = 0;

void debugger_update()
{
    if(bus_read(0xFF02) == 0x81)
    {
        char c = bus_read(0xFF01);
        
        if((write_index + 1) % DEBUG_SIZE == read_index)
        {
            read_index = (read_index + 1) % DEBUG_SIZE;
        }
        
        buffer[write_index] = c;
        write_index = (write_index + 1) % DEBUG_SIZE;
        
        bus_write(0xFF02, 0);
    }
}

void debugger_print()
{
    // Check if buffer is empty
    if(read_index != write_index)
    {
        // Print data from buffer
        int i;
        printf("DEBUG: ");
        for(i = read_index; i != write_index; i = (i + 1) % DEBUG_SIZE)
        {
            printf("%c", buffer[i]);
        }
        printf("\n");
        
        //read_index = write_index;
        //memset(buffer, 0, DEBUG_SIZE);
    }
}

/*static char result[1024] = {0};
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
        printf("DEBUG: %s\n", result);
    }
}*/