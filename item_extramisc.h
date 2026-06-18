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

#ifndef ITEM_EXTRAMISC_INCLUDED
#define ITEM_EXTRAMISC_INCLUDED

#include "item.h"

class Item_func_unaccent : public Item_str_func {
public:
  Item_func_unaccent(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override {
    return get_item_copy<Item_func_unaccent>(thd, this);
  }
};

class Item_func_slugify : public Item_str_func {
public:
  Item_func_slugify(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override {
    return get_item_copy<Item_func_slugify>(thd, this);
  }
};

class Item_func_age : public Item_str_func {
public:
  Item_func_age(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override {
    return get_item_copy<Item_func_age>(thd, this);
  }
};

class Item_func_human_number : public Item_str_func {
public:
  Item_func_human_number(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override {
    return get_item_copy<Item_func_human_number>(thd, this);
  }
};

class Item_func_parse_duration : public Item_real_func {
public:
  Item_func_parse_duration(THD *thd, Item *arg1) : Item_real_func(thd, arg1) {}
  double val_real() override;
  longlong val_int() override;
  my_decimal *val_decimal(my_decimal *decimal_value) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override {
    return get_item_copy<Item_func_parse_duration>(thd, this);
  }
};

#endif
