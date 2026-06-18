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

#ifndef EXTRA_MISC_COMMON_INCLUDED
#define EXTRA_MISC_COMMON_INCLUDED

#include <string>

std::string extra_misc_unaccent(const char *str, size_t length);
std::string extra_misc_slugify(const char *str, size_t length);
std::string extra_misc_human_number(double value);
bool extra_misc_parse_duration(const char *str, size_t length, double *seconds);
std::string extra_misc_age(time_t from, time_t to);

#endif
