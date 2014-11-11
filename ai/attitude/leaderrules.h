/*parses the leader.ruleset file. Regarding that Attitude AI is a dynamically
* loadable static module, all ruleset infos need to be loaded in modular fashion.
* When the module is not desired it will not be loaded.
* leaderrules.c|h is the component which loads the list of ai_variant objects 
* from aivariants.ruleset. It works with aivariant.c|h which contains all the
* constructors, destructors and member accessors for the module. In as much as
* possible, I try to mimic the freeciv code style to make it easy for others to
* implement it with their own freeciv based projects. */
/* utility */
#include "iterator.h"
#include "support.h"
#include "registry.h"

/* common */
#include "requirements.h"

/* server */
#include "ruleset.h"

/* local */
#include "aivariant.h"

#define LOOKUP_INT_REASON_VAL(file, format, sec, rstr, row)  \
FC_PTR_TO_INT(\
secfile_lookup_int_def_min_max(file, \
ATTITUDE_REASON_DEFAULT_VALUE, ATTITUDE_REASON_MIN_VALUE, ATTITUDE_REASON_MAX_VALUE, \
format, sec, rstr, row))

#define LOOKUP_INT_REASON_HL(file, format, sec, rstr, row)  \
FC_PTR_TO_INT(\
secfile_lookup_int_def_min_max(file, \
ATTITUDE_HALFLIFE_DEF_TURNS, ATTITUDE_HALFLIFE_MIN_TURNS, ATTITUDE_HALFLIFE_MAX_TURNS, \
format, sec, rstr, row))

#define LOOKUP_INT_FAVORITE_VAL(file, format, sec, rstr, row)  \
FC_PTR_TO_INT(\
secfile_lookup_int_def_min_max(file, \
ATTITUDE_FAVOR_DEFAULT, ATTITUDE_FAVOR_MIN, ATTITUDE_FAVOR_MAX, \
format, sec, rstr, row))

enum reason_type reason_type_by_rule(const char *type);
struct favorite favorite_from_str(const char *type, const char *range,
				bool survives, bool negated,
				const char *value);
void load_reasons(struct section_file *file, 
  struct ai_variant *paivari, const struct section *psection);
void load_favorites(struct section_file *file, struct ai_variant *paivari, 
    const struct section *psection);
void load_ai_variant_rulesets(bool act);
