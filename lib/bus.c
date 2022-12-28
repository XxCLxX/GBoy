#include <bus.h>
#include <cartridge.h>

// Reference the Memory Map
u8 bus_read(u16 address)
{
    if (address < 0x8000)
    {
        return rom_read(address);
    }

    printf("Unsupported bus_read(%04X)\n", address);
    //NO_IMPLEM
}

void bus_write(u16 address, u8 value)
{
    if (address < 0x8000)
    {
        rom_write(address, value);
        return;
    }

    printf("Unsupported bus_write(%04X)\n", address);
    //NO_IMPLEM
}

u16 bus_read16(u16 address)
{
    u16 low = bus_read(address);
    u16 high = bus_read(address + 1);

    return low | (high << 8);
}

void bus_write16(u16 address, u16 value)
{
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}