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
#include <mariadb.h>
#include <mysql/plugin_function.h>
#include <sql_class.h>

template <class Item_func_impl>
class Create_func_extra_misc_arg1 : public Create_func_arg1 {
public:
  Item *create_1_arg(THD *thd, Item *arg1) override {
    return new (thd->mem_root) Item_func_impl(thd, arg1);
  }

protected:
  Create_func_extra_misc_arg1() {}
  ~Create_func_extra_misc_arg1() override {}
};

class Create_func_unaccent
    : public Create_func_extra_misc_arg1<Item_func_unaccent> {
public:
  static Create_func_unaccent s_singleton;
};

class Create_func_slugify
    : public Create_func_extra_misc_arg1<Item_func_slugify> {
public:
  static Create_func_slugify s_singleton;
};

class Create_func_age : public Create_func_extra_misc_arg1<Item_func_age> {
public:
  static Create_func_age s_singleton;
};

class Create_func_human_number
    : public Create_func_extra_misc_arg1<Item_func_human_number> {
public:
  static Create_func_human_number s_singleton;
};

class Create_func_parse_duration
    : public Create_func_extra_misc_arg1<Item_func_parse_duration> {
public:
  static Create_func_parse_duration s_singleton;
};

Create_func_unaccent Create_func_unaccent::s_singleton;
Create_func_slugify Create_func_slugify::s_singleton;
Create_func_age Create_func_age::s_singleton;
Create_func_human_number Create_func_human_number::s_singleton;
Create_func_parse_duration Create_func_parse_duration::s_singleton;

#define BUILDER(F) &F::s_singleton

static Plugin_function
    plugin_descriptor_function_unaccent(BUILDER(Create_func_unaccent)),
    plugin_descriptor_function_slugify(BUILDER(Create_func_slugify)),
    plugin_descriptor_function_age(BUILDER(Create_func_age)),
    plugin_descriptor_function_human_number(BUILDER(Create_func_human_number)),
    plugin_descriptor_function_parse_duration(
        BUILDER(Create_func_parse_duration));

/*************************************************************************/

maria_declare_plugin(extra_misc){MariaDB_FUNCTION_PLUGIN,
                                 &plugin_descriptor_function_unaccent,
                                 "unaccent",
                                 "lefred",
                                 "Function UNACCENT()",
                                 PLUGIN_LICENSE_GPL,
                                 0,
                                 0,
                                 0x0100,
                                 NULL,
                                 NULL,
                                 "1.0",
                                 MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_slugify,
     "slugify",
     "lefred",
     "Function SLUGIFY()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_age,
     "age",
     "lefred",
     "Function AGE()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_human_number,
     "human_number",
     "lefred",
     "Function HUMAN_NUMBER()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_parse_duration,
     "parse_duration",
     "lefred",
     "Function PARSE_DURATION()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "0.2.0",
     MariaDB_PLUGIN_MATURITY_BETA} maria_declare_plugin_end;
