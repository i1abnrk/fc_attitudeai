/* utility */
#include "shared.h"
#include "support.h"

/* common */
#include "fc_types.h"
#include "events.h"

/* server/advisor */
#include "advdata.h"

#define MAX_NUM_AI_VARIANTS MAX_NUM_ITEMS
/* Leave a mandatory one percent chance */
#define ATTITUDE_FAVOR_MIN -99
#define ATTITUDE_FAVOR_DEFAULT 0
#define ATTITUDE_FAVOR_MAX 99
#define ATTITUDE_REASON_MIN_VALUE -99
#define ATTITUDE_REASON_DEFAULT_VALUE 0
#define ATTITUDE_REASON_MAX_VALUE 99
#define ATTITUDE_HALFLIFE_MIN_TURNS 1
#define ATTITUDE_HALFLIFE_MAX_TURNS 200
#define ATTITUDE_HALFLIFE_DEF_TURNS ATTITUDE_HALFLIFE_MAX_TURNS
#define AAI_CLIP(dai, aai) ((aai>=0) ? CLIP(aai, dai, 100) \
                                     : CLIP(0, dai, 100-aai))

struct ai_type;

/*>fc_types.h*/
typedef int ai_variant_id;

/*Favorite structured to mirror universals_n in common/fc_types.h
 *Favorite aims to take a role at how aggressively certain "goals" are pursued
 *by the leader ai.*/
struct favorite {
  /* what is it? */
  struct universal type;
  /* how much? */
  int value;
};

#define SPECLIST_TAG favorite
#define SPECLIST_TYPE struct favorite
#include "speclist.h"
#define favorite_list_iterate(favorites, pfavor)         \
  TYPED_LIST_ITERATE(struct favorite, favorites, pfavor)
#define favorite_list_iterate_end LIST_ITERATE_END
/*Reasons to make temporary adjustments to advdata of player ai
 *TODO: Test these incrementally until universal_u|favorite|score_component can reasonably
 *substitute for reason_type in the functions created.*/
/*enum reason_type {
  common/fc_types.h
  REASON_FOOD = 0,  when food is the matter
  REASON_TRADE,  when trade is the focus
  REASON_POLLUTION,  when pollution is an issue
  REASON_EXPANSION,  when expansion is abrupt
  REASON_TREATY,  when a treaty is adjustible
  REASON_POPULATION,  when population is the focus
    when a threat is made by a {treaty_state} or immediate player action,
   * (if discovered subj. to ai_level)
   server/advisors/advdata.h
  REASON_THREAT,
  REASON_EXPLORE,  when the need to explore is hampered
  REASON_SCIENCE,  when science resource is needed
  REASON_GOLD,  when the economy needs focus
  REASON_DIPLOMACY,  when diplomacy hampers other needs
  REASON_TAX,
  REASON_LUXURY,
  REASON_WONDER,  when a wonder is worshipped/required/obtained
  REASON_SPACESHIP,  when a spaceship is desired/jealous
  REASON_FAVORITE,  when a favorite *thing is coveted (advdata.goals)
  REASON_CELEBRATE,
  REASON_CRAZY,  when a leader is crazy, increase the fuzzy. (just for fun!)
  REASON_LAST  stop iterating reasons
};*/
#define SPECENUM_NAME reason_type
#define SPECENUM_VALUE0 REASON_FOOD
#define SPECENUM_VALUE1 REASON_SHIELD
#define SPECENUM_VALUE2 REASON_GOLD
#define SPECENUM_VALUE3 REASON_LUXURY
#define SPECENUM_VALUE4 REASON_SCIENCE
#define SPECENUM_VALUE5 REASON_TAX
#define SPECENUM_VALUE6 REASON_WONDER
#define SPECENUM_VALUE7 REASON_EXPLORE
#define SPECENUM_VALUE8 REASON_EXPAND
#define SPECENUM_VALUE9 REASON_POPULATION
#define SPECENUM_VALUE10 REASON_DIPLOMACY
#define SPECENUM_VALUE11 REASON_TREATY
#define SPECENUM_VALUE12 REASON_THREAT
#define SPECENUM_VALUE13 REASON_SPACESHIP
#define SPECENUM_VALUE14 REASON_CELEBRATE
#define SPECENUM_VALUE15 REASON_FAVORITE
#define SPECENUM_VALUE16 REASON_CRAZY
#define SPECENUM_COUNT REASON_COUNT
#include "specenum_gen.h"

struct reason {
  enum reason_type rtype;
  int value;
  int halflife;
};
/* Nation leader ai data. */
#define SPECLIST_TAG reason
#define SPECLIST_TYPE struct reason
#include "speclist.h"
#define reason_list_iterate(reasons, preason)                     \
  TYPED_LIST_ITERATE(struct reason, reasons, preason)
#define reason_list_iterate_end LIST_ITERATE_END


/*adjustment value calculated relative to the formula 
 adjust * (curr_turn-first_turn) / halflife * ln(2) 
 *TODO: player_memory to savegame. Make subdirectory "fcai" in .freeciv'
 *for player memory data to prevent corruption of savegames loaded from standard
 *freeciv.*/
struct leader_memory {
  int leader; /*my player_slot_id*/
  int nation; /*other nation*/
  int first_turn; /*first turn of incident*/
  struct reason reason;
   /*TRUE iff "I like it", adjust == ((sympathy?1:-1) * base_adjust) */
  bool sympathy;
  bool helped;
};
/* Nation leader ai data. */
#define SPECLIST_TAG leader_memory
#define SPECLIST_TYPE struct leader_memory
#include "speclist.h"
#define leader_memory_list_iterate(lmlist, pleader)                     \
  TYPED_LIST_ITERATE(struct leader_memory, lmlist, pleader)
#define leader_memory_list_iterate_end LIST_ITERATE_END

struct ai_variant {
  ai_variant_id id;

  const char *name;
  
  struct reason_list *reasons;
    
  struct favorite_list *favorites;
  
  /* TODO: save leader memories in a separate file from savegame. */
  struct leader_memory_list *memory;
};

#define SPECLIST_TAG ai_variant
#define SPECLIST_TYPE struct ai_variant
#include "speclist.h"
#define ai_variant_list_iterate(aivarilist, paivari)  \
  TYPED_LIST_ITERATE(struct ai_variant, aivarilist, paivari)
#define ai_variant_list_iterate_end LIST_ITERATE_END

const char *ai_variant_name(struct ai_variant *paivari);
struct ai_variant_list *ai_variants(void);
struct ai_variant *ai_variant_by_number(ai_variant_id id);
bool aiv_initialized(void);
void ai_variants_init(void);
void ai_variants_free(void);
void reason_new(struct ai_variant *paivari, enum reason_type rt, int value, int halflife);
void reason_destroy(struct reason *preason);
void favorite_new(struct ai_variant *paivari, struct universal type, int value);
void favorite_destroy(struct favorite *pfavor);
struct ai_variant *ai_variant_new(const char *name);
bool rules_have_leader(const char *name);
void ai_variant_destroy(struct ai_variant *paivari);
/* These foo_amend functions maintains uniqueness. foo_amend is like
 * pfoo = foo_get_index(i); 
 * foo_remove(pfoo); 
 * foo_mutator(pfoo, bar); 
 * foo_put_index(pfoo, i)*/
bool ai_variant_reason_amend(struct ai_variant *paivari, struct reason *preason);
bool ai_variant_reason_remove(struct ai_variant *paivari, struct reason *preason);
bool ai_variant_favorite_amend(struct ai_variant *paivari, struct favorite *pfavor);
bool ai_variant_favorite_remove(struct ai_variant *paivari, struct favorite *pfavor);
