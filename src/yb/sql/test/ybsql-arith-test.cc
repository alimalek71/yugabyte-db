//--------------------------------------------------------------------------------------------------
// Copyright (c) YugaByte, Inc.
//--------------------------------------------------------------------------------------------------

#include <thread>
#include <cmath>

#include "yb/sql/test/ybsql-test-base.h"
#include "yb/gutil/strings/substitute.h"

using std::string;
using std::unique_ptr;
using std::shared_ptr;
using strings::Substitute;

namespace yb {
namespace sql {

class YbSqlArith : public YbSqlTestBase {
 public:
  YbSqlArith() : YbSqlTestBase() {
  }
};

TEST_F(YbSqlArith, TestSqlArithBigint) {
  // Init the simulated cluster.
  ASSERT_NO_FATALS(CreateSimulatedCluster());

  // Get a processor.
  YbSqlProcessor *processor = GetSqlProcessor();
  LOG(INFO) << "Running simple query test.";
  // Create the table 1.
  const char *create_stmt =
    "CREATE TABLE test_bigint(h1 int primary key, c1 bigint, c2 bigint, c3 bigint);";
  CHECK_VALID_STMT(create_stmt);

  // Simple counter update
  CHECK_VALID_STMT("UPDATE test_bigint SET c1 = 77, c2 = c2 + 77 WHERE h1 = 1;");

  // Select counter.
  CHECK_VALID_STMT("SELECT * FROM test_bigint WHERE h1 = 1");
  std::shared_ptr<YQLRowBlock> row_block = processor->row_block();
  CHECK_EQ(row_block->row_count(), 1);
  const YQLRow& row = row_block->row(0);
  CHECK_EQ(row.column(0).int32_value(), 1);
  CHECK_EQ(row.column(1).int64_value(), 77);
  CHECK(row.column(2).IsNull());
  CHECK(row.column(3).IsNull());

  // Simple counter update
  CHECK_VALID_STMT("UPDATE test_bigint SET c1 = c1 + 20, c2 = c1 + c1, c3 = c1 + c2 WHERE h1 = 1;");

  // Select counter.
  CHECK_VALID_STMT("SELECT * FROM test_bigint WHERE h1 = 1");
  row_block = processor->row_block();
  CHECK_EQ(row_block->row_count(), 1);
  const YQLRow& new_row = row_block->row(0);
  CHECK_EQ(new_row.column(0).int32_value(), 1);
  CHECK_EQ(new_row.column(1).int64_value(), 97);
  CHECK_EQ(new_row.column(2).int64_value(), 154);
  CHECK(new_row.column(3).IsNull());
}

TEST_F(YbSqlArith, TestSqlArithInt) {
  // Init the simulated cluster.
  ASSERT_NO_FATALS(CreateSimulatedCluster());

  // Get a processor.
  YbSqlProcessor *processor = GetSqlProcessor();
  LOG(INFO) << "Running simple query test.";
  // Create the table 1.
  const char *create_stmt =
    "CREATE TABLE test_int(h1 int primary key, c1 int, c2 int, c3 smallint, c4 tinyint);";
  CHECK_VALID_STMT(create_stmt);

  // Simple counter update
  CHECK_VALID_STMT("UPDATE test_int SET c1 = 77, c2 = c2 + 77 WHERE h1 = 1;");

  // Select counter.
  CHECK_VALID_STMT("SELECT * FROM test_int WHERE h1 = 1");
  std::shared_ptr<YQLRowBlock> row_block = processor->row_block();
  CHECK_EQ(row_block->row_count(), 1);
  const YQLRow& row = row_block->row(0);
  CHECK_EQ(row.column(0).int32_value(), 1);
  CHECK_EQ(row.column(1).int32_value(), 77);
  CHECK(row.column(2).IsNull());
  CHECK(row.column(3).IsNull());
  CHECK(row.column(4).IsNull());

  // Simple counter update
  CHECK_VALID_STMT("UPDATE test_int SET c1 = c1 + 20, c2 = c1 + c1, c3 = c1 + 3, c4 = c1 + 4"
                   "  WHERE h1 = 1;");

  // Select counter.
  CHECK_VALID_STMT("SELECT * FROM test_int WHERE h1 = 1");
  row_block = processor->row_block();
  CHECK_EQ(row_block->row_count(), 1);
  const YQLRow& new_row = row_block->row(0);
  CHECK_EQ(new_row.column(0).int32_value(), 1);
  CHECK_EQ(new_row.column(1).int32_value(), 97);
  CHECK_EQ(new_row.column(2).int32_value(), 154);
  CHECK_EQ(new_row.column(3).int16_value(), 80);
  CHECK_EQ(new_row.column(4).int8_value(), 81);
}

TEST_F(YbSqlArith, TestSqlArithCounter) {
  // Init the simulated cluster.
  ASSERT_NO_FATALS(CreateSimulatedCluster());

  // Get a processor.
  YbSqlProcessor *processor = GetSqlProcessor();
  LOG(INFO) << "Running simple query test.";
  // Create the table 1.
  const char *create_stmt =
    "CREATE TABLE test_counter(h1 int primary key, c1 counter, c2 counter, c3 counter);";
  CHECK_VALID_STMT(create_stmt);

  // Simple counter update
  CHECK_VALID_STMT("UPDATE test_counter SET c1 = c1 + 77 WHERE h1 = 1;");

  // Select counter.
  CHECK_VALID_STMT("SELECT * FROM test_counter WHERE h1 = 1");
  std::shared_ptr<YQLRowBlock> row_block = processor->row_block();
  CHECK_EQ(row_block->row_count(), 1);
  const YQLRow& row = row_block->row(0);
  CHECK_EQ(row.column(0).int32_value(), 1);
  CHECK_EQ(row.column(1).int64_value(), 77);

  // Simple counter update
  CHECK_VALID_STMT("UPDATE test_counter SET c1 = c1 + 10 WHERE h1 = 1;");

  // Select counter.
  CHECK_VALID_STMT("SELECT * FROM test_counter WHERE h1 = 1");
  row_block = processor->row_block();
  CHECK_EQ(row_block->row_count(), 1);
  const YQLRow& new_row = row_block->row(0);
  CHECK_EQ(new_row.column(0).int32_value(), 1);
  CHECK_EQ(new_row.column(1).int64_value(), 87);
}

TEST_F(YbSqlArith, TestSqlErrorArithCounter) {
  // Init the simulated cluster.
  ASSERT_NO_FATALS(CreateSimulatedCluster());

  // Get a processor.
  YbSqlProcessor *processor = GetSqlProcessor();
  LOG(INFO) << "Running simple query test.";
  // Create the table 1.
  const char *create_stmt =
    "CREATE TABLE test_counter(h1 int primary key, c1 counter, c2 counter, c3 counter);";
  CHECK_VALID_STMT(create_stmt);

  // Insert is not allowed.
  CHECK_INVALID_STMT("INSERT INTO test_counter(h1, c1) VALUES(1, 2);");

  // Update with constant.
  CHECK_INVALID_STMT("UPDATE test_counter SET c1 = 2 WHERE h1 = 1;");

  // Update with wrong column.
  CHECK_INVALID_STMT("UPDATE test_counter SET c1 = c2 + 3 WHERE h1 = 2;");
}

} // namespace sql
} // namespace yb
