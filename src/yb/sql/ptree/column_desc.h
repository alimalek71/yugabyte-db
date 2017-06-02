//--------------------------------------------------------------------------------------------------
// Copyright (c) YugaByte, Inc.
//
// Structure definitions for column descriptor of a table.
//--------------------------------------------------------------------------------------------------

#ifndef YB_SQL_PTREE_COLUMN_DESC_H_
#define YB_SQL_PTREE_COLUMN_DESC_H_

#include "yb/client/client.h"
#include "yb/common/types.h"
#include "yb/sql/ptree/pt_type.h"
#include "yb/util/memory/mc_types.h"

namespace yb {
namespace sql {

//--------------------------------------------------------------------------------------------------

// This class can be used to describe any reference of a column.
class ColumnDesc {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef std::shared_ptr<ColumnDesc> SharedPtr;
  typedef std::shared_ptr<const ColumnDesc> SharedPtrConst;

  ColumnDesc()
      : index_(-1),
        id_(-1),
        is_hash_(false),
        is_primary_(false),
        is_static_(false),
        yql_type_(YQLType::Create(DataType::UNKNOWN_DATA)),
        internal_type_(InternalType::VALUE_NOT_SET) {
  }

  void Init(int index,
            int id,
            bool is_hash,
            bool is_primary,
            bool is_static,
            bool is_counter,
            const std::shared_ptr<YQLType>& yql_type,
            InternalType internal_type) {
    index_ = index,
    id_ = id;
    is_hash_ = is_hash;
    is_primary_ = is_primary;
    is_static_ = is_static;
    is_counter_ = is_counter;
    yql_type_ = yql_type;
    internal_type_ = internal_type;
  }

  bool IsInitialized() const {
    return (index_ >= 0);
  }

  int index() const {
    return index_;
  }

  int id() const {
    return id_;
  }

  bool is_hash() const {
    return is_hash_;
  }

  bool is_primary() const {
    return is_primary_;
  }

  bool is_static() const {
    return is_static_;
  }

  bool is_counter() const {
    return is_counter_;
  }

  std::shared_ptr<YQLType> yql_type() const {
    return yql_type_;
  }

  InternalType internal_type() const {
    return internal_type_;
  }

 private:
  int index_;
  int id_;
  bool is_hash_;
  bool is_primary_;
  bool is_static_;
  bool is_counter_;
  std::shared_ptr<YQLType> yql_type_;
  InternalType internal_type_;
};

}  // namespace sql
}  // namespace yb

#endif  // YB_SQL_PTREE_COLUMN_DESC_H_
