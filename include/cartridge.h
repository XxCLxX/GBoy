#pragma once
#include <common.h>

//Reference from Pan Docs
typedef struct
{
    u8 entry[4];
    u8 logo[0x30];
    char title[16];
    u16 new_lic_code;
    u8 sgb_flag;
    u8 cart_type;
    u8 rom_size;
    u8 ram_size;
    u8 dest_code;
    u8 old_lic_code;
    u8 rom_version;
    u8 checksum;
    u16 global_checksum;
}
cartridge_header;

bool load_cartridge(char *cart);

u8 rom_read(u16 address);
void rom_write(u16 address, u8 value);


bool cartridge_need_save();
void cartridge_battery_save();
void cartridge_battery_load();