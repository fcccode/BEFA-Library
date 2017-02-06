#include <befa.hpp>

// ExecutableFile types we'll be using
using instr_type = ExecutableFile::inst_t::type;
using basic_block_type = ExecutableFile::bb_t::type;
using symbol_type = ExecutableFile::sym_t::type;
using section_type = ExecutableFile::sec_t::type;

int main(int argc, const char **argv) {
  // check if first param is not missing
  assert_ex(argc == 2, "missing path parameter");

  // open file
  auto file = ExecutableFile::open(*(argv + 1));

  // check if file is valid
  assert_ex(file.isValid(), "file is not valid");

  // generate symbol table
  auto sym_table_symbols = file.generate_table();

  file.disassembly()
          // RxCpp::reduce(...) function
      .reduce(
          // default last basic block,         symbol,         section
          std::make_tuple((bfd_vma)-1, std::string(""), std::string("")),
          [&sym_table_symbols](
              auto acc,
              const auto &instruction
          ) {
            auto bb = instruction.getParent();
            auto sym = ptr_lock(bb->getParent());
            auto sec = ptr_lock(sym->getParent());

            auto last_sec = sec->getName();
            auto last_sym = sym->getName();
            auto last_bb = bb->getId();

            if (std::get<2>(acc) != last_sec) {
              printf("%s <0x%08lx>:\n",
                     sec->getName().c_str(),
                     sec->getAddress()
              );
            }
            if (std::get<1>(acc) != last_sym) {
              printf("  %s <0x%08lx>:\n",
                     sym->getName().c_str(),
                     sym->getAddress()
              );
            }
            if (std::get<0>(acc) != last_bb) {
              printf("    BasicBlock #%lu <0x%08lx>:\n",
                     bb->getId(),
                     instruction.getAddress()
              );
            }

                // iterate through arguments of instruction
            instruction
                .getArgs(sym_table_symbols)

                    // convert to string
                .map([](
                    auto arg
                ) {
                  // try to use 'arg' as a Symbol
                  // if not, default value "" will be mapped instead
                  return map_visitable<symbol_table::SymbolVisitorL>(
                      arg, [](const symbol_table::Symbol *ptr)
                            { return ptr->getName(); }
                  );
                })

                    // filter out empty (non-symbol stuff)
                .filter([](
                    auto name
                ) { return name != ""; })

                    // string join achieved by reduction
                .reduce(std::string(""), [](
                    auto seed, auto b
                ) { return seed == "" ? b : seed + ", " + b; })

                    // print instruction
                .subscribe([&instruction](
                    auto str_params
                ) {
                  printf("      <0x%08lx> %s %s\n",
                         instruction.getAddress(),
                         instruction.getName().c_str(),
                         str_params.c_str());
                });

            std::get<2>(acc) = last_sec;
            std::get<1>(acc) = last_sym;
            std::get<0>(acc) = last_bb;

            return acc;
          }  // lambda::reduce()
          // we don't care about result of reduction
          // output we need is in the console
      ).subscribe([](const auto &) {});

  // executes all
  file.runDisassembler();
  return 0;
}
