#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "ai.h"
#include "player.h"

/* server */
#include "aiiface.h"

/* h */
#include "attitudeai.h"

static struct ai_type *self = NULL;
/**************************************************************************
  Return module capability string
**************************************************************************/
const char *fc_ai_attitude_capstr(void)
{
  return FC_AI_MOD_CAPSTR;
}

static struct ai_type *att_get_type(void) {
  return self;
}

/**************************************************************************
  Setup player ai_funcs function pointers.
**************************************************************************/
bool fc_ai_attitude_setup(struct ai_type *ai)
{
  strncpy(ai->name, "attitude", sizeof(ai->name));
  /* ai->funcs.game_free = NULL;
  TODO: Write corresponding functions (foreign key = local name)*/
  ai->funcs.player_alloc = att_player_alloc;
  ai->funcs.player_free = att_player_free;
  /* ai->funcs.player_save = dai_player_save;
  ai->funcs.player_load = dai_player_load;
  ai->funcs.gained_control = dai_assess_danger_player;
  ai->funcs.lost_control = NULL;
  ai->funcs.split_by_civil_war = dai_assess_danger_player;
  ai->funcs.created_by_civil_war = cai_created_by_civil_war;\
  
  ai->funcs.phase_begin = dai_data_phase_begin;
  ai->funcs.phase_finished = dai_data_phase_finished; free expiring memories

  ai->funcs.city_alloc = dai_city_alloc;
  ai->funcs.city_free = dai_city_free;
  
  ai->funcs.city_got = NULL;
  ai->funcs.city_lost = NULL;
  
  ai->funcs.city_save = dai_city_save;
  ai->funcs.city_load = dai_city_load;
  ai->funcs.choose_building = dai_build_adv_override;
  ai->funcs.build_adv_prepare = dai_wonder_city_distance;
  ai->funcs.build_adv_init = cai_build_adv_init;
  ai->funcs.build_adv_adjust_want = dai_build_adv_adjust;
  
  ai->funcs.gov_value = cai_gov_value;

  ai->funcs.units_ruleset_init = dai_units_ruleset_init;
  ai->funcs.units_ruleset_close = cai_units_ruleset_close;

  FIXME: We should allocate memory only for units owned by
     default ai in unit_got. We track no data
     about enemy units.
     But advisors code still depends on some default ai data (role) to be
     always allocated.
  
  ai->funcs.unit_alloc = NULL;
  ai->funcs.unit_free = NULL;
  ai->funcs.unit_got = dai_unit_init;
  ai->funcs.unit_lost = dai_unit_close;
  
  ai->funcs.unit_created = dai_ferry_init_ferry;
  ai->funcs.unit_transformed = cai_ferry_transformed;

  ai->funcs.unit_turn_end = dai_unit_turn_end;
  ai->funcs.unit_move = dai_unit_move_or_attack;
  ai->funcs.unit_task = dai_unit_new_adv_task;

  ai->funcs.unit_save = dai_unit_save;
  ai->funcs.unit_load = dai_unit_load;

  ai->funcs.settler_reset = dai_auto_settler_reset;
  ai->funcs.settler_run = dai_auto_settler_run;
  ai->funcs.settler_cont = dai_auto_settler_cont;

  ai->funcs.first_activities = dai_do_first_activities;
  ai->funcs.restart_phase = cai_restart_phase;
  ai->funcs.diplomacy_actions = dai_diplomacy_actions;
  ai->funcs.last_activities = dai_do_last_activities;
  ai->funcs.treaty_evaluate = dai_treaty_evaluate;
  ai->funcs.treaty_accepted = dai_treaty_accepted;
  ai->funcs.first_contact = dai_diplomacy_first_contact;
  ai->funcs.incident = dai_incident;

  ai->funcs.log_fragment_city = dai_city_log;
  ai->funcs.log_fragment_unit = dai_unit_log;

  ai->funcs.consider_plr_dangerous = dai_consider_plr_dangerous;
  ai->funcs.consider_tile_dangerous = dai_consider_tile_dangerous;
  ai->funcs.consider_wonder_city = dai_consider_wonder_city;
  
  ai->funcs.refresh = NULL;
  */
  return TRUE;
}

void att_player_alloc(struct player *pplayer) {
  /*
  * if the player is ai
  *   if ai variant exists for player->leader
        initialize ai variants
  *   endif
  * endif
  */
  if (player_has_variant()) {
    if (!aiv_initialized()) {
      ai_variants_init();
    }
  }
  struct ai_type *deftype = att_get_type();
  dai_player_alloc(deftype, pplayer);
}

void att_player_free(struct player *pplayer) {
  if (player_has_variant()) {
    if (aiv_initialized()) {
      ai_variants_free();
    }
  }
  struct ai_type *deftype = att_get_type();
  dai_player_free(deftype, pplayer);
}

