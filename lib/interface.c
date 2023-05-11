#include <gboy.h>
#include <interface.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <bus.h>
#include <ppu.h>
// https://gbdev.io/pandocs/Rendering.html

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *sdlSurface;

SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *sdlDebugSurface;

static int scale = 4;

void interface_init()
{
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");
    TTF_Init();
    printf("TTF INIT\n");

    // Main Window
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlWindow, &sdlRenderer);

    sdlSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                      0x00FF0000,
                                      0x0000FF00,
                                      0x000000FF,
                                      0xFF000000);

    sdlTexture = SDL_CreateTexture(sdlRenderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   SCREEN_WIDTH, SCREEN_HEIGHT);

    // Debug Window
    SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0,
                                &sdlDebugWindow, &sdlDebugRenderer);

    sdlDebugSurface = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
                                           (32 * 8 * scale) + (64 * scale), 32,
                                           0x00FF0000,
                                           0x0000FF00,
                                           0x000000FF,
                                           0xFF000000);

    sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                        (16 * 8 * scale) + (16 * scale),
                                        (32 * 8 * scale) + (64 * scale));

    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    SDL_SetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);
}

void delay(u32 ms)
{
    SDL_Delay(ms);
}

u32 get_ticks()
{
    return SDL_GetTicks();
}

static unsigned long colour_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void displayTile(SDL_Surface *surface, u16 start_loc, u16 tileNum, int x, int y)
{
    SDL_Rect rec;

    for (int tile_y = 0; tile_y < 16; tile_y += 2)
    {
        u8 byte1 = bus_read(start_loc + (tileNum * 16) + tile_y);
        u8 byte2 = bus_read(start_loc + (tileNum * 16) + tile_y + 1);

        for (int bit = 7; bit >= 0; bit--)
        {
            u8 high = !!(byte1 & (1 << bit)) << 1;
            u8 low = !!(byte2 & (1 << bit));

            u8 colour = high | low;

            rec.x = x + ((7 - bit) * scale);
            rec.y = y + (tile_y / 2 * scale);
            rec.w = scale;
            rec.h = scale;

            SDL_FillRect(surface, &rec, colour_palette[colour]);
        }
    }
}

void updateDebugWindow()
{
    int x_draw = 0;
    int y_draw = 0;
    int tile_num = 0;

    SDL_Rect rec;
    rec.x = 0;
    rec.y = 0;
    rec.w = sdlDebugSurface->w;
    rec.h = sdlDebugSurface->h;
    SDL_FillRect(sdlDebugSurface, &rec, 0xFF111111);

    u16 address = 0x8000;

    // 384 tiles, 24x16
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            displayTile(sdlDebugSurface, address, tile_num, x_draw + (x * scale), y_draw + (y * scale));
            x_draw += (8 * scale);
            tile_num++;
        }
        y_draw += (8 * scale);
        x_draw = 0;
    }
    SDL_UpdateTexture(sdlDebugTexture, NULL, sdlDebugSurface->pixels, sdlDebugSurface->pitch);
    SDL_RenderClear(sdlDebugRenderer);
    SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
    SDL_RenderPresent(sdlDebugRenderer);
}

void interface_update()
{
    // Initiaization
    SDL_Rect rec;
    rec.x = rec.y = 0;
    rec.w = rec.h = 2048;

    u32 *video_buffer = get_ppu_context()->video_buffer;

    for (int line_num = 0; line_num < Y_RES; line_num++)
    {
        for (int x = 0; x < X_RES; x++)
        {
            rec.x = x * scale;
            rec.y = line_num * scale;
            rec.w = scale;
            rec.h = scale;

            SDL_FillRect(sdlSurface, &rec, video_buffer[x + (line_num * X_RES)]);
        }
    }

    SDL_UpdateTexture(sdlTexture, NULL, sdlSurface->pixels, sdlSurface->pitch);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);

    updateDebugWindow();
}

static unsigned long colour_palette_1[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
static unsigned long colour_palette_2[4] = {0xFFFFF6D3, 0xFFF9A875, 0xFFeB6B6F, 0xFF7C3F58};
static unsigned long colour_palette_3[4] = {0xFFD0D058, 0xFFA0A840, 0xFF708028, 0xFF405010};
static unsigned long colour_palette_4[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void change_palette(unsigned long *palette, int size)
{
    // SDL_Surface *surface
    sdlDebugSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                           0x00FF0000,
                                           0x0000FF00,
                                           0x000000FF,
                                           0xFF000000);
    SDL_LockSurface(sdlDebugSurface);
    memcpy(sdlDebugSurface->pixels, get_ppu_context()->video_buffer, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    SDL_UnlockSurface(sdlDebugSurface);

    SDL_PixelFormat *format = sdlDebugSurface->format;
    u32 *pixels = (u32 *)sdlDebugSurface->pixels;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], format, &r, &g, &b);
        pixels[i] = SDL_MapRGB(format, (palette[r >> 6] >> 16) & 0xFF, (palette[g >> 6] >> 8) & 0xFF, (palette[b >> 6]) & 0xFF);
    }

    SDL_UpdateTexture(sdlDebugTexture, NULL, sdlDebugSurface->pixels, SCREEN_WIDTH * 4);
    SDL_RenderClear(sdlDebugRenderer);
    SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
    SDL_RenderPresent(sdlDebugRenderer);
}

void interface_handle_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0)
    {
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            gboy_get_context()->close = true;

            switch (e.type)
            {
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_1:
                    change_palette(colour_palette_1, 4);
                    break;
                case SDLK_2:
                    change_palette(colour_palette_2, 4);
                    break;
                case SDLK_3:
                    change_palette(colour_palette_3, 4);
                    break;
                case SDLK_4:
                    change_palette(colour_palette_4, 4);
                    break;
                }
                break;
            }
        }
    }
}
