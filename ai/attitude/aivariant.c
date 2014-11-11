#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "nation.h"
#include "game.h"

#include "aivariant.h"

static struct ai_variant ai_variant_array[MAX_NUM_AI_VARIANTS];
static bool AIV_INITIALIZED=FALSE;

const char *ai_variant_name(struct ai_variant *paivari) {
  fc_assert_ret_val(NULL != paivari, NULL);
  return paivari->name;
}

struct ai_variant *ai_variant_by_number(ai_variant_id id) {
  if (id < 0 || id >= ARRAY_SIZE(ai_variant_array)) {
    return NULL;
  }
  return &ai_variant_array[id];
}

void ai_variants_init(void) {
  int i;
    /* Ensure we have enough space for players or teams. 
     *TODO: Need more ais in ruleset, for testing.*/
  //fc_assert(ARRAY_SIZE(ai_variant_array) >= team_slot_count());
  //fc_assert(ARRAY_SIZE(ai_variant_array) >= player_slot_count());
  memset(ai_variant_array, 0, sizeof(*ai_variant_array));
  for (i = 0; i < ARRAY_SIZE(ai_variant_array); i++) {
    ai_variant_array[i].id = (ai_variant_id) i;
    ai_variant_array[i].name = "";
    ai_variant_array[i].reasons = reason_list_new();
    ai_variant_array[i].favorites = favorite_list_new();
    ai_variant_array[i].memory = leader_memory_list_new();
  }
    /*TODO: array_pack(ai_variant_array, i)
   * Because I don't want to mess with game.h for a dll/la
   */
   AIV_INITIALIZED=TRUE;
}

bool aiv_initialized(void) {
  return AIV_INITIALIZED;
}

/*TODO: Undef favorite|reason_new|destroy 
 * and use favorite|reason_list_amend|remove instead */
/*Create a new reason for the leader data.*/
void reason_new(struct ai_variant *paivari, enum reason_type rt, int value, int halflife) {
  fc_assert_msg(value>=ATTITUDE_REASON_MIN_VALUE &&
            value<=ATTITUDE_REASON_MAX_VALUE, 
            "Invalid value %d for reason. Must be between %d and %d",
            value, ATTITUDE_REASON_MIN_VALUE, ATTITUDE_REASON_MAX_VALUE);
  fc_assert_msg(halflife>=ATTITUDE_HALFLIFE_MIN_TURNS &&
            halflife<=ATTITUDE_HALFLIFE_MAX_TURNS, 
            "Invalid halflife %d for reason. Must be between %d and %d",
            halflife, ATTITUDE_HALFLIFE_MIN_TURNS, ATTITUDE_HALFLIFE_MAX_TURNS);
            
  struct reason *r;
  r = fc_malloc(1 * sizeof(r)); 
  r->rtype = rt;
  r->value = value;
  r->halflife = halflife;
}

void reason_destroy(struct reason *preason) {
  free(&preason->rtype);
  free(&preason->value);
  free(&preason->halflife);
  free(preason);
  preason = NULL;
}

/*create a favorite condition*/
void favorite_new(struct ai_variant *paivari, struct universal type, int value) {
  fc_assert(universals_n_is_valid(type.kind));
  fc_assert_msg(value >= ATTITUDE_FAVOR_MIN && value <= ATTITUDE_FAVOR_MAX, 
            "Invalid value %d for favorite. Must be between %d and %d",
            value, ATTITUDE_FAVOR_MIN, ATTITUDE_FAVOR_MAX);
            
  struct favorite *fav;
  fav = fc_malloc(1 * sizeof(*fav)); 
  fav->type=type;
  fav->value=value;
  ai_variant_favorite_amend(paivari, fav);
}

void favorite_destroy(struct favorite *pfavor) {
  free(&pfavor->type);
  free(&pfavor->value);
  free(pfavor);
}

bool ai_variant_reason_amend(struct ai_variant *paivari, struct reason *preason) {
  bool success=FALSE;
  enum reason_type rtype;
  rtype = preason->rtype;
  
  reason_list_iterate(paivari->reasons, areason) {
    if(areason->rtype == rtype) {
      //change value, halflife of the matching reason
      success=TRUE;
      areason->value = preason->value;
      areason->halflife = preason->halflife;
    }
  } reason_list_iterate_end;
  
  //no matching rtype in reasons, so append one to reasons
  if(!success) {
    reason_new(paivari, preason->rtype, preason->value, preason->halflife);
  }
  
  return success;
}

bool ai_variant_reason_remove(struct ai_variant *paivari, struct reason *preason) {
  bool success=FALSE;
  return success;
}

bool ai_variant_favorite_amend(struct ai_variant *paivari, struct favorite *pfavor) {
  bool success=FALSE;
  
  return success;
}

bool ai_variant_favorite_remove(struct ai_variant *paivari, struct favorite *pfavor) {
  bool success=FALSE;
  
  return success;
}
/*Is there a leader by this name among the nations?*/
bool rules_have_leader(const char *name) {
  bool found = FALSE;
  nations_iterate(pnation) {
    struct nation_leader *pleader = nation_leader_by_name(pnation, name);
    if (NULL != pleader) { 
      found = TRUE;
      break;
    }
  } nations_iterate_end;
  return found;
}
