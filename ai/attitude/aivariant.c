#include <string.h>
/* common */
#include "nation.h"
#include "game.h"
#include "requirements.h"

/* ai/attitude */
#include "utils.h"

#include "aivariant.h"

static struct ai_variant *master_aiv_list = NULL;
static bool AIV_INITIALIZED = FALSE;
static ai_variant_id aiv_id= 0;

static void ai_variant_new(struct ai_variant *paivari);

const char *ai_variant_name(struct ai_variant *paivari) {
  fc_assert_ret_val(NULL != paivari, NULL);
  return paivari->name;
}

struct ai_variant *ai_variant_by_number(const ai_variant_id id) {
  if (id >= 0 && id <= aiv_id) {
    return NULL;
  }
  
  return master_aiv_list + id;
}

void ai_variants_init(int num) {
  struct ai_variant *paivari;
  int i;
    /* Ensure we have enough space for players or teams. 
     *TODO: Need more ais in ruleset, for testing.*/
  /*fc_assert_msg(ai_variant_list_size(ai_variant_array) >= team_slot_count());*/
  /*fc_assert_msg(ai_variant_list_size(ai_variant_array) >= player_slot_count());*/
  master_aiv_list = fc_malloc(num * sizeof(struct ai_variant));
  
  for (i = 0; i < num; i++) {
    ai_variant_new(paivari);
  }
    /*TODO: array_pack(master_aiv_list, i)
   * Because I don't want to mess with game.h for a dll/la
   */
   AIV_INITIALIZED = TRUE;
}

bool aiv_initialized(void) {
  return AIV_INITIALIZED;
}

void ai_variants_free(void) {
  FC_FREE(master_aiv_list);
  master_aiv_list = NULL;
  AIV_INITIALIZED = FALSE;
}

/*TODO: set the name*/
static void ai_variant_new(struct ai_variant *paivari) {
  memset(paivari, 0, sizeof(*paivari));
  enum reason_type rtype;
  enum universals_n ftype;
      
  for(rtype = reason_type_begin(); rtype != reason_type_end(); 
          rtype = reason_type_next(rtype)) {
    if(!ai_variant_reason_reset(paivari, rtype)) { /* does not exist yet */
      reason_new(paivari, rtype, 
        ATTITUDE_REASON_DEFAULT_VALUE, 
        ATTITUDE_HALFLIFE_DEFAULT_TURNS);
    }
  }
  
  for(ftype = universals_n_begin(); ftype != universals_n_end(); 
          ftype = universals_n_next(ftype)) {
    if(!ai_variant_favorite_reset(paivari, ftype)) { /* does not exist yet */
      favorite_new(paivari, ftype, ATTITUDE_FAVOR_DEFAULT);
    }
  }
  /*TODO: need to get name from somewhere*/
  paivari->id = ++aiv_id;
  struct ai_variant *next_aiv = master_aiv_list + aiv_id;
  memmove(next_aiv, paivari, sizeof(*paivari));
}

void ai_variant_destroy(const char *name) {

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
void favorite_new(struct ai_variant *paivari, enum universals_n kind, int value) {
  fc_assert(universals_n_is_valid(kind));
  fc_assert_msg(value >= ATTITUDE_FAVOR_MIN && value <= ATTITUDE_FAVOR_MAX, 
            "Invalid value %d for favorite. Must be between %d and %d",
            value, ATTITUDE_FAVOR_MIN, ATTITUDE_FAVOR_MAX);
            
  struct favorite *fav;
  fav = fc_malloc(1 * sizeof(*fav)); 
  fav->type.kind=kind;
  /* fav->type.value=NULL; */
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
  int value;
  int halflife;
  enum reason_type type;
  
  value = preason->value;
  halflife = preason->halflife;
  type = preason->type;
  
  fc_assert_msg(value>=ATTITUDE_REASON_MIN_VALUE &&
            value<=ATTITUDE_REASON_MAX_VALUE, 
            "Invalid value %d for reason. Must be between %d and %d",
            value, ATTITUDE_REASON_MIN_VALUE, ATTITUDE_REASON_MAX_VALUE);
  fc_assert_msg(halflife>=ATTITUDE_HALFLIFE_MIN_TURNS &&
            halflife<=ATTITUDE_HALFLIFE_MAX_TURNS, 
            "Invalid halflife %d for reason. Must be between %d and %d",
            halflife, ATTITUDE_HALFLIFE_MIN_TURNS, ATTITUDE_HALFLIFE_MAX_TURNS);
  fc_assert_msg(reason_type_is_valid(type), "Invalid reason_type %s", 
            reason_type_name(type)); 
  
  reason_list_iterate(paivari->reasons, areason) {
    if(areason->type == type) {
      /*change value, halflife of the matching reason*/
      if (areason->value != value) {
        changed = TRUE;
        areason->value = value;
      }
      if (areason->halflife != halflife) {
        changed = TRUE;
        areason->halflife = halflife;
      }
      
    }
  } reason_list_iterate_end;
  
  /*no matching type in reasons, so append this one to paivari->reasons*/
  if(!changed) {
    reason_new(paivari, type, value, halflife);
  }
  
  return changed;
}

bool ai_variant_reason_reset(struct ai_variant *paivari, enum reason_type ptype) {
  bool changed=FALSE;
  
  reason_list_iterate(paivari->reasons, preason) {
    if(0 == strcmp(reason_type_name(preason->type), reason_type_name(ptype))) {
      if (preason->value != ATTITUDE_REASON_DEFAULT_VALUE || 
            preason->halflife != ATTITUDE_HALFLIFE_DEFAULT_TURNS) {
        changed = TRUE;
        reason_new(paivari, ptype, ATTITUDE_REASON_DEFAULT_VALUE, ATTITUDE_HALFLIFE_DEFAULT_TURNS);
      }
    }
  } reason_list_iterate_end;
  
  return changed;
}

bool ai_variant_favorite_amend(struct ai_variant *paivari, struct favorite *pfavor) {
  bool changed=FALSE;
  enum universals_n kind;
  int value;
  
  kind = pfavor->type.kind;
  value = pfavor->value;
  
  fc_assert(universals_n_is_valid(kind));
  fc_assert_msg(value >= ATTITUDE_FAVOR_MIN && value <= ATTITUDE_FAVOR_MAX, 
            "Invalid value %d for favorite. Must be between %d and %d",
            value, ATTITUDE_FAVOR_MIN, ATTITUDE_FAVOR_MAX);
            
  /* TODO: validate pfavor->type.value*/
  favorite_list_iterate(paivari->favorites, afavor) {
    if (kind == afavor->type.kind) {
      if (0 == universalcmp(pfavor->type, afavor->type)) {
        changed = TRUE;
        afavor->type.value = pfavor->type.value;
      } 
      else {
        changed = TRUE;
        afavor->value = value;
      }
    }
  } favorite_list_iterate_end;
  /*TODO: should I free pfavor here?*/
  return changed;
}

/* foo_remove actually resets default values */
bool ai_variant_favorite_reset(struct ai_variant *paivari, enum universals_n type) {
  bool changed=FALSE;
  
  favorite_list_iterate(paivari->favorites, oldfav) {
    if (type == oldfav->type.kind) {
      if (oldfav->value == ATTITUDE_FAVOR_DEFAULT) {
        changed = FALSE;
      } 
      else {
        oldfav->value = ATTITUDE_FAVOR_DEFAULT;
        changed = TRUE;
      }
      break;
    }    
  } favorite_list_iterate_end;
  
  return changed;
}

/** Is there a leader by this name among the nations? */
bool rules_have_leader(const char *name) {
  bool found = FALSE;
  nations_iterate(pnation) {
    struct nation_leader *pleader = NULL;
    pleader = nation_leader_by_name(pnation, name);
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
    return CLIP(ait.mod, ait.val, 100);
  } else {
    return CLIP(0, ait.val, 100-ait.mod);
  }
}

struct ai_trait favorite_as_trait(struct favorite *pfavor) {
  struct ai_trait f_trait;
  int ft_val, ft_mod;
  
  ft_val = ATTITUDE_FAVOR_DEFAULT;
  ft_mod = pfavor->value;
  
  f_trait.val=ft_val; 
  f_trait.mod=ft_mod;
  
  return f_trait;
}

struct trait_limits favorite_limits(void) {
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
  
  paivari = ai_variant_by_number(our_aiv_id);
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

struct trait_limit reason_limits(void) {
  struct trait_limits r_lims;
  r_lims.min=ATTITUDE_REASON_MIN_VALUE;
  r_lims.max=ATTITUDE_REASON_MAX_VALUE;
  return r_lims;
}

bool player_has_variant(struct player *pplayer) {
  bool is_ai;
  const char *nlname, *ainame;
  
  is_ai = pplayer->ai_controlled;
  if (!is_ai) {
    return FALSE;
  } /* else */
  
  nlname = player_name(*pplayer);
  ai_variant_list_iterate(ai_variants(), paivari) {
    ainame = paivari->name;
    if (0 == strcmp(nlname, ainame)) {
       return TRUE;
    }
  } ai_variant_list_end;
  
  return FALSE;
}
