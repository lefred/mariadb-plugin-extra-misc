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
#include "common.h"
#include "mariadb.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

static bool starts_with(const char *str, size_t length, const char *prefix) {
  size_t prefix_length = strlen(prefix);
  return length >= prefix_length && memcmp(str, prefix, prefix_length) == 0;
}

static bool map_utf8(const char *str, size_t length, size_t *consumed,
                     const char **replacement) {
  struct Mapping {
    const char *from;
    const char *to;
  };

  static const Mapping mappings[] = {
      {"\xC3\x80", "A"},  {"\xC3\x81", "A"}, {"\xC3\x82", "A"},
      {"\xC3\x83", "A"},  {"\xC3\x84", "A"}, {"\xC3\x85", "A"},
      {"\xC4\x80", "A"},  {"\xC4\x82", "A"}, {"\xC4\x84", "A"},
      {"\xC3\x86", "AE"}, {"\xC3\x87", "C"}, {"\xC4\x86", "C"},
      {"\xC4\x8C", "C"},  {"\xC3\x88", "E"}, {"\xC3\x89", "E"},
      {"\xC3\x8A", "E"},  {"\xC3\x8B", "E"}, {"\xC4\x92", "E"},
      {"\xC4\x98", "E"},  {"\xC3\x8C", "I"}, {"\xC3\x8D", "I"},
      {"\xC3\x8E", "I"},  {"\xC3\x8F", "I"}, {"\xC4\xAA", "I"},
      {"\xC3\x91", "N"},  {"\xC5\x83", "N"}, {"\xC3\x92", "O"},
      {"\xC3\x93", "O"},  {"\xC3\x94", "O"}, {"\xC3\x95", "O"},
      {"\xC3\x96", "O"},  {"\xC3\x98", "O"}, {"\xC5\x8C", "O"},
      {"\xC5\x92", "OE"}, {"\xC3\x99", "U"}, {"\xC3\x9A", "U"},
      {"\xC3\x9B", "U"},  {"\xC3\x9C", "U"}, {"\xC5\xAA", "U"},
      {"\xC3\x9D", "Y"},  {"\xC5\xB8", "Y"}, {"\xC5\xBD", "Z"},
      {"\xC3\xA0", "a"},  {"\xC3\xA1", "a"}, {"\xC3\xA2", "a"},
      {"\xC3\xA3", "a"},  {"\xC3\xA4", "a"}, {"\xC3\xA5", "a"},
      {"\xC4\x81", "a"},  {"\xC4\x83", "a"}, {"\xC4\x85", "a"},
      {"\xC3\xA6", "ae"}, {"\xC3\xA7", "c"}, {"\xC4\x87", "c"},
      {"\xC4\x8D", "c"},  {"\xC3\xA8", "e"}, {"\xC3\xA9", "e"},
      {"\xC3\xAA", "e"},  {"\xC3\xAB", "e"}, {"\xC4\x93", "e"},
      {"\xC4\x99", "e"},  {"\xC3\xAC", "i"}, {"\xC3\xAD", "i"},
      {"\xC3\xAE", "i"},  {"\xC3\xAF", "i"}, {"\xC4\xAB", "i"},
      {"\xC3\xB1", "n"},  {"\xC5\x84", "n"}, {"\xC3\xB2", "o"},
      {"\xC3\xB3", "o"},  {"\xC3\xB4", "o"}, {"\xC3\xB5", "o"},
      {"\xC3\xB6", "o"},  {"\xC3\xB8", "o"}, {"\xC5\x8D", "o"},
      {"\xC5\x93", "oe"}, {"\xC3\xB9", "u"}, {"\xC3\xBA", "u"},
      {"\xC3\xBB", "u"},  {"\xC3\xBC", "u"}, {"\xC5\xAB", "u"},
      {"\xC3\xBD", "y"},  {"\xC3\xBF", "y"}, {"\xC5\xBE", "z"},
      {"\xC3\x9F", "ss"}};

  for (const Mapping &mapping : mappings) {
    size_t mapping_length = strlen(mapping.from);
    if (starts_with(str, length, mapping.from)) {
      *consumed = mapping_length;
      *replacement = mapping.to;
      return true;
    }
  }
  return false;
}

std::string extra_misc_unaccent(const char *str, size_t length) {
  std::string out;
  out.reserve(length);
  for (size_t i = 0; i < length;) {
    const unsigned char c = static_cast<unsigned char>(str[i]);
    if (c < 0x80) {
      out.push_back(static_cast<char>(c));
      i++;
      continue;
    }

    size_t consumed = 0;
    const char *replacement = nullptr;
    if (map_utf8(str + i, length - i, &consumed, &replacement)) {
      out.append(replacement);
      i += consumed;
      continue;
    }

    i++;
    while (i < length && (static_cast<unsigned char>(str[i]) & 0xc0) == 0x80)
      i++;
  }
  return out;
}

std::string extra_misc_slugify(const char *str, size_t length) {
  std::string plain = extra_misc_unaccent(str, length);
  std::string out;
  bool pending_dash = false;

  for (char ch : plain) {
    unsigned char c = static_cast<unsigned char>(ch);
    if (std::isalnum(c)) {
      if (pending_dash && !out.empty())
        out.push_back('-');
      out.push_back(static_cast<char>(std::tolower(c)));
      pending_dash = false;
    } else
      pending_dash = true;
  }

  return out;
}

std::string extra_misc_human_number(double value) {
  static const char *suffixes[] = {"", "K", "M", "B", "T", "P", "E"};
  double scaled = std::fabs(value);
  int suffix = 0;
  while (scaled >= 1000.0 && suffix < 6) {
    scaled /= 1000.0;
    suffix++;
  }
  if (value < 0)
    scaled = -scaled;

  char buffer[64];
  snprintf(buffer, sizeof(buffer), suffix == 0 ? "%.0f%s" : "%.2f%s", scaled,
           suffixes[suffix]);
  std::string out(buffer);
  if (suffix > 0) {
    size_t dot = out.find('.');
    if (dot != std::string::npos) {
      size_t suffix_pos = out.find_first_not_of("-0123456789.", dot);
      size_t end = suffix_pos == std::string::npos ? out.size() : suffix_pos;
      while (end > dot && out[end - 1] == '0')
        out.erase(--end, 1);
      if (end > dot && out[end - 1] == '.')
        out.erase(--end, 1);
    }
  }
  return out;
}

static void skip_spaces(const char *str, size_t length, size_t *pos) {
  while (*pos < length && std::isspace(static_cast<unsigned char>(str[*pos])))
    (*pos)++;
}

static bool parse_number(const char *str, size_t length, size_t *pos,
                         double *value) {
  char *end = nullptr;
  const char *start = str + *pos;
  *value = strtod(start, &end);
  if (end == start)
    return false;
  *pos = static_cast<size_t>(end - str);
  return true;
}

bool extra_misc_parse_duration(const char *str, size_t length,
                               double *seconds) {
  size_t pos = 0;
  double total = 0.0;
  bool parsed_any = false;

  skip_spaces(str, length, &pos);
  if (pos < length && (str[pos] == 'P' || str[pos] == 'p')) {
    pos++;
    bool time_part = false;
    while (pos < length) {
      skip_spaces(str, length, &pos);
      if (pos < length && (str[pos] == 'T' || str[pos] == 't')) {
        time_part = true;
        pos++;
        continue;
      }

      double value = 0.0;
      if (!parse_number(str, length, &pos, &value) || pos >= length)
        return false;

      char unit = static_cast<char>(
          std::tolower(static_cast<unsigned char>(str[pos++])));
      if (unit == 'd')
        total += value * 86400.0;
      else if (unit == 'h')
        total += value * 3600.0;
      else if (unit == 'm')
        total += value * (time_part ? 60.0 : 2592000.0);
      else if (unit == 's')
        total += value;
      else
        return false;
      parsed_any = true;
    }
    *seconds = total;
    return parsed_any;
  }

  size_t first_colon = std::string(str, length).find(':');
  if (first_colon != std::string::npos) {
    int h = 0, m = 0;
    double s = 0.0;
    if (sscanf(std::string(str, length).c_str(), "%d:%d:%lf", &h, &m, &s) ==
        3) {
      *seconds = h * 3600.0 + m * 60.0 + s;
      return true;
    }
    return false;
  }

  while (pos < length) {
    skip_spaces(str, length, &pos);
    if (pos >= length)
      break;

    double value = 0.0;
    if (!parse_number(str, length, &pos, &value))
      return false;
    skip_spaces(str, length, &pos);

    size_t unit_start = pos;
    while (pos < length && std::isalpha(static_cast<unsigned char>(str[pos])))
      pos++;
    if (unit_start == pos)
      return false;

    std::string unit(str + unit_start, pos - unit_start);
    std::transform(unit.begin(), unit.end(), unit.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (unit == "s" || unit == "sec" || unit == "second" || unit == "seconds")
      total += value;
    else if (unit == "m" || unit == "min" || unit == "minute" ||
             unit == "minutes")
      total += value * 60.0;
    else if (unit == "h" || unit == "hr" || unit == "hour" || unit == "hours")
      total += value * 3600.0;
    else if (unit == "d" || unit == "day" || unit == "days")
      total += value * 86400.0;
    else if (unit == "w" || unit == "week" || unit == "weeks")
      total += value * 604800.0;
    else
      return false;

    parsed_any = true;
  }

  *seconds = total;
  return parsed_any;
}

static time_t make_utc_time(struct tm *value) {
#if defined(HAVE_TIMEGM)
  return timegm(value);
#else
  char *old_tz = getenv("TZ");
  std::string saved_tz = old_tz ? old_tz : "";
  setenv("TZ", "UTC", 1);
  tzset();
  time_t result = mktime(value);
  if (old_tz)
    setenv("TZ", saved_tz.c_str(), 1);
  else
    unsetenv("TZ");
  tzset();
  return result;
#endif
}

static bool add_calendar(struct tm base, int years, int months,
                         struct tm *out) {
  base.tm_year += years;
  base.tm_mon += months;
  time_t converted = make_utc_time(&base);
  if (converted == static_cast<time_t>(-1))
    return false;
  gmtime_r(&converted, out);
  return true;
}

std::string extra_misc_age(time_t from, time_t to) {
  if (to < from)
    std::swap(from, to);

  struct tm from_tm;
  struct tm to_tm;
  gmtime_r(&from, &from_tm);
  gmtime_r(&to, &to_tm);

  int years = to_tm.tm_year - from_tm.tm_year;
  struct tm candidate;
  add_calendar(from_tm, years, 0, &candidate);
  if (make_utc_time(&candidate) > to)
    years--;

  int months = 0;
  while (months < 11) {
    struct tm next;
    add_calendar(from_tm, years, months + 1, &next);
    if (make_utc_time(&next) > to)
      break;
    months++;
  }

  add_calendar(from_tm, years, months, &candidate);
  time_t anchor = make_utc_time(&candidate);
  long long rest = static_cast<long long>(difftime(to, anchor));
  long long days = rest / 86400;
  rest %= 86400;
  long long hours = rest / 3600;
  rest %= 3600;
  long long minutes = rest / 60;
  long long seconds = rest % 60;

  char buffer[128];
  snprintf(buffer, sizeof(buffer), "%dy %dmo %lldd %lldh %lldm %llds", years,
           months, days, hours, minutes, seconds);
  return std::string(buffer);
}
