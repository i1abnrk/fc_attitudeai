#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

#include <stdio.h>
#include <string.h>

//utility
#include "capability.h"
#include "log.h"
#include "shared.h"
#include "support.h"

//common
#include "game.h"
#include "nation.h"

//server
#include "ruleset.h"

#include "utils.h"
#include "leaderrules.h"

#define AI_VARIANT_SECTION_PREFIX "aivariant_"
#define RULES_SUFFIX "ruleset"

static const char *check_ruleset_capabilities(struct section_file *file,
                                              const char *us_capstr,
                                              const char *filename);
static struct section_file *openload_ruleset_file(const char *whichset);
static const char *valid_ruleset_filename(const char *subdir,
                                          const char *name,
                                          const char *extension);

/**************************************************************************
  Ruleset files should have a capabilities string datafile.options
  This gets and returns that string, and checks that the required
  capabilities specified are satisified.
**************************************************************************/
static const char *check_ruleset_capabilities(struct section_file *file,
                                              const char *us_capstr,
                                              const char *filename)
{
  const char *datafile_options;

  if (!(datafile_options = secfile_lookup_str(file, "datafile.options"))) {
    log_fatal("\"%s\": ruleset capability problem:", filename);
    ruleset_error(LOG_FATAL, "%s", secfile_error());
  }
  if (!has_capabilities(us_capstr, datafile_options)) {
    log_fatal("\"%s\": ruleset datafile appears incompatible:", filename);
    log_fatal("  datafile options: %s", datafile_options);
    log_fatal("  supported options: %s", us_capstr);
    ruleset_error(LOG_FATAL, "Capability problem");
  }
  if (!has_capabilities(datafile_options, us_capstr)) {
    log_fatal("\"%s\": ruleset datafile claims required option(s)"
              " that we don't support:", filename);
    log_fatal("  datafile options: %s", datafile_options);
    log_fatal("  supported options: %s", us_capstr);
    ruleset_error(LOG_FATAL, "Capability problem");
  }
  return datafile_options;
}

/**************************************************************************
  datafilename() wrapper: tries to match in two ways.
  Returns NULL on failure, the (statically allocated) filename on success.
**************************************************************************/
static const char *valid_ruleset_filename(const char *subdir,
                                          const char *name,
                                          const char *extension)
{
  char filename[512];
  const char *dfilename;

  fc_assert_ret_val(subdir && name && extension, NULL);

  fc_snprintf(filename, sizeof(filename), "%s/%s.%s",
              subdir, name, extension);
  log_verbose("Trying \"%s\".", filename);
  dfilename = fileinfoname(get_data_dirs(), filename);
  if (dfilename) {
    return dfilename;
  }

  fc_snprintf(filename, sizeof(filename), "default/%s.%s", name, extension);
  log_verbose("Trying \"%s\": default ruleset directory.", filename);
  dfilename = fileinfoname(get_data_dirs(), filename);
  if (dfilename) {
    return dfilename;
  }

  fc_snprintf(filename, sizeof(filename), "%s_%s.%s",
              subdir, name, extension);
  log_verbose("Trying \"%s\": alternative ruleset filename syntax.",
              filename);
  dfilename = fileinfoname(get_data_dirs(), filename);
  if (dfilename) {
    return dfilename;
  } else {
    ruleset_error(LOG_FATAL,
                  /* TRANS: message about an installation error. */
                  _("Could not find a readable \"%s.%s\" ruleset file."),
                  name, extension);
  }

  return(NULL);
}


/**************************************************************************
  Do initial section_file_load on a ruleset file.
  "whichset" = "techs", "units", "buildings", "terrain", ...
**************************************************************************/
static struct section_file *openload_ruleset_file(const char *whichset)
{
  char sfilename[512];
  const char *dfilename = valid_ruleset_filename(game.server.rulesetdir,
                                                 whichset, RULES_SUFFIX);
  struct section_file *secfile;

  /* Need to save a copy of the filename for following message, since
     section_file_load() may call datafilename() for includes. */

  sz_strlcpy(sfilename, dfilename);

  if (!(secfile = secfile_load(sfilename, FALSE))) {
    ruleset_error(LOG_FATAL, "Could not load ruleset '%s':\n%s\n Attitude AI requires it.",
                  sfilename, secfile_error());
  }
  return secfile;
}

/**************************************************************************
  Load "name" and (optionally) "rule_name" into a struct name_translation.
**************************************************************************
static bool ruleset_load_names(struct name_translation *pname,
                               const char *domain,
                               struct section_file *file,
                               const char *sec_name)
{
  const char *name = secfile_lookup_str(file, "%s.name", sec_name);
  const char *rule_name = secfile_lookup_str(file, "%s.rule_name", sec_name);

  if (!name) {
    ruleset_error(LOG_ERROR,
                  "\"%s\" [%s]: no \"name\" specified.",
                  secfile_name(file), sec_name);
    return FALSE;
  }

  names_set(NULL, domain, name, rule_name);

  return TRUE;
}*/


static void load_aivariants(struct section_file *file);

/*static const char *check_ai_names(struct ai_type *pai,
                                     struct ai_type **ppconflictai);
                                     */

/*********************************************************************
 * Match on rules_have_leader(section.name) in aivariant.h,
 * else use ai/default/foo 
 ********************************************************************/
static void load_aivariants(struct section_file *file) {
  /*load_aivariant_names()*/
  struct section_list *sec;
  int i, num_aiv = 0;
  const char *filename = secfile_name(file);
  (void) check_ruleset_capabilities(file, RULESET_CAPABILITIES, filename);
  (void) secfile_entry_by_path(file, "datafile.description");   /* unused */
  
  sec = secfile_sections_by_name_prefix(file, AI_VARIANT_SECTION_PREFIX);
  if (NULL == sec || 0 == (num_aiv = section_list_size(sec))) {
    ruleset_error(LOG_FATAL, "\"%s\": No ai variants?!?", filename);
  }
  /*TODO: Sane number for MAX_NUM_AI_VARIANTS*/
  if(num_aiv > MAX_NUM_AI_VARIANTS) {
    ruleset_error(LOG_FATAL, "\"%s\": Too many AI variants (%d, max %d)",
                  filename, num_aiv, MAX_NUM_AI_VARIANTS);
  }
  log_verbose("%d ai variants (including possibly unused)", num_aiv);

  /* TODO: Support ruleset translation in Attitude AI.
    Note to translator: "name" is the same as nation_leader->name
    favorite
  for (i = 0; i < num_aiv; i++) {
    struct ai_variant *v = ai_variant_by_number(i);
    const char *sec_name = section_name(section_list_get(sec, i))
    v->name = ;
  }*/
  
  
  if (!aiv_initialized()) {
    ai_variants_init(num_aiv);
  }
  
  i = 0;
  section_list_iterate(sec, psection) {
  
    const char *sec_name = section_name(psection);
    /*****************************************************************
     * nation rules must be loaded first
     * leader name must match one defined in nation rulesets, DEBUG ME!
     ****************************************************************/
    struct ai_variant *v = ai_variant_by_number(i);
    v->id = i;
    const char *vname = secfile_lookup_str(file, "%s.%s", sec_name, "name");
    if (!rules_have_leader(vname)) {
      ruleset_error(LOG_ERROR, "Leader name in aivariants.ruleset \'%s\' not found in nations.ruleset",
                 vname);
    }
    sz_strlcpy(v->name, vname);
    load_reasons(file, v, psection);
    load_favorites(file, v, psection);
    
    /*TODO: LOAD memories FROM SAVEFILE*/
    v->memory = leader_memory_list_new();
    i++;
  } section_list_iterate_end;

  section_list_destroy(sec);
}

enum reason_type reason_type_by_rule(const char *type) {
  enum reason_type rtype = reason_type_by_name(type, fc_strcasecmp);
  /*TODO: validate reason_type
  if(!reason_type_is_valid(rtype)) {
    ruleset_error(LOG_FATAL, ("Invalid reason_type \'%s\' in ruleset %s."), 
        rtype, rulename);
  }*/
  return rtype;
}

void load_reasons(struct section_file *file, struct ai_variant *paivari, 
                  const struct section *psection) {
  const char *sec = section_name(psection);
  const char *rstr = "reasons";
  const char *type;
  int val, hl;
  int row = 0;
  /*for each row in reasons table do:
      get the reason_type from the "type" column value
      get the value from the "value" column
      get the halflife from the "halflife" column
    endfor*/
  type = secfile_lookup_str_default(file, NULL, "%s.%s%d.type", sec, rstr, row);
  while (NULL!=type && 0!=fc_strcasecmp("LAST", type)) {
    val = LOOKUP_INT_REASON_VAL(file, "%s.%s%d.value", sec, rstr, row);
    hl = LOOKUP_INT_REASON_HL(file, "%s.%s%d.halflife", sec, rstr, row);
    row++;
    
    enum reason_type rtype = reason_type_by_rule(type);
    reason_new(paivari, rtype, val, hl);
    type = secfile_lookup_str_default(file, NULL, "%s.%s%d.type", sec, rstr, row);
  }//wend
}

void load_favorites(struct section_file *file, 
                    struct ai_variant *paivari, 
                    const struct section *psection) {
  const char *sec = section_name(psection);
  const char *fstr = "favorite";
  const char *type, *name;
  int val;
  int row = 0;
  //for each row in reasons table do:
  
    //get the reason_type from the "type" column value
    //get the value from the "value" column
    //get the halflife from the "halflife" columns
  //endfor
  type = secfile_lookup_str_default(file, NULL, "%s.%s%d.type", sec, fstr, row);
  name = secfile_lookup_str_default(file, NULL, "%s.%s%d.name", sec, fstr, row);
  while (NULL!=type && 0!=fc_strcasecmp("LAST", type)) {
    val = LOOKUP_INT_FAVORITE_VAL(file, "%s.%s%d.value", sec, fstr, row);
    row++;
    
    enum universals_n kind = universals_n_by_name(name);
    favorite_new(paivari, kind, val);
    type = secfile_lookup_str_default(file, NULL, "%s.%s%d.type", sec, fstr, row);
    name = secfile_lookup_str_default(file, NULL, "%s.%s%d.name", sec, fstr, row);
  }//wend
}

void load_ai_variant_rulesets(bool act) {
  struct section_file *aivarifile;
  
  aivarifile = openload_ruleset_file("aivariant");
  load_aivariants(aivarifile);
}
