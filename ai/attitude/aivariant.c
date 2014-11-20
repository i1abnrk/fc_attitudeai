#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "nation.h"
#include "game.h"
#include "requirements.h"

#include "aivariant.h"

static struct ai_variant_list master_aiv_list;
static bool AIV_INITIALIZED=FALSE;

const char *ai_variant_name(struct ai_variant *paivari) {
  fc_assert_ret_val(NULL != paivari, NULL);
  return paivari->name;
}

struct ai_variant *ai_variant_by_number(ai_variant_id id) {
  fc_assert_msg(id >= 0 && id < ai_variant_list_size(master_aiv_list),
    "AI Variant id %d out of range. Must be between %d and %d",
    id, 0, ai_variant_list_size(master_aiv_list));

  return ai_variant_list_get(master_aiv_list, id);
}

void ai_variants_init(void) {
  int i;
    /* Ensure we have enough space for players or teams. 
     *TODO: Need more ais in ruleset, for testing.*/
  /*fc_assert_msg(ai_variant_list_size(ai_variant_array) >= team_slot_count());*/
  /*fc_assert_msg(ai_variant_list_size(ai_variant_array) >= player_slot_count());*/
  master_aiv_list = ai_variant_list_new();
  
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
/** Create a new reason for the leader data.*/
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
  r->type = rt;
  r->value = value;
  r->halflife = halflife;
}

void reason_destroy(struct reason *preason) {
  free(&preason->type);
  free(&preason->value);
  free(&preason->halflife);
  free(preason);
  preason = NULL;
}

/** Create a favorite condition. */
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
  bool changed = FALSE;
  enum reason_type type;
  type = preason->type;
  
  reason_list_iterate(paivari->reasons, areason) {
    if(areason->type == type) {
      /*change value, halflife of the matching reason*/
      if (areason->value != preason->value) {
        changed = TRUE;
        areason->value = preason->value;
      }
      if (areason->halflife != preason->halflife) {
        changed = TRUE;
        areason->halflife = preason->halflife;
      }
      
    }
  } reason_list_iterate_end;
  
  /*no matching type in reasons, so append one to reasons*/
  if(!changed) {
    reason_new(paivari, preason->type, preason->value, preason->halflife);
  }
  
  return changed;
}

bool ai_variant_reason_reset(struct ai_variant *paivari, enum reason_type *ptype) {
  bool changed=FALSE;
  
  reason_list_iterate(paivari->reasons, preason) {
    if(preason->type == ptype) {
      if (preason->value != ATTITUDE_REASON_DEFAULT_VALUE || 
            areason->halflife != ATTITUDE_HALFLIFE_DEFAULT_TURNS) {
        changed = TRUE;
        reason_new(paivari, ptype, ATTITUDE_REASON_DEFAULT_VALUE, ATTITUDE_HALFLIFE_DEFAULT_TURNS);
      }
    }
  } reason_list_iterate_end;
  return changed;
}

bool ai_variant_favorite_amend(struct ai_variant *paivari, struct favorite *pfavor) {
  bool changed=FALSE;
  
  fc_assert(universals_n_is_valid(pfavor->type.kind));
  fc_assert_msg(pfavor->value >= ATTITUDE_FAVOR_MIN && value <= ATTITUDE_FAVOR_MAX, 
            "Invalid value %d for favorite. Must be between %d and %d",
            value, ATTITUDE_FAVOR_MIN, ATTITUDE_FAVOR_MAX);
            
  struct universal type;
  /* TODO: validate pfavor->type.value*/
  favorite_list_iterate(paivari->favorites, afavor) {
    if (pfavor->type.kind == afavor->type.kind) {
      if (afavor->type.value != pfavor->type.value) {
        changed = TRUE;
        afavor->type.value = pfavor->type.value;
      }
      if (afavor->value != pfavor->value) {
        changed = TRUE;
        afavor->value = pfavor->value;
      }
    }
  } favorite_list_iterate_end;
  /*TODO: should I free pfavor here?*/
  return changed;
}

/* foo_remove actually resets default values */
bool ai_variant_favorite_reset(struct ai_variant *paivari, enum universal type) {
  bool changed=FALSE;
  enum universals_n un;
  struct favorite fav=favorite_new(type, ATTITUDE_FAVOR_DEFAULT);
  
  favorite_list_iterate(paivari->favorites, oldfav) {
    if (type == oldfav->type) {
      if (oldfav != fav) {
        changed = TRUE;
        oldfav = fav;
      }
    }    
  } favorite_list_iterate_end();
  
  return changed;
}

/** Is there a leader by this name among the nations? */
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

/**
 * As ai_trait_get_value in aitraits.h, but clips negative mod from the high
 * end as well. Ai_variant model uses multiplicative, whereas trait model uses
 * an additive model.
 */
 
int aai_clip(struct ai_trait ait) {
  if (ait.mod >= 0) { 
    return CLIP(ait.mod, ait.val, 100)
  } else {
    return CLIP(0, ait.val, 100-ait.mod)
  }
}

struct ai_trait *favorite_as_trait(struct favorite *pfavor) {
  struct ai_trait f_trait;
  int ft_val, ft_mod;
  
  ft_val = ATTITUDE_FAVOR_DEFAULT;
  ft_mod = pfavor->value;
  
  f_trait = (ai_trait) {"val"=ft_val, "mod"=ft_mod};
  
  return f_trait;
}

struct trait_limits *favorite_limits(void) {
  struct trait_limits f_lims;
  f_lims.min = ATTITUDE_FAVOR_MIN;
  f_lims.max = ATTITUDE_FAVOR_MAX;
  return f_lims;
}

struct ai_trait *reason_as_trait(int our_aiv_id, int their_slot_id, enum reason_type *prtype) {
  struct ai_trait r_trait;
  struct aivariant *paivari;
  enum reason_type rt_type;
  int curr_turn, rt_adj, rt_hl, rt_turns, rt_val, slot;
  
  paivari = get_aiv_by_number(our_aiv_id);
  rt_type = prtype;
  slot = their_slot_id;
  curr_turn = game.info.turn;
  
  /*calculate memories*/
  leader_memory_list_iterate(paivari, pmemory) {
    enum reason_type mtype = pmemory->reason.type;
    if (rt_type == mtype) {
      if (slot == pmemory->nation) {
        rt_turns = curr_turn - pmemory->first_turn;
        rt_hl = pmemory.reason.halflife;
        rt_val = pmemory.reason.value;
        rt_adj = calc_halflife(rt_val, rt_hl, rt_turns);
        rt_adj = ((pmemory->sympathetic)?rt_adj:-rt_adj));
      } else {
        rt_adj = 0;
      }
    } else {
      rt_adj = 0;
    }
  } leader_memory_list_end;
  
  r_trait = (ai_trait) {"val"=ATTITUDE_REASON_DEFAULT_VALUE, "mod"=rt_adj};  
  r_trait = aai_clip(r_trait);
  
  return r_trait;
}

struct trait_limit *reason_limits(void) {
  struct trait_limits r_lims;
  r_lims.min=ATTITUDE_REASON_MIN_VALUE;
  r_lims.max=ATTITUDE_REASON_MAX_VALUE;
  return r_lims;
}

/*maybe unnecessary*/
struct ai_trait *memory_as_trait(int aiv_id) {

}

struct trait_limit *memory_limits(void) {

}
