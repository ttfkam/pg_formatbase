CREATE EXTENSION formatbase;

--
-- number to formatted text
--

SELECT to_base(-1, 64356543);
SELECT to_base( 0, 64356543);
SELECT to_base( 1, 64356543);
SELECT to_base( 2, 64356543);
SELECT to_base( 3, 64356543);
SELECT to_base( 4, 64356543);
SELECT to_base( 5, 64356543);
SELECT to_base( 6, 64356543);
SELECT to_base( 7, 64356543);
SELECT to_base( 8, 64356543);
SELECT to_base( 9, 64356543);
SELECT to_base(10, 64356543);
SELECT to_base(11, 64356543);
SELECT to_base(12, 64356543);
SELECT to_base(13, 64356543);
SELECT to_base(14, 64356543);
SELECT to_base(15, 64356543);
SELECT to_base(16, 64356543);
SELECT to_base(17, 64356543);
SELECT to_base(18, 64356543);
SELECT to_base(19, 64356543);
SELECT to_base(20, 64356543);
SELECT to_base(21, 64356543);
SELECT to_base(22, 64356543);
SELECT to_base(23, 64356543);
SELECT to_base(24, 64356543);
SELECT to_base(25, 64356543);
SELECT to_base(26, 64356543);
SELECT to_base(27, 64356543);
SELECT to_base(28, 64356543);
SELECT to_base(29, 64356543);
SELECT to_base(30, 64356543);
SELECT to_base(31, 64356543);
SELECT to_base(32, 64356543);
SELECT to_base(33, 64356543);
SELECT to_base(34, 64356543);
SELECT to_base(35, 64356543);
SELECT to_base(36, 64356543);
SELECT to_base(37, 64356543);
SELECT to_base(38, 64356543);
SELECT to_base(39, 64356543);
SELECT to_base(40, 64356543);
SELECT to_base(41, 64356543);
SELECT to_base(42, 64356543);
SELECT to_base(43, 64356543);
SELECT to_base(44, 64356543);
SELECT to_base(45, 64356543);
SELECT to_base(46, 64356543);
SELECT to_base(47, 64356543);
SELECT to_base(48, 64356543);
SELECT to_base(49, 64356543);
SELECT to_base(50, 64356543);
SELECT to_base(51, 64356543);
SELECT to_base(52, 64356543);
SELECT to_base(53, 64356543);
SELECT to_base(54, 64356543);
SELECT to_base(55, 64356543);
SELECT to_base(56, 64356543);
SELECT to_base(57, 64356543);
SELECT to_base(58, 64356543);
SELECT to_base(59, 64356543);
SELECT to_base(60, 64356543);
SELECT to_base(61, 64356543);
SELECT to_base(62, 64356543);
SELECT to_base(63, 64356543);
SELECT to_base(64, 64356543);
SELECT to_base(65, 64356543);

-- Test upper values
SELECT to_base(36, '9223372036854775807'::int8);
SELECT to_base(36, '-9223372036854775807'::int8);
SELECT to_base(36, '-9223372036854775808'::int8);

--
-- formatted text to number
--

-- Invalid for base 16
SELECT parse_base(16, '123456789abcdefg');
SELECT parse_base(16, '123456789ABCDEFG');

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
