# mariadb-plugin-extra-misc

![mariabd-plugin-extra-misc](logo/extra_misc_func.png)

`extra_misc` is a MariaDB plugin that adds small utility SQL functions:

- `unaccent(str)`
- `slugify(str)`
- `age(date_or_datetime)`
- `human_number(number)`
- `parse_duration(str)`

```sql
SELECT plugin_name, plugin_type, plugin_library,    
       plugin_description,        plugin_author 
  FROM information_schema.PLUGINS 
 WHERE plugin_type = 'FUNCTION'   AND plugin_library = 'extra_misc.so' 
 ORDER BY plugin_name;
```

Result:

```text
+----------------+-------------+----------------+---------------------------+---------------+
| plugin_name    | plugin_type | plugin_library | plugin_description        | plugin_author |
+----------------+-------------+----------------+---------------------------+---------------+
| age            | FUNCTION    | extra_misc.so  | Function AGE()            | lefred        |
| human_number   | FUNCTION    | extra_misc.so  | Function HUMAN_NUMBER()   | lefred        |
| parse_duration | FUNCTION    | extra_misc.so  | Function PARSE_DURATION() | lefred        |
| slugify        | FUNCTION    | extra_misc.so  | Function SLUGIFY()        | lefred        |
| unaccent       | FUNCTION    | extra_misc.so  | Function UNACCENT()       | lefred        |
+----------------+-------------+----------------+---------------------------+---------------+
```

## Build

Link or copy this directory into the MariaDB `plugin` directory as
`extra_misc`, then configure MariaDB with the plugin enabled:

```bash
cmake -S /path/to/MariaDB-server \
  -B /path/to/build \
  -DPLUGIN_EXTRA_MISC=DYNAMIC

cmake --build /path/to/build --target extra_misc
```

## Install

```sql
INSTALL SONAME 'extra_misc';
```

## Functions

### `unaccent(str)`

Returns `str` with common Latin accents removed.

```sql
SELECT unaccent('Crème brûlée déjà vu');
-- Creme brulee deja vu
```

### `slugify(str)`

Removes accents, lowercases ASCII letters, replaces separator runs with `-`,
and trims leading/trailing separators.

```sql
SELECT slugify('Crème brûlée déjà vu!');
-- creme-brulee-deja-vu
```

### `age(date_or_datetime)`

Returns the calendar-aware elapsed duration between `date_or_datetime` and the
current statement timestamp.

```sql
SELECT age('1976-03-14 06:50:00');
-- 50y 3mo 4d 8h 36m 46s
```

### `human_number(number)`

Formats large numbers using SI-style suffixes.

```sql
SELECT human_number(1234567);
-- 1.23M
```

### `parse_duration(str)`

Parses a human duration and returns seconds. Supported forms include unit
groups, `HH:MM:SS`, and a simple ISO-8601 duration subset.

```sql
SELECT parse_duration('1h 30m 10.5s');
-- 5410.5

SELECT parse_duration('01:02:03');
-- 3723

SELECT parse_duration('P1DT2H');
-- 93600
```

Supported unit names are seconds, minutes, hours, days, and weeks. ISO month
values are treated as 30 days.

## Test

```bash
cd /path/to/build/mysql-test
./mtr extra_misc
```
