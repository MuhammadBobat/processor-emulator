#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include "emulator.h"
#include <string>

// ============= Breakpoint ==============
Breakpoint::Breakpoint() = default;

Breakpoint::Breakpoint(addr_t address, const char* name)
  : _address(address & ARCH_BITMASK), _name(name) {}

addr_t Breakpoint::get_address() const {
  return _address;
}

const char* Breakpoint::get_name() const {
  return _name.c_str();
}

int Breakpoint::has(addr_t address) const {
  return _address == (address & ARCH_BITMASK);
}

int Breakpoint::has(const char* name) const {
  return _name == name;
}

// ============= Emulator ==============
Emulator::Emulator() = default;

// ----------> Main emulation loop

InstructionData Emulator::fetch() const {
  InstructionData data{};
  data.opcode = state.memory.at(state.pc);
  data.address = state.memory.at(state.pc + 1);
  return data;
}

std::unique_ptr<InstructionBase> Emulator::decode(InstructionData data) const {
  return InstructionBase::generateInstruction(data);
}

int Emulator::execute(InstructionBase* instr) {
  instr->execute(state);
  return 1;
}

int Emulator::run(int steps) {
  if (steps == 0)
    return 1;

  // Return 0 on an error, 1 on a breakpoint or when the steps run out.
  for (; steps > 0; --steps) {
    // Instructions are two-byte aligned, so an odd PC is an error.
    if ((state.pc % 2) == 1)
      return 0;

    // The unique_ptr owns the decoded instruction, so it cannot leak.
    std::unique_ptr<InstructionBase> instr(decode(fetch()));

    if (instr == nullptr)
      return 0;

    if (execute(instr.get()) == 0)
      return 0;

    ++total_cycles;

    if (is_breakpoint() == 1)
      return 1;
  }

  return 1;
}

// ----------> Breakpoint management

int Emulator::insert_breakpoint(addr_t address, const char* name) {
  if (static_cast<int>(breakpoints.size()) == MAX_INSTRUCTIONS)
    return 0;
  if (find_breakpoint(address) != nullptr)
    return 0;
  if (find_breakpoint(name) != nullptr)
    return 0;
  breakpoints.emplace_back(address, name);
  return 1;
}

const Breakpoint* Emulator::find_breakpoint(addr_t address) const {
  auto it = std::find_if(breakpoints.begin(), breakpoints.end(),
    [address](const Breakpoint& bp) { return bp.has(address); });

  return (it != breakpoints.end()) ? &*it : nullptr;
}

const Breakpoint* Emulator::find_breakpoint(const char* name) const {
  auto it = std::find_if(breakpoints.begin(), breakpoints.end(),
    [name](const Breakpoint& bp) { return bp.has(name); });

  return (it != breakpoints.end()) ? &*it : nullptr;
}

int Emulator::num_breakpoints() const {
  return static_cast<int>(breakpoints.size());
}

// ----------> State access

int Emulator::cycles() const {
  return total_cycles;
}

data_t Emulator::read_acc() const {
  return state.acc;
}

addr_t Emulator::read_pc() const {
  return state.pc;
}

addr_t Emulator::read_mem(addr_t address) const {
  address &= ARCH_BITMASK;
  return state.memory.at(address);
}

// ----------> Utilities

int Emulator::is_zero() const {
  return state.acc == 0;
}

int Emulator::is_breakpoint() const {
  return find_breakpoint(state.pc) != nullptr;
}

int Emulator::print_program() const {
  for (int offset = 0; offset < MEMORY_SIZE; offset += INSTRUCTION_SIZE) {
    InstructionData data{};
    data.opcode = state.memory.at(offset);
    data.address = state.memory.at(offset + 1);
    std::unique_ptr<InstructionBase> instr(decode(data));

    // Unused (0xFF 0xFF) or undecodable slots print as raw bytes only.
    if ((instr == nullptr) || (data.opcode == static_cast<byte_t>(ARCH_BITMASK) && data.address == static_cast<byte_t>(ARCH_BITMASK)))
      std::cout << offset << ":\t" << static_cast<int>(data.opcode) << '\t' << static_cast<int>(data.address) << '\n';
    else {
      std::cout << offset << ":\t" << static_cast<int>(data.opcode) << '\t' << static_cast<int>(data.address) << "\t:\t" << instr->to_string() << '\n';
    }
  }
  return 1;
}

int Emulator::load_state(const char* filename) {
  breakpoints.clear();

  std::ifstream in(filename);
  if (!in.is_open())
    return 0;

  // Validate every field before accepting it.
  if (!(in >> total_cycles) || total_cycles < 0)
    return 0;

  if (!(in >> state.acc) || state.acc > ARCH_MAXVAL || state.acc < 0)
    return 0;
  if (!(in >> state.pc) || state.pc >= MEMORY_SIZE || state.pc < 0)
    return 0;

  // Read into an int so the bytes parse as numbers, not characters.
  int num = 0;
  for (int offset = 0; offset < MEMORY_SIZE; ++offset) {
    if (!(in >> num) || num > ARCH_MAXVAL || num < 0)
      return 0;

    state.memory.at(offset) = static_cast<byte_t>(num);
  }

  // The rest of the file holds one "address name" pair per breakpoint.
  while (true) {
    int addr = 0;
    std::string name;
    if (!(in >> addr >> name))
      break;
    if (addr < 0 || addr >= MEMORY_SIZE)
      return 0;
    if (!insert_breakpoint(addr, name.c_str()))
      return 0;
  }

  return 1;
}

int Emulator::save_state(const char* filename) const {
  std::ofstream out(filename);
  if (!out.is_open())
    return 0;

  out << total_cycles << '\n' << state.acc << '\n' << state.pc << '\n';
  for (int offset = 0; offset < MEMORY_SIZE; ++offset)
    out << static_cast<int>(state.memory.at(offset)) << '\n';
  for (size_t idx = 0; idx < breakpoints.size(); ++idx)
    out << breakpoints[idx].get_address() << ' ' << breakpoints[idx].get_name() << '\n';

  return 1;
}
