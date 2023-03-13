#include <gboy.h>
#include <interface.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <bus.h>
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

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlWindow, &sdlRenderer);

    SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0, &sdlDebugWindow, &sdlDebugRenderer);

    sdlDebugSurface = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
                                           (32 * 8 * scale) + (64 * scale), 32,
                                           0x00FF0000,
                                           0x0000FF00,
                                           0x000000FF,
                                           0xFF000000);

    sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
                                        (16 * 8 * scale) + (16 * scale),
                                        (32 * 8 * scale) + (64 * scale));

    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    SDL_GetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);
}

void delay(u32 ms)
{
    SDL_Delay(ms);
}

static unsigned long colour_palette[4] = {0xFFFFFFFF, 0xFFAAAAAAA, 0xFF555555, 0xFF000000};

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
            rec.y = y + ((tile_y / 2) * scale);
            rec.w = scale;
            rec.h = scale;

            SDL_FillRect(surface, &rec, colour_palette[colour]);
        }
    }
}

updateDebugWindow()
{
    int x_draw = 0;
    int y_draw = 0;
    int title_num = 0;

    SDL_Rect rec;
    rec.x = 0;
    rec.y = 0;
    rec.w = sdlDebugSurface->w;
    rec.h = sdlDebugSurface->h;
    SDL_FillRect(sdlDebugWindow, &rec, 0xFF111111);

    u16 address = 0x8000;

    // 384 tiles, 24x16
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            displayTile(sdlDebugSurface, address, title_num, x_draw + (x * scale), y_draw + (y * scale));
            x_draw += (8 * scale);
            title_num++;
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
    updateDebugWindow();
}

void interface_handle_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0)
    {

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            gboy_get_context()->close = true;
        }
    }
}