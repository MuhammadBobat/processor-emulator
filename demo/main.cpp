// Demo: builds a tiny program, runs it with a breakpoint, then round-trips the
// machine state through save_state / load_state.
#include <fstream>
#include <iostream>
#include "emulator.h"

namespace {

// Writes a state file holding a three-instruction program:
//   0: LDR 100   acc = mem[100]
//   2: ADD 101   acc += mem[101]
//   4: STR 102   mem[102] = acc
// with mem[100] = 3 and mem[101] = 4, and a breakpoint named "stored" at address 4.
void write_program(const char* path) {
  int memory[MEMORY_SIZE];
  for (int& byte : memory)
    byte = ARCH_MAXVAL;  // unused slots

  memory[0] = LDR; memory[1] = 100;
  memory[2] = ADD; memory[3] = 101;
  memory[4] = STR; memory[5] = 102;
  memory[100] = 3;
  memory[101] = 4;

  std::ofstream out(path);
  out << 0 << '\n' << 0 << '\n' << 0 << '\n';
  for (int byte : memory)
    out << byte << '\n';
  out << 4 << " stored\n";
}

}  // namespace

int main() {
  const char* program_file = "demo_program.txt";
  const char* saved_file = "demo_saved.txt";

  write_program(program_file);

  Emulator emu;
  if (!emu.load_state(program_file)) {
    std::cerr << "failed to load " << program_file << '\n';
    return 1;
  }

  std::cout << "Loaded program with " << emu.num_breakpoints() << " breakpoint(s)\n";

  emu.run(10);  // stops at the breakpoint, after the first two instructions
  std::cout << "Stopped at pc=" << emu.read_pc() << " acc=" << emu.read_acc()
            << " cycles=" << emu.cycles() << " (3 + 4 = 7)\n";

  emu.run(1);  // executes STR
  std::cout << "mem[102]=" << emu.read_mem(102) << '\n';

  emu.save_state(saved_file);
  Emulator restored;
  restored.load_state(saved_file);
  std::cout << "Restored: acc=" << restored.read_acc() << " pc=" << restored.read_pc()
            << " mem[102]=" << restored.read_mem(102) << '\n';

  return 0;
}
