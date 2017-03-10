#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

static void validate_base(int32 base) {
  /* Base-0 & base-1 are non-sensical. Nothing about base-64 supported. */
  if (base < 2 || base > 64) {
		ereport(ERROR,
			(
				errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				errmsg("output base out of range"),
				errdetail("base %d is not allowed", base),
				errhint("base must be between 2 and 64")
			)
    );
  }
}

static int buffer_size(int32 base) {
  /* Sizes w/ trailing NULL & negative */
  /* Base-0 and base-1 are non-sensical. Dump out if used. */
  static const char BUF_SIZES[] = {
  /* 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21*/
    -1,-1,65,42,34,30,27,25,23,22,21,21,20,20,19,19,18,18,18,17,17,17,
  /*22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43*/
    17,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,
  /*44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64*/
    14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13
  };

  return BUF_SIZES[base];
}

static const char *lookup_map(int32 base) {
  /* Map text characters to numeric values */
  /* Note: the repetition of letter mappings allows case-insensitivity */
  static const char LOWER_MAP[] = {
  /* 0 1 2 3 4 5 6 7 8 9  :  ;  <  =  >  ?  @  A  B  C  D  E  F  G  H  I*/
     0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,-1,10,11,12,13,14,15,16,17,18,
  /* J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `*/
    19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,-1,-1,-1,-1,-1,-1,
  /* a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w*/
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
  /* x  y  z*/
    33,34,35
  };

  static const char UPPER_MAP[] = {
  /* 0 1 2 3 4 5 6 7 8 9  :  ;  <  =  >  ?  @  A  B  C  D  E  F  G  H  I*/
     0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,-1,36,37,38,39,40,41,42,43,44,
  /* J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `*/
    45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,62,63,
  /* a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w*/
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
  /* x  y  z*/
    33,34,35
  };

  /* Allow case-insensitivity when using base36 or lower */
  return base > 36 ? UPPER_MAP : LOWER_MAP;
}

PG_FUNCTION_INFO_V1(to_base);

Datum
to_base(PG_FUNCTION_ARGS)
{
  static const char *MAP =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_`";

  int32 base = PG_GETARG_INT32(0);
  int64 val = PG_GETARG_INT64(1);
  bool is_negative = val < 0;
  int size = buffer_size(base);
  char *buffer;

  validate_base(base);

  /* Fast path for common values (and are the same in every base) */
  switch (val) {
    case -1:
      PG_RETURN_TEXT_P(cstring_to_text("-1"));
    case 0:
      PG_RETURN_TEXT_P(cstring_to_text("0"));
    case 1:
      PG_RETURN_TEXT_P(cstring_to_text("1"));
    default:
      /* Fall through to continue processing */
      break;
  }

  /* Flip negatives for numeric operations below */
  if (is_negative) {
    /* greater than 64-bit sign flip value (one more negative than positive) */
    if (val == 0x8000000000000000LL) {
      /* Avoid overflow by simply punting */
			ereport(ERROR,
				(
					errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					errmsg("negative input value too large"),
					errdetail("value '-9223372036854775808' cannot be encoded")
				)
      );
    }
    val = -val;
  }

  /* Set up the output buffer */
  buffer = palloc(sizeof(char) * size);
  if (buffer == NULL) {  /* out of memory */
    elog(ERROR, "Out of memory");
  }
  buffer += (size - 1);
  *buffer = '\0';

  /* Write out the encoded number */
  while (val > 0 && --buffer) {
    *buffer = MAP[val % base];
    val /= base;
  }
  if (is_negative) {
    --buffer;
    *buffer = '-';
  }

  PG_RETURN_TEXT_P(cstring_to_text(buffer));
}

PG_FUNCTION_INFO_V1(parse_base);

Datum
parse_base(PG_FUNCTION_ARGS)
{
  static const int START_OFFSET = 48;
  static const int END_OFFSET = 122;

  int32 base = PG_GETARG_INT32(0);
  text *src = PG_GETARG_TEXT_P(1);
  int len = VARSIZE(src) - VARHDRSZ;
  const char *map = lookup_map(base);
  char *val;
  int64 result = 0;
  bool is_negative = FALSE;
  char next;
  int jumpchar;

  validate_base(base);

  if (len == 0) {
		ereport(ERROR,
			(
				errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("empty text value not allowed")
			)
		);
  }
  if (len > buffer_size(base)) {
		ereport(ERROR,
			(
				errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				errmsg("input value too large to fit in bigint"),
				errdetail("value '%s' cannot be encoded", val)
			)
		);
  }
  val = palloc(len + 1);
  memcpy(val, VARDATA(src), len);
  val[len] = '\0';
  if (*val == '-') {
    if (len > 1) {
      is_negative = TRUE;
      ++val;
    } else {
      ereport(ERROR,
        (
          errcode(ERRCODE_INVALID_PARAMETER_VALUE),
          errmsg("'-' is not a valid number")
        )
      );
    }
  }
  while ((next = *val)) {
    jumpchar = next - START_OFFSET;
    if (jumpchar < 0 || next > END_OFFSET ||
        map[jumpchar] > base || map[jumpchar] < 0) {
      ereport(ERROR,
        (
          errcode(ERRCODE_INVALID_PARAMETER_VALUE),
          errmsg("invalid character found in encoded value"),
          errdetail("digit '%c' is not allowed", next),
          errhint("use only digits found in base %d", base)
        )
      );
    }
    result *= base;
    result += map[jumpchar];
    ++val;
  }
  if (is_negative) {
    result *= -1;
  }
  PG_RETURN_INT64(result);
}
