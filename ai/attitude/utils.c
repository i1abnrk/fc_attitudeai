#include "utils.h"

const char *strtoupper(const char *src) {
  int i=0, l = strlen(src);
  const char *res;
  fc_strlcat(res, fc_toupper(FC_PTR_TO_INT(strchr(src, i))), l);
  return res; 
}
