#include <math.h>
#include <string.h>

/*gen_headers*/
#include "freeciv_config.h"

/*utility*/
#include "support.h"

/*common*/
#include "requirements.h"

#include "utils.h"

const char *strtoupper(char *src) {
  int i=0, l = strlen(src);
  char stmp;
  /* dtmp = to_upper(stmp) */
  char utmp;
  char dtmp[l];
  
  for(; i < l; i++) {
    stmp = src[i];
    utmp = fc_toupper(stmp);
    dtmp[i] = utmp;
    dtmp[i + 1] = '\0';
  }
  
  const char *dest = dtmp;
  
  /*give our local pointer back to heap*/
  free(dtmp);
  
  return dest;
}

int calc_halflife(int value, int halflife, int turns) {
  int result;
  double d_result;
  double factor;
  
  factor = pow(0.5, ((double) turns / halflife));
  d_result = factor * value;
  result = (int) floor(d_result);
  return result;
}

int universalcmp(const struct universal *u1, const struct universal *u2) {
  int kindcmp;
  enum universals_n kind1 = u1->kind, kind2 = u2->kind;
  
  /* universals_n_name is comparable as a string, compare value of type.kind */
  kindcmp = strcmp(universals_n_name(kind1), universals_n_name(kind2));
  if (kindcmp != 0) {
    return kindcmp;
  }
  
  int un1 = universal_number(u1), un2 = universal_number(u2);
  
  if (un1==un2) {
    return 0;
  } else {
    if (un1 > un2) {
      return 1;
    }
    return -1;
  }
  
  return 0;
}


