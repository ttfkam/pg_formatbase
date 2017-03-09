-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION formatbase" to load this file. \quit

CREATE FUNCTION to_base(int4, int8)
RETURNS text
AS '$libdir/formatbase'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION to_base(base int4, val int4)
RETURNS text
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT to_base(base,val::int8);
$$;
