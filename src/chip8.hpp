#pragma once

#include "constants.hpp"
#include <cstdint>

class Chip8 {
  uint8_t stack_ptr;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t opcode;
  uint16_t program_counter;
  uint16_t index;
  uint8_t registers[REGISTERS];
  uint8_t memory[MEMEORY_SIZE];
  uint16_t stack[STACK_SIZE];
  bool keys[KEYS];

  void advance_program_counter();

public:
  uint8_t framebuffer[FRAMERBUFFER_ROWS][FRAMERBUFFER_COLS];
  Chip8();
  void cycle();
  void debug_instruction();
  void load_rom(const char *filename);
  void set_key_status(uint8_t key, bool is_pressed);
};
