CREATE EXTENSION formatbase;

--
-- number to formatted text
--

SELECT to_base(-1, 64356543);
SELECT to_base( 0, 64356543);
SELECT to_base( 1, 64356543);
SELECT to_base(base, 64356543) FROM generate_series(2, 64) AS base;
SELECT to_base(65, 64356543);

-- Test upper values
SELECT to_base(36, '9223372036854775807'::int8);
SELECT to_base(36, '-9223372036854775807'::int8);
SELECT to_base(36, '-9223372036854775808'::int8);

--
-- formatted text to number
--

-- Valid base 16
SELECT parse_base(16, '123456789abcdef');
SELECT parse_base(16, '-123456789abcdef');
SELECT parse_base(16, '123456789ABCDEF');
SELECT parse_base(16, '-123456789ABCDEF');

-- Base 36 should be case insensitive and give the same answer
SELECT parse_base(36, 'deadbeef');
SELECT parse_base(36, 'DEADBEEF');
SELECT parse_base(36, '-deadbeef');
SELECT parse_base(36, '-DEADBEEF');

-- Base >36 should be case sensitive and yield different results
SELECT parse_base(48, 'deadbeef');
SELECT parse_base(48, 'DEADBEEF');
SELECT parse_base(48, '-deadbeef');
SELECT parse_base(48, '-DEADBEEF');

-- Parse binary
SELECT parse_base( 2, '101010101010101');
SELECT parse_base( 2, '-101010101010101');

-- Invalid for base 16
SELECT parse_base(16, '123456789abcdefg');
SELECT parse_base(16, '123456789ABCDEFG');

-- Invalid for any base
SELECT parse_base( 2, '-');
SELECT parse_base( 8, '-');
SELECT parse_base(16, '-');
SELECT parse_base(36, '-');
SELECT parse_base(64, '-');
