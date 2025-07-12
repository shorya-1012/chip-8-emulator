#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <cstdlib>

constexpr int SCREEN_HEIGHT = 480;
constexpr int SCREEN_WIDTH = 960;

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

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
}

int main(int argc, char *argv[]) {
  init_graphics();
  bool is_running = true;
  SDL_Event event;

  Chip8 cpu;
  // Todo : Load rom

  while (is_running) {

    // Todo : CPU cycle
    // cpu.cycle();

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        is_running = false;
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
  return EXIT_SUCCESS;
}
