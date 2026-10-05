# 8-bit Processor Emulator (C++)

An emulator for a simple 8-bit accumulator processor, with step execution,
breakpoints and save/load of the full machine state. Written for a university
C++ module.

The task specification and the header skeleton (`common.h`, `instructions.h` and
the interface in `emulator.h`) were provided with the coursework, credited to
Pavlos Petoumenos. The implementation in the `.cpp` files is my own.

## Files

- `src/common.h` - constants, shared types, `ProcessorState` and the `InstructionBase` interface.
- `src/instructions.h` - declarations of the eight instruction classes.
- `src/emulator.h` - `Emulator` and `Breakpoint` classes.
- `src/emulator.cpp` - fetch / decode / execute loop, breakpoints, state save and load.
- `src/instructions.cpp` - the instruction hierarchy (LDR, STR, JMP, BNE, ADD, AND, ORR, XOR).
- `demo/main.cpp` - a small program showing load, run, breakpoint and save/restore.
- `Makefile` - build, run, sanitizer and clang-tidy targets.

## Building and running

```
make run        # build and run the demo
make sanitize   # rebuild with AddressSanitizer and UBSan, then run
make tidy       # run clang-tidy (needs clang-tidy installed)
```

The demo (`demo/main.cpp`) loads a three-instruction program (`LDR`, `ADD`, `STR`)
with a breakpoint, runs it, saves the machine state and restores it into a fresh
emulator. Expected output:

```
Loaded program with 1 breakpoint(s)
Stopped at pc=4 acc=7 cycles=2 (3 + 4 = 7)
mem[102]=7
Restored: acc=7 pc=6 mem[102]=7
```

## Design notes

- **Ownership.** `decode()` returns a `std::unique_ptr<InstructionBase>` built by a
  factory, so a decoded instruction is owned by exactly one scope and cannot leak.
  There is no raw `new` or `delete`, and no C-style arrays or strings.
- **Template method.** `InstructionBase::execute()` runs the subclass `_execute()`,
  then advances the PC and masks the accumulator and PC to the architecture's width.
  Individual instructions cannot forget either step.
- **Bounds safety.** Memory is accessed through `.at()` throughout, so a bad address
  fails loudly instead of corrupting state.
- **Validated I/O.** `load_state()` checks every field (cycle count, accumulator, PC,
  each memory byte, each breakpoint) and returns failure on malformed input.
- **Templates.** `delete_breakpoint<T>` handles lookup by address or by name with one
  definition, and `count_instructions<T>` counts instructions of a given type.
- **Tooling.** The code was checked with clang-tidy and AddressSanitizer.

The coursework test suite is not included. All sources compile with
`g++ -std=c++17 -Wall -Wextra`.
