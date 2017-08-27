CREATE EXTENSION formatbase;

--
-- number to formatted text
--

SELECT text(64356543, -1);  -- Should fail
SELECT text(64356543, 0);   -- Should fail
SELECT text(64356543, 1);   -- Should fail
SELECT text(64356543, base) FROM generate_series(2, 64) AS base;  -- Should succeed
SELECT text(64356543, 65);  -- Should fail

-- Test upper bounds
SELECT text('9223372036854775807'::int8, 36);
SELECT text('-9223372036854775807'::int8, 36);
SELECT text('-9223372036854775808'::int8, 36);

--
-- formatted text to number
--

-- Valid base 16
SELECT int8('123456789abcdef', 16);
SELECT int8('-123456789abcdef', 16);
SELECT int4('1234567f', 16);
SELECT int4('-1234567f', 16);
SELECT int2('123f', 16);
SELECT int2('-123f', 16);

-- Invalid for base 16
SELECT int8('123456789abcdefg', 16);
SELECT int8('123456789ABCDEFG', 16);

-- Base 36- should be case insensitive and give the same answer
SELECT int8('aaaaaaaa', base) = int8('AAAAAAAA', base),
       int8('-aaaaaaaa', base) = int8('-AAAAAAAA', base),
       int4('aaaaa', base) = int8('AAAAA', base),
       int4('-aaaaa', base) = int8('-AAAAA', base),
       int2('aa', base) = int8('AA', base),
       int2('-aa', base) = int8('-AA', base)
  FROM generate_series(11, 36) AS base;

-- Base 37+ should be case sensitive and yield different results
SELECT int8('aaaaaaaa', base) <> int8('AAAAAAAA', base),
       int8('-aaaaaaaa', base) <> int8('-AAAAAAAA', base),
       int4('aaaaa', base) <> int8('AAAAA', base),
       int4('-aaaaa', base) <> int8('-AAAAA', base),
       int2('aa', base) <> int8('AA', base),
       int2('-aa', base) <> int8('-AA', base)
  FROM generate_series(37, 64) AS base;

-- Parse all the bases
SELECT int8('10101010'),
       int8('-10101010'),
       int4('10101'),
       int4('-10101'),
       int2('10'),
       int2('-10')
  FROM generate_series(2, 64) AS base;

-- Invalid for any base
SELECT int8('-', 2);
SELECT int8('-', 8);
SELECT int8('-', 16);
SELECT int8('-', 36);
SELECT int8('-', 64);
