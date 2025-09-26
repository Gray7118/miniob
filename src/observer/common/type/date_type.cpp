/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/date_type.h"
#include "common/value.h"
#include "storage/common/column.h"


int DateType::compare(const Value &left, const Value &right) const
{
  // 检查输入合法性
  ASSERT(left.attr_type() == AttrType::DATES, "left type is not date");
  ASSERT(right.attr_type() == AttrType::DATES, "right type is not date");

  return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
}


int DateType::compare(const Column &left, const Column &right, int left_idx, int right_idx) const
{
  ASSERT(left.attr_type() == AttrType::DATES, "left type is not date");
  ASSERT(right.attr_type() == AttrType::DATES, "right type is not date");

  return common::compare_int((void *)&((int*)left.data())[left_idx],
      (void *)&((int*)right.data())[right_idx]);
}

RC DateType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
  case AttrType::FLOATS: {
    float float_value = val.get_int();
    result.set_float(float_value);
    return RC::SUCCESS;
  }
  default:
    LOG_WARN("unsupported type %d", type);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
}

// 字符串转换为DATE
RC DateType::set_value_from_str(Value &val, const string &data) const
{
  RC rc = RC::SUCCESS;

  int year, month, day;
  bool is_valid_date = val.str_to_date(data.c_str(), data.length(), year, month, day);

  if (!is_valid_date) {
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else {
    int date_value = year * 10000 + month * 100 + day;
    val.set_date(date_value);
  }

  return rc;
}


// DATE转化为字符串
RC DateType::to_string(const Value &val, string &result) const
{
  int date_value = val.value_.int_value_;
  int year = date_value / 10000;
  int month = (date_value % 10000) / 100;
  int day = date_value % 100;

  stringstream ss;
  ss << year << "-" << (month < 10 ? "0" : "") << month << "-" << (day < 10 ? "0" : "") << day;
  result = ss.str();
  return RC::SUCCESS;
}