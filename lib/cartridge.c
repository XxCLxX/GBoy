#include <string.h>
#include <cartridge.h>

typedef struct
{
    char filename[1024];
    u32 rom_size;
    u8 *rom_data;
    cartridge_header *header;

    bool ram_enabled;
    bool ram_banking;

    u8 *rom_bank_x;
    u8 banking_mode;

    u8 rom_bank_value;
    u8 ram_bank_value;

    u8 *ram_bank;
    u8 *all_ram_banks[16];

    bool battery;
    bool battery_saved;
} cart_context;

static cart_context ctx;

bool cartridge_need_save()
{
    return ctx.battery_saved;
}

bool cartridge_mbc1()
{
    return BETWEEN(ctx.header->cart_type, 1, 3);
}

bool cartridge_battery()
{
    return ctx.header->cart_type == 3;
}

// Reference from Pan Docs
static const char *LIC_CODE[0x100] =
    {
        [0x00] = "None", // New licensee code
        [0x01] = "Nintendo R&D1",
        [0x08] = "Capcom",
        [0x13] = "Electronic Arts",
        [0x18] = "Hudson Soft",
        [0x19] = "b-ai",
        [0x20] = "kss",
        [0x22] = "pow",
        [0x24] = "PCM Complete",
        [0x25] = "san-x",
        [0x28] = "Kemco Japan",
        [0x29] = "seta",
        [0x30] = "Viacom",
        [0x31] = "Nintendo",
        [0x32] = "Bandai",
        [0x33] = "Ocean/Acclaim",
        [0x34] = "Konami",
        [0x35] = "Hector",
        [0x37] = "Taito",
        [0x38] = "Hudson",
        [0x39] = "Banpresto",
        [0x41] = "Ubi Soft",
        [0x42] = "Atlus",
        [0x44] = "Malibu",
        [0x46] = "angel",
        [0x47] = "Bullet-Proof",
        [0x49] = "irem",
        [0x50] = "Absolute",
        [0x51] = "Acclaim",
        [0x52] = "Activision",
        [0x53] = "American Sammy",
        [0x54] = "Konami",
        [0x55] = "Hi Tech Entertainment",
        [0x56] = "LJN",
        [0x57] = "Matchbox",
        [0x58] = "Mattel",
        [0x59] = "Milton Bradley",
        [0x60] = "Titus",
        [0x61] = "Virgin",
        [0x64] = "LucasArts",
        [0x67] = "Ocean",
        [0x69] = "Electronic Arts",
        [0x70] = "Infogrames",
        [0x71] = "Interplay",
        [0x72] = "Broderbund",
        [0x73] = "sculptured",
        [0x75] = "sci",
        [0x78] = "THQ",
        [0x79] = "Accolade",
        [0x80] = "misawa",
        [0x83] = "lozc",
        [0x86] = "Tokuma Shoten Intermedia",
        [0x87] = "Tsukuda Original",
        [0x91] = "Chunsoft",
        [0x92] = "Video system",
        [0x93] = "Ocean/Acclaim",
        [0x95] = "Varie",
        [0x96] = "Yonezawa/s'pal",
        [0x97] = "Kaneko",
        [0x99] = "Pack-In-Soft",
        [0xA4] = "Konami (Yu-Gi-Oh!)",
        [0xA6] = "Kawada", // Old licensee
        [0xA7] = "Takara",
        [0xA9] = "Technos Japan",
        [0xAA] = "Broderbund",
        [0xAC] = "Toei Animation",
        [0xAD] = "Toho",
        [0xAF] = "Namco",
        [0xB0] = "acclaim",
        [0xB1] = "ASCII or Nexsoft",
        [0xB2] = "Bandai",
        [0xB4] = "Square Enix",
        [0xB6] = "HAL Laboratory",
        [0xB7] = "SNK",
        [0xB9] = "Pony Canyon",
        [0xBA] = "Culture Brain",
        [0xBB] = "Sunsoft",
        [0xBD] = "Sony Imagesoft",
        [0xBF] = "Sammy",
        [0xC0] = "Taito",
        [0xC2] = "Kemco",
        [0xC3] = "SquareSoft",
        [0xC4] = "Tokuma Shoten Intermedia",
        [0xC5] = "Data East",
        [0xC6] = "Tonkinhouse",
        [0xC8] = "Koei",
        [0xC9] = "UFL",
        [0xCA] = "Ultra",
        [0xCB] = "Vap",
        [0xCC] = "Use Corporation",
        [0xCD] = "Meldac",
        [0xCE] = "Pony Canyon",
        [0xCF] = "Angel",
        [0xD0] = "Taito",
        [0xD1] = "Sofel",
        [0xD2] = "Quest",
        [0xD3] = "Sigma Enterprises",
        [0xD4] = "ASK Kodansha Co.",
        [0xD6] = "Naxat Soft",
        [0xD7] = "Copya System",
        [0xD9] = "Banpresto",
        [0xDA] = "Tomy",
        [0xDB] = "LJN",
        [0xDD] = "NCS",
        [0xDE] = "Human",
        [0xDF] = "Altron",
        [0xE0] = "Jaleco",
        [0xE1] = "Towa Chiki",
        [0xE2] = "Yutaka",
        [0xE3] = "Varie",
        [0xE5] = "Epcoh",
        [0xE7] = "Athena",
        [0xE8] = "Asmik ACE Entertainmnent",
        [0xE9] = "Natsume",
        [0xEA] = "King Records",
        [0xEB] = "Atlus",
        [0xEC] = "Epic/Sony Records",
        [0xEE] = "IGS",
        [0xF0] = "A Wave",
        [0xF3] = "Extreme Entertainment",
        [0xFF] = "LGN"};

static const char *CART_TYPES[] =
    {
        "ROM ONLY",
        "MBC1",
        "MBC+RAM",
        "MBC1+RAM+BATTERY",
        "$04",
        "MBC2",
        "MBC2+BATTERY",
        "$07",
        "ROM+RAM",
        "ROM+RAM+BATTERY",
        "$0A",
        "MMM01",
        "MMM01+RAM",
        "MMM01+RAM+BATTERY",
        "$0E",
        "MBC3+TIMER+BATTERY",
        "MBC3+TIMER+RAM+BATTERY",
        "MBC3",
        "MBC3+RAM",
        "MBC3+RAM+BATTERY",
        "$014",
        "$015",
        "$016",
        "$017",
        "$018",
        "MBC5",
        "MBC5+RAM",
        "MBC5+RAM+BATTERY",
        "MBC5+RUMBLE",
        "MBC5+RUMBLE+RAM",
        "$01F",
        "MBC6",
        "$021",
        "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
        "POCKET+CAMERA",
        "BANDAI TAMA5",
        "HuC3",
        "HuC1+RAM+BATTERY"};

const char *type_cartridge()
{
    if (ctx.header->cart_type <= 0x22)
    {
        return CART_TYPES[ctx.header->cart_type];
    }
    return "Unknown";
}

const char *lic_cartridge()
{
    if (ctx.header->new_lic_code <= 0xA4)
    {
        return LIC_CODE[ctx.header->old_lic_code];
    }
    return "Unknown";
}

void setup_banking_cartridge()
{
    for (int i = 0; i < 16; i++)
    {
        ctx.all_ram_banks[i] = 0;

        if ((ctx.header->ram_size == 2 && i == 0) ||
            (ctx.header->ram_size == 3 && i < 4) ||
            (ctx.header->ram_size == 4 && i < 16) ||
            (ctx.header->ram_size == 5 && i < 8))
        {
            ctx.all_ram_banks[i] = malloc(0x2000);
            memset(ctx.all_ram_banks[i], 0, 0x2000);
        }
    }
    ctx.ram_bank = ctx.all_ram_banks[0];
    ctx.rom_bank_x = ctx.rom_data + 0x4000;
}

bool load_cartridge(char *cart)
{
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart);

    FILE *f = fopen(cart, "rb");

    if (f == NULL)
    {
        printf("Failed to open the file: %s\n", cart);
        return false;
    }

    printf("Opened: %s\n", ctx.filename);

    fseek(f, 0, SEEK_END);
    ctx.rom_size = ftell(f);
    // fseek(f, 0, SEEK_SET);

    rewind(f);

    ctx.rom_data = malloc(ctx.rom_size);
    fread(ctx.rom_data, ctx.rom_size, 1, f);
    fclose(f);

    ctx.header = (cartridge_header *)(ctx.rom_data + 0x100);
    ctx.header->title[15] = 0;
    ctx.battery = cartridge_battery();
    ctx.battery_saved = false;

    printf("Cartridge Loaded:\n");
    printf("\t Title    : %s\n", ctx.header->title);
    printf("\t Type     : %2.2X (%s)\n", ctx.header->cart_type, type_cartridge());
    printf("\t ROM Size : %d KB\n", 32 << ctx.header->rom_size);
    printf("\t RAM Size : %2.2X\n", ctx.header->ram_size);
    printf("\t Publisher: %2.2X (%s)\n", ctx.header->old_lic_code, lic_cartridge());
    printf("\t Version  : %2.2X\n", ctx.header->rom_version);

    setup_banking_cartridge();

    /*
    Header Checksum
    uint8_t checksum = 0;
    for (uint16_t address = 0x0134; address <= 0x014C; address++)
    {
        checksum = chSSecksum - rom[addess] - 1;
    }
    */
    u16 check = 0;
    for (u16 address = 0x0134; address <= 0x014C; address++)
    {
        check = check - ctx.rom_data[address] - 1;
    }
    printf("\t Checksum: %2.2X (%s)\n", ctx.header->checksum, (check & 0xFF) ? "Passed" : "Failed");

    if (ctx.battery)
    {
        cartridge_battery_load();
    }

    return true;
}

void cartridge_battery_save()
{
    if(!ctx.ram_bank)
    {
        return;
    }

    char fn[1048];
    sprintf(fn, "%s.battery", ctx.filename);
    FILE *f = fopen(fn, "wb");

    if (f == NULL)
    {
        fprintf(stderr, "FAILED TO OPEN: %s\n", fn);
        return;
    }

    fwrite(ctx.ram_bank, 0x2000, 1, f);
    fclose(f);
}

void cartridge_battery_load()
{
    if(!ctx.ram_bank)
    {
        return;
    }
    
    char fn[1048];
    sprintf(fn, "%s.battery", ctx.filename);
    FILE *f = fopen(fn, "rb");

    if (f == NULL)
    {
        fprintf(stderr, "FAILED TO OPEN: %s\n", fn);
        return;
    }

    fread(ctx.ram_bank, 0x2000, 1, f);
    fclose(f);
}

u8 rom_read(u16 address)
{
    if (!cartridge_mbc1() || address < 0x4000)
    {
        return ctx.rom_data[address];
    }

    /*if (!cartridge_mbc1())
    {
        return 0xFF;
    }*/

    if ((address & 0xE000) == 0xA000)
    {
        if (!ctx.ram_enabled)
        {
            return 0xFF;
        }

        if (!ctx.ram_bank)
        {
            return 0xFF;
        }
        return ctx.ram_bank[address - 0xA000];
    }
    return ctx.rom_bank_x[address - 0x4000];
}

void rom_write(u16 address, u8 value)
{
    if (!cartridge_mbc1)
    {
        return;
    }

    if (address < 0x2000)
    {
        ctx.ram_enabled = ((value & 0xF) == 0xA);
    }

    if ((address & 0xE000) == 0x2000)
    {
        if (value == 0)
        {
            value = 1;
        }
        value &= 0b11111;

        ctx.rom_bank_value = value;
        ctx.rom_bank_x = ctx.rom_data + (0x4000 * ctx.rom_bank_value);
    }

    if ((address & 0xE000) == 0x4000)
    {
        ctx.ram_bank_value = value & 0b11;

        if (ctx.ram_banking)
        {
            if (cartridge_need_save())
            {
                cartridge_battery_save();
            }
            ctx.ram_bank = ctx.all_ram_banks[ctx.ram_bank_value];
        }
    }

    if ((address & 0xE000) == 0x6000)
    {
        ctx.banking_mode = value & 1;
        ctx.ram_banking = ctx.banking_mode;

        if (ctx.ram_banking)
        {
            if (cartridge_need_save())
            {
                cartridge_battery_save();
            }
            ctx.ram_bank = ctx.all_ram_banks[ctx.ram_bank_value];
        }
    }

    if ((address & 0xE000) == 0xA000)
    {
        if (!ctx.ram_enabled)
        {
            return;
        }

        if (!ctx.ram_bank)
        {
            return;
        }
        ctx.ram_bank[address - 0xA000] = value;

        if (ctx.battery)
        {
            ctx.battery_saved = true;
        }
    }
}