#include "chip8.hpp"
#include "constants.hpp"
#include <SDL3/SDL.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

constexpr int SCREEN_HEIGHT = 480;
constexpr int SCREEN_WIDTH = 960;

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture *texture = nullptr;

void init_graphics() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Unable to initialize SDL : \n %s\n", SDL_GetError());
    exit(SDL_APP_FAILURE);
  }

  if (!SDL_CreateWindowAndRenderer("chip8-emulator", SCREEN_WIDTH,
                                   SCREEN_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("Unable to initialize window  and renderer : \n %s\n",
            SDL_GetError());
    exit(SDL_APP_FAILURE);
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, FRAMERBUFFER_COLS,
                              FRAMERBUFFER_ROWS);

  if (!texture) {
    SDL_Log("Unable to create texture: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    SDL_Log("%s", "No rom provided\n");
    exit(EXIT_FAILURE);
  }

  const char *rom_file = argv[1];

  init_graphics();
  bool is_running = true;
  SDL_Event event;

  Chip8 cpu;
  cpu.load_rom(rom_file);

  while (is_running) {
    cpu.debug_instruction();
    cpu.cycle();

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        is_running = false;
        break;
      case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode key = event.key.key;
        if (KEYS_MAP.find(key) != KEYS_MAP.end()) {
          cpu.set_key_status(KEYS_MAP[key], true);
        }
        break;
      }
      case SDL_EVENT_KEY_UP: {
        SDL_Keycode key = event.key.key;
        if (KEYS_MAP.find(key) != KEYS_MAP.end()) {
          cpu.set_key_status(KEYS_MAP[key], false);
        }
        break;
      }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Update the texture
    void *pixels = nullptr;
    int pitch = 0;

    if (!SDL_LockTexture(texture, NULL, &pixels, &pitch)) {
      SDL_Log("Failed to lock texture: %s", SDL_GetError());
      return EXIT_FAILURE;
    }

    for (uint8_t y = 0; y < FRAMERBUFFER_ROWS; y++) {
      uint32_t *row = reinterpret_cast<uint32_t *>(
          static_cast<uint8_t *>(pixels) + y * pitch);

      for (uint8_t x = 0; x < FRAMERBUFFER_COLS; x++) {
        row[x] = cpu.framebuffer[y][x] ? 0xFFFFFFFF : 0xFF000000;
      }
    }

    SDL_UnlockTexture(texture);
    SDL_FRect dst_rect = {0, 0, static_cast<float>(SCREEN_WIDTH),
                          static_cast<float>(SCREEN_HEIGHT)};
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    SDL_RenderPresent(renderer);

    // Approximate 60Hz
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
