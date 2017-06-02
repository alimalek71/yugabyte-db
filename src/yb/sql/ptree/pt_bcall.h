//--------------------------------------------------------------------------------------------------
// Copyright (c) YugaByte, Inc.
//
// Tree node definitions for expression.
//--------------------------------------------------------------------------------------------------

#ifndef YB_SQL_PTREE_PT_BCALL_H_
#define YB_SQL_PTREE_PT_BCALL_H_

#include "yb/sql/ptree/pt_expr.h"

namespace yb {
namespace sql {

// Expression node that represents builtin function calls.
class PTBcall : public PTExpr {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef MCSharedPtr<PTBcall> SharedPtr;
  typedef MCSharedPtr<const PTBcall> SharedPtrConst;

  //------------------------------------------------------------------------------------------------
  // Constructor and destructor.
  PTBcall(MemoryContext *memctx,
          YBLocation::SharedPtr loc,
          const MCSharedPtr<MCString>& name,
          PTExprListNode::SharedPtr args);
  virtual ~PTBcall();

  // Support for shared_ptr.
  template<typename... TypeArgs>
  inline static PTBcall::SharedPtr MakeShared(MemoryContext *memctx, TypeArgs&&... args) {
    return MCMakeShared<PTBcall>(memctx, std::forward<TypeArgs>(args)...);
  }

  // Node semantics analysis.
  virtual CHECKED_STATUS Analyze(SemContext *sem_context) override;

  // Access API for arguments.
  const MCList<PTExpr::SharedPtr>& args() const {
    return args_->node_list();
  }

  // Access API for opcode.
  bfyql::BFOpcode bf_opcode() const {
    return bf_opcode_;
  }

  // Access API for cast opcodes.
  const MCVector<yb::bfyql::BFOpcode>& cast_ops() const {
    return cast_ops_;
  }

  yb::bfyql::BFOpcode result_cast_op() const {
    return result_cast_op_;
  }

  const MCSharedPtr<MCString>& name() const {
    return name_;
  }

  virtual CHECKED_STATUS CheckOperator(SemContext *sem_context) override;

  virtual CHECKED_STATUS CheckCounterUpdateSupport(SemContext *sem_context) const override;

 private:
  // Builtin function name.
  MCSharedPtr<MCString> name_;

  // Arguments to builtin call.
  PTExprListNode::SharedPtr args_;

  // Builtin opcode.
  bfyql::BFOpcode bf_opcode_;

  // Casting arguments to correct datatype before calling the builtin-function.
  MCVector<yb::bfyql::BFOpcode> cast_ops_;

  // Casting the returned result to expected type is also needed.
  yb::bfyql::BFOpcode result_cast_op_;
};

class PTToken : public PTBcall {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef MCSharedPtr<PTToken> SharedPtr;
  typedef MCSharedPtr<const PTToken> SharedPtrConst;

  //------------------------------------------------------------------------------------------------
  // Constructor and destructor.
  PTToken(MemoryContext *memctx,
          YBLocation::SharedPtr loc,
          const MCSharedPtr<MCString>& name,
          PTExprListNode::SharedPtr args) : PTBcall(memctx, loc, name, args) { }

  virtual ~PTToken() { }

  // Support for shared_ptr.
  template<typename... TypeArgs>
  inline static PTToken::SharedPtr MakeShared(MemoryContext *memctx, TypeArgs&&... args) {
    return MCMakeShared<PTToken>(memctx, std::forward<TypeArgs>(args)...);
  }

  // Node semantics analysis.
  virtual CHECKED_STATUS Analyze(SemContext *sem_context) override;

  // Check if token call is well formed before analyzing it
  virtual CHECKED_STATUS CheckOperator(SemContext *sem_context) override;

  bool is_partition_key_ref() const {
    return is_partition_key_ref_;
  }

 private:
  // true if this token call is just reference to the partition key, e.g.: "token(h1, h2, h3)"
  // TODO not supported yet: false for regular builtin calls to be evaluated, e.g.: "token(2,3,4)"
  bool is_partition_key_ref_ = false;
};

}  // namespace sql
}  // namespace yb

#endif  // YB_SQL_PTREE_PT_BCALL_H_
