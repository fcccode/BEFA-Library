//
// Created by miro on 12/7/16.
//

#ifndef BEFA_CMP_HPP
#define BEFA_CMP_HPP

#include "instruction.hpp"
#include "binary_operation.hpp"

namespace llvm {

struct CmpInstruction
    : public BinaryOperation {
  using Instruction::sym_t;

  enum types_e {
        // greater than     greater or equal
        GT,                 GE,
        // lesser than      lesser or equal
        LT,                 LE,
        // unsigned GT      unsigned GE
        UGT,                UGE,
        // unsigned LT      unsigned LE
        ULT,                ULE,
        // equal            not equal
        EQ,                 NE,
  };

  // FIXME: remove this
  static const std::map<
      std::string,               types_e
  >   str_to_jmp;
  static const std::map<
      types_e,              std::string
  >   jmp_to_str;

  void accept(
      VisitorBase&               visitor
  )   const                      override {
    visitor.visit(this);
  }

  std::string                  toString() const override;

  /**
   *
   * @param result where to safe the output of comparition
   * @param lhs left hand side of operation
   * @param op compare operation
   * @param rhs right hand side of operation
   * @param assembly
   */
  CmpInstruction(
      a_ir_t::c_info  ::ref      assembly,
      const sym_t::ptr::shared&  result,
      const sym_t::ptr::shared&  lhs,
      types_e                    op,
      const sym_t::ptr::shared&  rhs
  );

 private:
  static inline
  std::string              fetch_name(
      const sym_t::ptr::shared&  result,
      const sym_t::ptr::shared&  lhs,
      types_e                    op,
      const sym_t::ptr::shared&  rhs
  );
};

struct CompareFactory
    : public LLVMFactory {
  using symbol_type =            symbol_table::VisitableBase;
  using symbol_ptr  =            std::shared_ptr<symbol_type>;

  void                           operator()(
      a_ir_t::c_info::ref        instruction,
      sym_table_t::ptr::shared   symbol_table,
      ir_t::rx::shared_subs      subscriber
  )   const                      override;
};
}  // namespace llvm

#endif //BEFA_CMP_HPP
