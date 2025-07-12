#include "chip8.hpp"
#include <cstdint>
#include <random>
#include <strings.h>
#include <utility>

// setup random generator
std::random_device random_dev;
std::mt19937 generator(random_dev());
std::uniform_int_distribution<> distribution(0, 255);

Chip8::Chip8() {
  program_counter = 0x200;
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

void Chip8::advance_program_counter() { program_counter += 2; }

std::pair<uint8_t, uint8_t> get_registers(uint16_t arguments) {
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
      // Todo : clear the screen
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
    switch (opcode & 0x000F) {
    case 0x0: {
      auto [x, y] = get_registers(arguments);
      registers[x] = registers[y];
      break;
    }
    case 0x1: {
      auto [x, y] = get_registers(arguments);
      registers[x] = registers[x] | registers[y];
      break;
    }
    case 0x2: {
      auto [x, y] = get_registers(arguments);
      registers[x] = registers[x] & registers[y];
      break;
    }
    case 0x3: {
      auto [x, y] = get_registers(arguments);
      registers[x] = registers[x] ^ registers[y];
      break;
    }
    case 0x4: {
      auto [x, y] = get_registers(arguments);
      uint16_t res = registers[x] + registers[y];
      registers[15] = res > 0xFF ? 1 : 0;
      registers[x] = res & 0xFF;
      break;
    }
    case 0x5: {
      auto [x, y] = get_registers(arguments);
      registers[15] = registers[x] > registers[y] ? 1 : 0;
      registers[x] = registers[x] - registers[y];
      break;
    }
    case 0x6: {
      auto [x, y] = get_registers(arguments);
      registers[15] = registers[x] & 1;
      registers[x] = registers[x] >> 1;
      break;
    }
    case 0x7: {
      auto [x, y] = get_registers(arguments);
      registers[15] = registers[y] > registers[x] ? 1 : 0;
      registers[x] = registers[y] - registers[x];
      break;
    }
    case 0xE: {
      auto [x, y] = get_registers(arguments);
      registers[15] = (registers[x] >> 7) & 1;
      registers[x] = registers[x] << 1;
      break;
    }
    }
    break;
  }
  case 0x9: {
    auto [x, y] = get_registers(arguments);
    if (registers[x] == registers[y]) {
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
    // Todo Drawing
    break;
  }
    // next instruction
  }
}
