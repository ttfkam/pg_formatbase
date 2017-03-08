-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION formatbase" to load this file. \quit

CREATE FUNCTION to_base(val bigint, base integer)
RETURNS text
LANGUAGE sql IMMUTABLE STRICT AS $$
  WITH cte AS (
    SELECT regexp_split_to_array(
      '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-',
      '') AS idx
  )
  SELECT regexp_replace(string_agg(idx[((val / pow(base, g))::int % base) + 1], ''), '^0+', '')
  FROM generate_series(63, 0, -1) AS g, cte
  WHERE base <= 64
$$;

CREATE FUNCTION as_base(bigint, integer)
RETURNS text
AS '$libdir/formatbase'
LANGUAGE C IMMUTABLE STRICT;
