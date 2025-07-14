#include "chip8.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <random>
#include <strings.h>
#include <utility>

// setup random generator
std::random_device random_dev;
std::mt19937 generator(random_dev());
std::uniform_int_distribution<> distribution(0, 255);

Chip8::Chip8() {
  program_counter = PROGRAM_COUNTER_OFFSET;
  stack_ptr = 0;
  delay_timer = 0;
  sound_timer = 0;
  opcode = 0;
  index = 0;

  for (uint16_t i = 0; i < MEMEORY_SIZE; i++) {
    memory[i] = 0;
  }

  // Load font set to memory
  for (uint8_t i = 0; i < 80; i++) {
    memory[i] = FONT_SET[i];
  }

  for (uint8_t i = 0; i < REGISTERS; i++) {
    registers[i] = 0;
  }

  for (uint8_t y = 0; y < 32; y++) {
    for (uint8_t x = 0; x < 64; x++) {
      framebuffer[y][x] = 0;
    }
  }

  for (uint8_t i = 0; i < STACK_SIZE; i++) {
    stack[i] = 0;
  }

  for (uint8_t i = 0; i < KEYS; i++) {
    keys[i] = 0;
  }
}

inline void Chip8::advance_program_counter() { program_counter += 2; }

void Chip8::set_key_status(uint8_t key, bool is_pressed) {
  keys[key] = is_pressed;
}

void Chip8::debug_instruction() { SDL_Log("%X", opcode); }

inline std::pair<uint8_t, uint8_t> get_registers(uint16_t arguments) {
  uint8_t x = arguments >> 8;
  uint8_t y = (arguments & 0x00FF) >> 4;
  return {x, y};
}

void Chip8::cycle() {
  opcode = memory[program_counter] << 8 | memory[program_counter + 1];

  uint8_t first_nibble = opcode >> 12;
  uint16_t arguments = opcode & 0x0FFF;

  switch (first_nibble) {
  case 0x0:
    switch (opcode) {
    case 0x00E0:
      for (uint8_t y = 0; y < FRAMERBUFFER_ROWS; y++) {
        for (uint8_t x = 0; x < FRAMERBUFFER_COLS; x++) {
          framebuffer[y][x] = 0;
        }
      }
      break;
    case 0x00EE:
      program_counter = stack[--stack_ptr];
      break;
    }
    break;
  case 0x1:
    program_counter = arguments;
    break;
  case 0x2:
    stack[stack_ptr++] = program_counter;
    program_counter = arguments;
    break;
  case 0x3: {
    uint16_t kk = arguments & 0x00FF;
    uint8_t x = arguments >> 8;
    if (registers[x] == kk) {
      advance_program_counter();
    }
    break;
  }
  case 0x4: {
    uint16_t kk = arguments & 0x00FF;
    uint8_t x = arguments >> 8;
    if (registers[x] != kk) {
      advance_program_counter();
    }
    break;
  }
  case 0x5: {
    auto [x, y] = get_registers(arguments);
    if (registers[x] == registers[y]) {
      advance_program_counter();
    }
    break;
  }
  case 0x6: {
    uint16_t kk = arguments & 0x00FF;
    uint8_t x = arguments >> 8;
    registers[x] = kk;
    break;
  }
  case 0x7: {
    uint16_t kk = arguments & 0x00FF;
    uint8_t x = arguments >> 8;
    registers[x] += kk;
    break;
  }
  case 0x8: {
    auto [x, y] = get_registers(arguments);
    switch (opcode & 0x000F) {
    case 0x0: {
      registers[x] = registers[y];
      break;
    }
    case 0x1: {
      registers[x] = registers[x] | registers[y];
      break;
    }
    case 0x2: {
      registers[x] = registers[x] & registers[y];
      break;
    }
    case 0x3: {
      registers[x] = registers[x] ^ registers[y];
      break;
    }
    case 0x4: {
      uint16_t res = registers[x] + registers[y];
      registers[15] = res > 0xFF ? 1 : 0;
      registers[x] = res & 0xFF;
      break;
    }
    case 0x5: {
      registers[15] = registers[x] > registers[y] ? 1 : 0;
      registers[x] = registers[x] - registers[y];
      break;
    }
    case 0x6: {
      registers[15] = registers[x] & 1;
      registers[x] = registers[x] >> 1;
      break;
    }
    case 0x7: {
      registers[15] = registers[y] > registers[x] ? 1 : 0;
      registers[x] = registers[y] - registers[x];
      break;
    }
    case 0xE: {
      registers[15] = (registers[x] >> 7) & 1;
      registers[x] = registers[x] << 1;
      break;
    }
    }
    break;
  }
  case 0x9: {
    auto [x, y] = get_registers(arguments);
    if (registers[x] != registers[y]) {
      advance_program_counter();
    }
    break;
  }
  case 0xA: {
    index = arguments;
    break;
  }
  case 0xB: {
    program_counter = arguments + registers[0];
    break;
  }
  case 0xC: {
    uint8_t x = arguments >> 8;
    uint16_t kk = arguments & 0x00FF;
    registers[x] = (distribution(generator) & kk);
    break;
  }
  case 0xD: {
    auto [vx, vy] = get_registers(arguments);
    uint8_t x_register = registers[vx];
    uint8_t y_register = registers[vy];
    uint8_t n = opcode & 0x000F;
    for (uint8_t y = 0; y < n; y++) {
      uint8_t sprite_byte = memory[index + y];
      for (uint8_t x = 0; x < 8; x++) {
        // start drawing from most sig. bit
        uint8_t sprite_pixel = (sprite_byte >> (7 - x)) & 1;

        uint8_t screen_x = (x_register + x) % FRAMERBUFFER_COLS;
        uint8_t screen_y = (y_register + y) % FRAMERBUFFER_ROWS;

        if (sprite_pixel) {
          framebuffer[screen_y][screen_x] ^= 1;
          if (framebuffer[screen_y][screen_x] == 0) {
            registers[15] = 1;
          }
        }
      }
    }
    break;
  }
  case 0xE: {
    uint8_t x = arguments >> 8;
    switch (opcode & 0x00FF) {
    case 0x9E: {
      if (keys[registers[x]] == true) {
        advance_program_counter();
      }
      break;
    }
    case 0xA1: {
      if (keys[registers[x]] == false) {
        advance_program_counter();
      }
      break;
    }
    }
    break;
  }
  case 0xF: {
    uint8_t x = arguments >> 8;
    switch (opcode & 0x00FF) {
    case 0x07: {
      registers[x] = delay_timer;
      break;
    }
    case 0x0A: {
      bool key_pressed = false;
      for (uint8_t i = 0; i < KEYS; i++) {
        if (keys[i]) {
          key_pressed = true;
          registers[x] = i;
          break;
        }
      }
      if (!key_pressed)
        return;
      break;
    }
    case 0x15: {
      delay_timer = registers[x];
      break;
    }
    case 0x18: {
      sound_timer = registers[x];
      break;
    }
    case 0x1E: {
      index = index + registers[x];
      break;
    }
    case 0x29: {
      index = registers[x] * 5;
      break;
    }
    case 0x33: {
      memory[index] = registers[x] / 100;
      memory[index + 1] = (registers[x] / 10) % 10;
      memory[index + 2] = registers[x] % 10;
      break;
    }
    case 0x55: {
      uint8_t i = index;
      for (uint8_t curr_register = 0; curr_register <= x; curr_register++) {
        memory[i++] = registers[curr_register];
      }
      break;
    }
    case 0x65: {
      uint8_t curr_register = 0;
      for (uint16_t i = 0; i <= x; i++) {
        registers[curr_register++] = memory[index + i];
      }
      break;
    }
    }
  }
  }
  advance_program_counter();
  if (delay_timer > 0)
    delay_timer--;
  if (sound_timer > 0)
    // Todo : Play some sound
    sound_timer--;
}

void Chip8::load_rom(const char *filename) {
  std::ifstream infile(filename, std::ios::binary | std::ios::ate);
  if (!infile) {
    SDL_Log("%s", "Unable to load rom\n");
    exit(EXIT_FAILURE);
  }

  uint32_t size = infile.tellg();
  infile.seekg(0, std::ios::beg);

  if (PROGRAM_COUNTER_OFFSET + size > MEMEORY_SIZE) {
    SDL_Log("%s", "Rom Too Large\n");
    exit(EXIT_FAILURE);
  }

  infile.read(reinterpret_cast<char *>(&memory[PROGRAM_COUNTER_OFFSET]), size);
}
