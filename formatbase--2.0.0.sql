-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION formatbase" to load this file. \quit

CREATE FUNCTION text(int8, int4)
RETURNS text
AS '$libdir/formatbase', 'to_base_text'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION text(val int4, base int4)
RETURNS text
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT text(base,val::int8);
$$;

CREATE FUNCTION int8(text, int4)
RETURNS int8
AS '$libdir/formatbase', 'from_base_int8'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION int4(val text, base int4)
RETURNS text
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT int8(val, base)::int4;
$$;

CREATE FUNCTION int2(val text, base int4)
RETURNS text
LANGUAGE sql IMMUTABLE STRICT AS $$
  SELECT int8(val, base)::int2;
$$;
