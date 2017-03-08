#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(formatbase);

Datum
formatbase(PG_FUNCTION_ARGS)
{
  static const char *IDX = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";

  /* Sizes w/ trailing NULL & negative */
  static const char BUF_SIZES[65] = {
    -1,-1, /* Base-0 and base-1 are non-sensical. Dump out if we use them. */
    65, /* binary */
    42,34,30,27,25,
    23, /* octal */
    22,21,21,20,20,19,19,
    18, /* hex */
    18,18,17,17,17,17,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,
    14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13
  };

  int64 val = PG_GETARG_INT64(0);
  int32 base = PG_GETARG_INT32(1);
  int buffer_size;
  char *buffer;

  /* Base-0 & base-1 are non-sensical. Nothing about base-64 supported. */
  if (base < 2 || base > 64) {
    return NULL;
  }

  /* Fast path for common values (and are the same in every base) */
  switch (val) {
    case -1:
      return PG_RETURN_TEXT_P(cstring_to_text("-1"));
    case 0:
      return PG_RETURN_TEXT_P(cstring_to_text("0"));
    case 1:
      return PG_RETURN_TEXT_P(cstring_to_text("1"));
    default:
      /* Fall through to continue processing */
  }

  /* Set up the output buffer */
  buffer_size = BUF_SIZES[base];
  buffer = palloc(sizeof(char) * buffer_size);
  if (buffer == NULL) {  /* out of memory */
    return PG_RETURN_NULL();
  }
  buffer += (buffer_size - 1);
  *buffer = '\0';

  /* Handle negatives and edge cases */
  bool is_negative = val < 0;
  if (is_negative) {
    if (val < -9223372036854775807L) {  /* max 64-bit sign flip value */
      /* avoid overflow by simply punting */
      return PG_RETURN_NULL();
    }
    val = -val;
  }

  /* Write out the encoded number */
  while (val > 0 && --buffer) {
    *buffer = IDX[val % base];
    val /= base;
  }
  if (is_negative) {
    --buffer;
    *buffer = '-';
  }

  return PG_RETURN_TEXT_P(cstring_to_text(buffer));
}
