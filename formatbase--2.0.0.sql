-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION formatbase" to load this file. \quit

CREATE FUNCTION text(int8, int4)
RETURNS text
AS '$libdir/formatbase', 'to_base'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION text(num int4, base int4)
RETURNS text
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT text(num::int8, base);
$$;

CREATE FUNCTION int8(val text, base int4)
RETURNS int8
AS '$libdir/formatbase', 'from_base'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION int4(val text, base int4)
RETURNS int4
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT int8(val, base)::int4;
$$;

CREATE FUNCTION int2(val text, base int4)
RETURNS int2
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT int8(val, base)::int2;
$$;
