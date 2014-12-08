#include <math.h>
#include <string.h>

/*utility*/
#include "support.h"

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
  
  //strcpy(dest, dtmp);
  
  /*give our local pointer back to heap*/
  free(dtmp);
  
  return dest;
}

int calc_halflife(int value, int halflife, int turns) {
  int result;
  double d_result;
  double factor;
  
  factor = 1.0 / pow(2.0, ((double) turns / halflife));
  d_result = factor * value;
  result = (int) floor(d_result);
  return result;
}

int universalcmp(struct universal u1, struct universal u2) {
  int kindcmp;
  enum universals_n kind1 = u1.kind, kind2 = u2.kind;
  universals_u value1 = u1.value, value2 = u2.value;
  
  /* universals_n_name is comparable as a string, compare value of type.kind */
  kindcmp = strcmp(universals_n_name(kind1), universals_n_name(kind2));
  if (kindcmp != 0) {
    return kindcmp;
  }
  
  /*if the kind is the same compare the values. 
   *values type can be int, enum int or struct
   *so compare size. Should be always the same size here, but check.*/
  int sz1 = sizeof(value1);
  int sz2 = sizeof(value2);
  
  if(sz1 > sz2) {
    return 1;
  }
  
  if(sz2 > sz1) {
    return -1;
  }
  /*same kind and size*/
  
  /*the structs all have an inttype id field*/
  if (sizeof(typeof(value1))!=sizeof(int)) {
  
  }
  
  return 0;
}


