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
  
  const char *dest;
  
  memcpy(dtmp, dest, l);
  
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
  kindcmp = strcmp(universals_n_name(kind1), universals_n_name(kind2));
  if (kindcmp != 0) {
    return kindcmp;
  }
  
  bool kindcmp = fc_strcasecmp(kind1, kind2);
  
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
  if(NULL!=FC_MEMBER(typeof(value1), "id") && FC_MEMBER(typeof(value2), "id")) {
    return (value1->id==value2->id)?0:((value1->id > value2->id)?1:-1);
  } else {
    return (value1==value2)?0:((value1>value2)?1:-1);
  }
  
  return 0;
}


