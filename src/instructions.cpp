#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include "instructions.h"
#include <string>

// ========== InstructionBase ==========
// Template method: each subclass supplies _execute(); the shared PC advance and
// bit-width masking happen here so no instruction can forget them.
void InstructionBase::execute(ProcessorState& state) const {
  _execute(state);

  state.pc += INSTRUCTION_SIZE;

  state.acc &= ARCH_BITMASK;
  state.pc &= ARCH_BITMASK;
}

addr_t InstructionBase::get_address() const {
  return _address;
}

void InstructionBase::_set_address(addr_t address) {
  _address = address & ARCH_BITMASK;
}

std::string InstructionBase::to_string() const {
  std::stringstream ss;
  ss << name() << ' ' << get_address();
  return ss.str();
}

// Factory: maps an opcode to an owning pointer to the matching instruction,
// or nullptr for an unknown opcode.
std::unique_ptr<InstructionBase> InstructionBase::generateInstruction(InstructionData data) {
  if (data.opcode == Ildr::_opcode)
    return std::make_unique<Ildr>(data.address);

  if (data.opcode == Istr::_opcode)
    return std::make_unique<Istr>(data.address);

  if (data.opcode == Ijmp::_opcode)
    return std::make_unique<Ijmp>(data.address);

  if (data.opcode == Ibne::_opcode)
    return std::make_unique<Ibne>(data.address);

  if (data.opcode == Iadd::_opcode)
    return std::make_unique<Iadd>(data.address);

  if (data.opcode == Iand::_opcode)
    return std::make_unique<Iand>(data.address);

  if (data.opcode == Iorr::_opcode)
    return std::make_unique<Iorr>(data.address);

  if (data.opcode == Ixor::_opcode)
    return std::make_unique<Ixor>(data.address);

  return nullptr;
}

// ========== LDR ==========
Ildr::Ildr(addr_t address) {
  _set_address(address);
}

void Ildr::_execute(ProcessorState& state) const {
  // .at() keeps every memory access bounds-checked.
  state.acc = state.memory.at(get_address());
}

std::string Ildr::name() const {
  return _name;
}

// ========== STR ==========
Istr::Istr(addr_t address) {
  _set_address(address);
}

void Istr::_execute(ProcessorState& state) const {
  state.memory.at(get_address()) = state.acc;
}

std::string Istr::name() const {
  return _name;
}

// ========== JMP ==========
Ijmp::Ijmp(addr_t address) {
  _set_address(address);
}

void Ijmp::_execute(ProcessorState& state) const {
  // The -2 cancels the PC increment execute() applies afterwards.
  state.pc = get_address() - 2;
}

std::string Ijmp::name() const {
  return _name;
}

// ========== BNE ==========
Ibne::Ibne(addr_t address) {
  _set_address(address);
}

void Ibne::_execute(ProcessorState& state) const {
  // Branch if the accumulator is non-zero (same -2 offset as JMP).
  if (state.acc != 0)
    state.pc = get_address() - 2;
}

std::string Ibne::name() const {
  return _name;
}

// ========== ADD ==========
Iadd::Iadd(addr_t address) {
  _set_address(address);
}

void Iadd::_execute(ProcessorState& state) const {
  state.acc += state.memory.at(get_address());
}

std::string Iadd::name() const {
  return _name;
}

// ========== AND ==========
Iand::Iand(addr_t address) {
  _set_address(address);
}

void Iand::_execute(ProcessorState& state) const {
  state.acc &= state.memory.at(get_address());
}

std::string Iand::name() const {
  return _name;
}

// ========== ORR ==========
Iorr::Iorr(addr_t address) {
  _set_address(address);
}

void Iorr::_execute(ProcessorState& state) const {
  state.acc |= state.memory.at(get_address());
}

std::string Iorr::name() const {
  return _name;
}

// ========== XOR ==========
Ixor::Ixor(addr_t address) {
  _set_address(address);
}

void Ixor::_execute(ProcessorState& state) const {
  state.acc ^= state.memory.at(get_address());
}

std::string Ixor::name() const {
  return _name;
}
