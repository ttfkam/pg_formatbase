#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

#define MAP_END 122

static inline void
validate_base(int32 base) {
  /* Base-0 & base-1 are non-sensical. Nothing above base-64 supported. */
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

/* Sizes w/ trailing NULL & negative */
/* Base-0 and base-1 are non-sensical. Dump out if used. */
static const int BUFFER_SIZES[] = {
  -1,-1,65,42,34,30,27,25,23,22,21,21,20,20,19,19,    /*  0-15 */
  18,18,18,17,17,17,17,16,16,16,16,16,16,15,15,15,    /* 16-31 */
  15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,    /* 32-47 */
  14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13  /* 48-64 */
};

/* Map text characters to numeric values */
/* Note: the repetition of letter mappings allows case-insensitivity */
static const int8 LOWER_MAP[] = {
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
  -1,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
  25,26,27,28,29,30,31,32,33,34,35,-1,-1,-1,-1,-1,
  -1,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
  25,26,27,28,29,30,31,32,33,34,35
};

static const int8 UPPER_MAP[] = {
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
  -1,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
  51,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,62,
  63,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
  25,26,27,28,29,30,31,32,33,34,35
};

PG_FUNCTION_INFO_V1(to_base);

Datum
to_base(PG_FUNCTION_ARGS)
{
  static const char *MAP =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_`";

  int64 val = PG_GETARG_INT64(0);
  int32 base = PG_GETARG_INT32(1);
  bool is_negative = val < 0;
  int size = BUFFER_SIZES[base];
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

  /* Set up the output buffer */
  buffer = palloc(sizeof(char) * size);
  if (buffer == NULL) {  /* out of memory */
    elog(ERROR, "Out of memory");
  }
  buffer += (size - 1);
  *buffer = '\0';

  /* Write out the encoded number */
  if (is_negative) {
      --buffer;
      *buffer = MAP[-(val % base)];
      val /= -base;
  }
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

PG_FUNCTION_INFO_V1(from_base);

Datum
from_base(PG_FUNCTION_ARGS)
{
  text *src = PG_GETARG_TEXT_P(0);
  int32 base = PG_GETARG_INT32(1);
  int len = VARSIZE(src) - VARHDRSZ;
  const int8 *map = base > 36 ? UPPER_MAP : LOWER_MAP;
  unsigned char *val = NULL;
  int64 result = 0;
  bool is_negative = FALSE;
  unsigned char next;  /* saves an extra comparison to negative numbers */

  validate_base(base);

  if (len == 0) {
    ereport(ERROR,
      (
        errcode(ERRCODE_INVALID_PARAMETER_VALUE),
        errmsg("empty text value not allowed")
      )
    );
  }
  val = palloc(len + 1);
  memcpy(val, VARDATA(src), len);
  val[len] = '\0';
  if (len > BUFFER_SIZES[base]) {
    ereport(ERROR,
      (
        errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
        errmsg("input value too large to fit in bigint"),
        errdetail("value '%s' cannot be encoded", val)
      )
    );
  }
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
  while (next = *val - 48) {
    if (next > MAP_END || map[next] >= base || map[next] < 0) {
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
    result += map[next];
    ++val;
  }
  if (is_negative) {
    result *= -1;
  }
  PG_RETURN_INT64(result);
}
