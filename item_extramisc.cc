/* Copyright (c) 2026 lefred (Frederic Descamps)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

#define MYSQL_SERVER
#include "item_extramisc.h"
#include "common.h"
#include "mariadb.h"

#include "sql_class.h"
#include "tztime.h"

static const LEX_CSTRING unaccent_name = {STRING_WITH_LEN("unaccent")};
static const LEX_CSTRING slugify_name = {STRING_WITH_LEN("slugify")};
static const LEX_CSTRING age_name = {STRING_WITH_LEN("age")};
static const LEX_CSTRING human_number_name = {STRING_WITH_LEN("human_number")};
static const LEX_CSTRING parse_duration_name = {
    STRING_WITH_LEN("parse_duration")};

static String *return_std_string(String *str, const std::string &value) {
  str->copy(value.c_str(), value.length(), &my_charset_utf8mb3_general_ci);
  return str;
}

String *Item_func_unaccent::val_str(String *str) {
  StringBuffer<256> tmp;
  String *value = args[0]->val_str(&tmp);
  if ((null_value = args[0]->null_value || value == nullptr))
    return nullptr;
  return return_std_string(str,
                           extra_misc_unaccent(value->ptr(), value->length()));
}

bool Item_func_unaccent::fix_length_and_dec(THD *thd) {
  collation.set(&my_charset_utf8mb3_general_ci, DERIVATION_COERCIBLE);
  fix_char_length(args[0]->max_char_length());
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_unaccent::func_name_cstring() const {
  return unaccent_name;
}

String *Item_func_slugify::val_str(String *str) {
  StringBuffer<256> tmp;
  String *value = args[0]->val_str(&tmp);
  if ((null_value = args[0]->null_value || value == nullptr))
    return nullptr;
  return return_std_string(str,
                           extra_misc_slugify(value->ptr(), value->length()));
}

bool Item_func_slugify::fix_length_and_dec(THD *thd) {
  collation.set(&my_charset_utf8mb3_general_ci, DERIVATION_COERCIBLE);
  fix_char_length(args[0]->max_char_length());
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_slugify::func_name_cstring() const {
  return slugify_name;
}

String *Item_func_age::val_str(String *str) {
  THD *thd = current_thd;
  MYSQL_TIME ltime;
  if (args[0]->get_date(thd, &ltime, date_mode_t(0))) {
    null_value = true;
    return nullptr;
  }

  uint error = 0;
  my_time_t from = my_tz_OFFSET0->TIME_to_gmt_sec(&ltime, &error);
  if (error)
    return nullptr;

  MYSQL_TIME now = thd->query_start_TIME();
  error = 0;
  my_time_t to_sec = my_tz_OFFSET0->TIME_to_gmt_sec(&now, &error);
  if (error)
    return nullptr;

  time_t to = static_cast<time_t>(to_sec);
  null_value = false;
  return return_std_string(str, extra_misc_age(static_cast<time_t>(from), to));
}

bool Item_func_age::fix_length_and_dec(THD *thd) {
  collation.set(&my_charset_utf8mb3_general_ci, DERIVATION_COERCIBLE);
  fix_char_length(64);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_age::func_name_cstring() const { return age_name; }

String *Item_func_human_number::val_str(String *str) {
  double value = args[0]->val_real();
  if ((null_value = args[0]->null_value))
    return nullptr;
  return return_std_string(str, extra_misc_human_number(value));
}

bool Item_func_human_number::fix_length_and_dec(THD *thd) {
  collation.set(&my_charset_utf8mb3_general_ci, DERIVATION_COERCIBLE);
  fix_char_length(32);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_human_number::func_name_cstring() const {
  return human_number_name;
}

double Item_func_parse_duration::val_real() {
  StringBuffer<128> tmp;
  String *value = args[0]->val_str(&tmp);
  if ((null_value = args[0]->null_value || value == nullptr))
    return 0.0;

  double seconds = 0.0;
  if (!extra_misc_parse_duration(value->ptr(), value->length(), &seconds)) {
    my_error(ER_WRONG_VALUE, MYF(0), "duration", value->c_ptr_safe());
    null_value = true;
    return 0.0;
  }

  null_value = false;
  return seconds;
}

longlong Item_func_parse_duration::val_int() {
  return static_cast<longlong>(val_real());
}

my_decimal *Item_func_parse_duration::val_decimal(my_decimal *decimal_value) {
  double2my_decimal(E_DEC_FATAL_ERROR, val_real(), decimal_value);
  return decimal_value;
}

bool Item_func_parse_duration::fix_length_and_dec(THD *thd) {
  max_length = 20;
  decimals = 3;
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_parse_duration::func_name_cstring() const {
  return parse_duration_name;
}
