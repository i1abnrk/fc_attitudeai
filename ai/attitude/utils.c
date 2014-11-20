#include "utils.h"

const char *strtoupper(const char *src) {
  int i=0, l = strlen(src);
  const char *res;
  fc_strlcat(res, fc_toupper(FC_PTR_TO_INT(strchr(src, i))), l);
  return res; 
}

int calc_halflife(int value, int halflife, int turns) {
  int result;
  double d_result;
  double factor;
  
  factor = 1.0 / (2.0 ^ ((double) turns / halflife));
  d_result = factor * value;
  result = (int) math.floor(d_result);
  return result;
}
