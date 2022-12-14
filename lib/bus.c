#include <bus.h>

// Reference the Memory Map
u8 bus_read(u16 address)
{
    if (address < 0x8000)
    {
        return rom_read(address);
    }

    NO_IMPLEM
}

void bus_write(u16 address, u8 value)
{
    if (address < 0x8000)
    {
        rom_write(address, value);
        return;
    }
    NO_IMPLEM
}