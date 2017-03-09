#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(to_base);

Datum
to_base(PG_FUNCTION_ARGS)
{
  static const char *MAP = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";
  /* Sizes w/ trailing NULL & negative */
  /* Base-0 and base-1 are non-sensical. Dump out if used. */
  static const char BUF_SIZES[] = {
  /* 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23*/
    -1,-1,65,42,34,30,27,25,23,22,21,21,20,20,19,19,18,18,18,17,17,17,17,16,
  /*24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47*/
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,
  /*48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64*/
    14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13
  };

  int32 base = PG_GETARG_INT32(0);
  int64 val = PG_GETARG_INT64(1);
  bool is_negative = val < 0;
  int buffer_size;
  char *buffer;

  /* Base-0 & base-1 are non-sensical. Nothing about base-64 supported. */
  if (base < 2 || base > 64) {
    elog(ERROR, "Output base out of range. Must be between 2 and 64.");
  }

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
      elog(ERROR, "Negative input value too large");
    }
    val = -val;
  }

  /* Set up the output buffer */
  buffer_size = BUF_SIZES[base];
  buffer = palloc(sizeof(char) * buffer_size);
  if (buffer == NULL) {  /* out of memory */
      elog(ERROR, "Out of memory");
  }
  buffer += (buffer_size - 1);
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
