#include <bus.h>
#include <ram.h>
#include <cartridge.h>
#include <cpu.h>
#include <io.h>

// Reference the Memory Map
u8 bus_read(u16 address)
{
    if (address < 0x8000) //ROM Banks
    {
        return rom_read(address);
    }
    else if(address < 0xA000) //VRAM
    {
        printf("Unsupported memory address, bus_read(%04X)\n", address);
        NO_IMPLEM
    }
    else if(address < 0xC000) //External RAM
    {
        return rom_read(address);
    }
    else if(address < 0xE000) //WRAM
    {
        return wram_read(address);
    }
    else if(address < 0xFE00) //ECHO RAM
    {
        return 0;
    }
    else if(address < 0xFEA0) //OAM
    {
        printf("Unsupported memory address, bus_read(%04X)\n", address);
        //NO_IMPLEM
        return 0x0;
    } 
    else if(address < 0xFF00) //Not Usable
    {
        //printf("Unsupported memory address, bus_read(%04X)\n", address);
        return 0;
    }
    else if(address < 0xFF80) //IO Registers
    {
        return io_read(address);
    }
    else if(address == 0xFFFF) //Interrupts Enable Register
    {
        return ie_register_get();
    }

    return hram_read(address);
    //NO_IMPLEM
}

void bus_write(u16 address, u8 value)
{
    if (address < 0x8000) //ROM Banks
    {
        rom_write(address, value);
    }
    else if(address < 0xA000)
    {
        printf("Unsupported memory address, bus_write(%04X)\n", address);
        //NO_IMPLEM
    }
    else if(address < 0xC000) //External RAM
    {
        rom_write(address, value);
    }
    else if(address < 0xE000) //WRAM
    {
        wram_write(address, value);
    }
    else if(address < 0xFE00) //ECHO RAM
    {

    }
    else if(address < 0xFEA0) //OAM
    {
        printf("Unsupported memory address, bus_write(%04X)\n", address);
        //NO_IMPLEM
    }
    else if(address < 0xFF00) //Not Usable
    {

    }
    else if(address < 0xFF80) //IO Registers
    {
        io_write(address, value);
    }
    else if(address == 0xFFFF) //Interrupts Enable Register
    {
        ie_register_set(value);
    }
    else
    {
        hram_write(address, value);
    }
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