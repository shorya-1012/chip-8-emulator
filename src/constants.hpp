#pragma once
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <cstdint>
#include <unordered_map>

const uint8_t FONT_SET[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10,
    0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10,
    0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0,
    0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0,
    0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80,
    0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80,
};

constexpr uint16_t MEMEORY_SIZE = 4096;
constexpr uint8_t STACK_SIZE = 16;
constexpr uint8_t REGISTERS = 16;
constexpr uint8_t KEYS = 16;
constexpr uint8_t FRAMERBUFFER_ROWS = 32;
constexpr uint8_t FRAMERBUFFER_COLS = 64;
constexpr uint16_t PROGRAM_COUNTER_OFFSET = 0x200;

static std::unordered_map<uint32_t, uint8_t> KEYS_MAP = {
    // Number keys
    {SDLK_0, 0},
    {SDLK_1, 1},
    {SDLK_2, 2},
    {SDLK_3, 3},
    {SDLK_4, 4},
    {SDLK_5, 5},
    {SDLK_6, 6},
    {SDLK_7, 7},
    {SDLK_8, 8},
    {SDLK_9, 9},
    // Alphabet keys
    {SDLK_A, 10},
    {SDLK_B, 11},
    {SDLK_C, 12},
    {SDLK_D, 13},
    {SDLK_E, 14},
    {SDLK_F, 15},
};
