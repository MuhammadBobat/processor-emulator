#pragma once
// 8-bit accumulator-based processor emulator with step execution and breakpoints.
// The interface skeleton was provided with the coursework (original header by
// Pavlos Petoumenos); the implementation and the template additions are mine.

#include "common.h"
#include <string>
#include <vector>

#define MAX_INSTRUCTIONS ((MEMORY_SIZE) / (INSTRUCTION_SIZE))

// A breakpoint: the address we break on plus a symbolic name.
class Breakpoint {
  public:
    Breakpoint();
    Breakpoint(addr_t address, const char* name);

    Breakpoint(const Breakpoint& other) = default;
    Breakpoint(Breakpoint&& other) noexcept = default;
    Breakpoint& operator=(const Breakpoint& other) = default;
    Breakpoint& operator=(Breakpoint&& other) noexcept = default;
    ~Breakpoint() = default;

    addr_t get_address() const;
    const char* get_name() const;

    // True if the breakpoint targets this address / name.
    int has(addr_t address) const;
    int has(const char* name) const;

  private:
    addr_t _address{0};
    std::string _name{};
};

class Emulator {
  public:
    Emulator();

    Emulator(const Emulator& other) = default;
    Emulator(Emulator&& other) noexcept = default;
    Emulator& operator=(const Emulator& other) = default;
    Emulator& operator=(Emulator&& other) noexcept = default;
    ~Emulator() = default;

    // ----------> Main emulation loop

    // Read the two bytes of the instruction at the PC.
    InstructionData fetch() const;

    // Build an owning InstructionBase-derived object from the raw bytes
    // (nullptr for an unknown opcode).
    std::unique_ptr<InstructionBase> decode(InstructionData instruction) const;

    // Run one instruction. Returns 1 on success, 0 on failure.
    int execute(InstructionBase* instr);

    // Run up to `steps` cycles. Returns 1 if we stopped normally (breakpoint or
    // step limit reached), 0 on an error (odd PC, bad opcode, bad access).
    int run(int steps);

    // ----------> Breakpoint management

    // Fails (returns 0) if the address or name is already registered.
    int insert_breakpoint(addr_t address, const char* name);

    // Non-owning pointer to the matching breakpoint, or nullptr.
    const Breakpoint* find_breakpoint(addr_t address) const;
    const Breakpoint* find_breakpoint(const char* name) const;

    // Remove a breakpoint by address (addr_t) or name (const char*).
    // One template covers both lookups. Returns 1 if one was removed.
    template <typename T>
    int delete_breakpoint(T key) {
      const Breakpoint* found = find_breakpoint(key);

      if (found == nullptr)
        return 0;

      int found_idx = static_cast<int>(found - &breakpoints[0]);
      breakpoints.erase(breakpoints.begin() + found_idx);
      return 1;
    }

    int num_breakpoints() const;

    // ----------> State access

    int cycles() const;
    data_t read_acc() const;
    addr_t read_pc() const;
    addr_t read_mem(addr_t address) const;

    // ----------> Utilities

    int is_zero() const;        // accumulator == 0
    int is_breakpoint() const;  // is there a breakpoint at the current PC
    int print_program() const;  // disassemble all of memory to stdout

    // State file format:
    //   line 1        total cycles executed
    //   line 2        accumulator
    //   line 3        PC
    //   lines 4-259   the 256 memory bytes, one per line
    //   remaining     one "address name" pair per breakpoint
    // load_state validates every field and returns 0 on malformed input.
    int load_state(const char* state_filename);
    int save_state(const char* state_filename) const;

    // Count the instructions of type T in memory. Templated, so the definition
    // lives in the header.
    template <typename T>
    int count_instructions() {
      int count = 0;

      // Instructions are two bytes, so only even offsets hold opcodes.
      for (int offset = 0; offset < MEMORY_SIZE; offset += 2)
        if (state.memory.at(offset) == T::_opcode)
          ++count;
      return count;
    }

  private:
    ProcessorState state{};
    std::vector<Breakpoint> breakpoints{};
    int total_cycles{0};
};
