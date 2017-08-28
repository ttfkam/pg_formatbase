# pg_formatbase
Format numbers and parse string values by base

## API

### Number to base text

The base can be any value between 2 (binary) and 64.

```sql
text(num bigint, base integer)
text(num integer, base integer)
text(num smallint, base integer)
```

### Text to number

The base can be any value between 2 (binary) and 64. Each function returns a type of the same name, e.g., int2('ff', 16) returns the int2 (aka smallint, 16-bit integer) value 255.

```sql
int8(value text, base integer)
int4(value text, base integer)
int2(value text, base integer)
```

## Examples

```sql
SELECT text(7, 2);          -- returns '111'::text
SELECT text(255, 16);       -- returns 'ff'::text
SELECT text(-32602, 36);    -- returns '-p5m'::text
SELECT int2('111', 2);      -- returns 7::int2
SELECT int4('ff', 16);      -- returns 255::int4
SELECT int8('-p5m', 36);    -- returns -32602::int8
SELECT int2(text(7, 2), 2); -- returns 7::int2
```
