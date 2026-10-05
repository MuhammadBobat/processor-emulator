#pragma once
// The instruction set, provided with the coursework (original header by Pavlos
// Petoumenos). Arithmetic, logic and memory instructions act on the accumulator
// and one memory address, JMP jumps to its address, and BNE branches when the
// accumulator is non-zero. ADD is the only arithmetic instruction (subtract by
// adding the two's complement, which an XOR with 0xFF gives); AND, ORR and XOR
// are the logical ones.

#include "common.h"
#include <string>

// Each subclass holds its mnemonic and opcode as inline static members, since
// both depend only on the class, not on the instance.

// An LDR instruction
class Ildr : public InstructionBase {
  public:
    Ildr(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"LDR"};
    inline static InstructionOpcode _opcode{LDR};
};

// An STR instruction
class Istr : public InstructionBase {
  public:
    Istr(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"STR"};
    inline static InstructionOpcode _opcode{STR};
};

// An unconditional JMP
class Ijmp : public InstructionBase {
  public:
    Ijmp(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"JMP"};
    inline static InstructionOpcode _opcode{JMP};
};

// A conditional BNE
class Ibne : public InstructionBase {
  public:
    Ibne(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"BNE"};
    inline static InstructionOpcode _opcode{BNE};
};

// An ADD instruction
class Iadd : public InstructionBase {
  public:
    Iadd(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"ADD"};
    inline static InstructionOpcode _opcode{ADD};
};

// An AND instruction
class Iand : public InstructionBase {
  public:
    Iand(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"AND"};
    inline static InstructionOpcode _opcode{AND};
};

// An ORR instruction
class Iorr : public InstructionBase {
  public:
    Iorr(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"ORR"};
    inline static InstructionOpcode _opcode{ORR};
};

// An XOR instruction
class Ixor : public InstructionBase {
  public:
    Ixor(addr_t address);
    void _execute(ProcessorState& state) const override;
    std::string name() const override;

    inline static const std::string _name{"XOR"};
    inline static InstructionOpcode _opcode{XOR};
};
