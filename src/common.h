#pragma once
// Shared constants and types. Provided with the coursework (original header by
// Pavlos Petoumenos).

#include <inttypes.h>
#include <array>
#include <memory>
#include <string>

constexpr int ARCH_BITS = 8;
constexpr int ARCH_BITMASK = (1 << ARCH_BITS) - 1;
constexpr int ARCH_MAXVAL = ARCH_BITMASK;
constexpr int INSTRUCTION_SIZE = 2;
constexpr int MEMORY_SIZE = 256;
constexpr int MAX_STRING = 96;

enum InstructionOpcode {
  LDR = 0,
  STR,
  JMP,
  BNE,
  ADD,
  AND,
  ORR,
  XOR,
  NUM_OPCODES
};

// Addresses and generic data are plain ints. They are 8-bit in the architecture,
// but int avoids narrowing warnings on arithmetic. A wrapper struct would catch
// misuse at compile time, but the provided tests expect plain ints.
typedef int addr_t;
typedef int data_t;

// One memory byte (unsigned, exactly 8 bits).
typedef uint8_t byte_t;

// The two raw bytes of an instruction in memory.
struct InstructionData {
  byte_t opcode;
  byte_t address;
};

// All the processor's state, kept separate so both the emulator and the
// instructions can access it.
struct ProcessorState {
  data_t acc{0};   // the only general purpose register
  addr_t pc{0};    // address of the next instruction
  std::array<byte_t, MEMORY_SIZE> memory{};

  // Resets memory to 0xFF, which decodes as an unused slot.
  ProcessorState() {
    memory.fill(static_cast<byte_t>(ARCH_BITMASK));
  }
};

// Abstract base for all instructions. Subclasses implement _execute() and
// name(); execute() wraps _execute() with the shared PC and bit-width handling.
class InstructionBase {
  public:
    virtual ~InstructionBase() = default;
    InstructionBase(const InstructionBase&) = delete;
    InstructionBase& operator=(const InstructionBase&) = delete;
    InstructionBase(InstructionBase&&) = delete;
    InstructionBase& operator=(InstructionBase&&) = delete;

    // Run the instruction, then advance the PC and keep the accumulator and PC
    // within 8 bits.
    void execute(ProcessorState& state) const;

    addr_t get_address() const;

    // Human-readable form, e.g. "LDR 12".
    std::string to_string() const;

    virtual void _execute(ProcessorState& state) const = 0;
    virtual std::string name() const = 0;

    // Factory: opcode -> owning pointer to the matching instruction.
    static std::unique_ptr<InstructionBase> generateInstruction(InstructionData data);

  protected:
    InstructionBase() = default;

    // Only the subclass constructors set the address; it never changes after.
    void _set_address(addr_t address);

  private:
    addr_t _address{0};
};
