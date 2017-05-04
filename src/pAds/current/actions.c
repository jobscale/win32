const char actions_rcs[] = "";
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/actions.c,v $
 *
 * Purpose     :  Declares functions to work with actions files
 *                Functions declared include: FIXME
 *
 * Copyright   :  Written by and Copyright (C) 2001-2008 the SourceForge
 *                pAds team. http://www.pads.com/
 *
 *                Based on the Internet Junkbuster originally written
 *                by and Copyright (C) 1997 Anonymous Coders and
 *                Junkbusters Corporation.  http://www.junkbusters.com
 *
 *                This program is free software; you can redistribute it
 *                and/or modify it under the terms of the GNU General
 *                Public License as published by the Free Software
 *                Foundation; either version 2 of the License, or (at
 *                your option) any later version.
 *
 *                This program is distributed in the hope that it will
 *                be useful, but WITHOUT ANY WARRANTY; without even the
 *                implied warranty of MERCHANTABILITY or FITNESS FOR A
 *                PARTICULAR PURPOSE.  See the GNU General Public
 *                License for more details.
 *
 *                The GNU General Public License should be included with
 *                this file.  If not, you can view it at
 *                http://www.gnu.org/copyleft/gpl.html
 *                or write to the Free Software Foundation, Inc., 59
 *                Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *********************************************************************/


#include "config.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#ifdef FEATURE_PTHREAD
#include <pthread.h>
#endif

#include "project.h"
#include "jcc.h"
#include "list.h"
#include "actions.h"
#include "miscutil.h"
#include "errlog.h"
#include "loaders.h"
#include "encode.h"
#include "urlmatch.h"
#include "cgi.h"
#include "ssplit.h"

const char actions_h_rcs[] = ACTIONS_H_VERSION;


/*
 * We need the main list of options.
 *
 * First, we need a way to tell between boolean, string, and multi-string
 * options.  For string and multistring options, we also need to be
 * able to tell the difference between a "+" and a "-".  (For bools,
 * the "+"/"-" information is encoded in "add" and "mask").  So we use
 * an enumerated type (well, the preprocessor equivalent).  Here are
 * the values:
 */
#define AV_NONE       0 /* +opt -opt */
#define AV_ADD_STRING 1 /* +stropt{string} */
#define AV_REM_STRING 2 /* -stropt */
#define AV_ADD_MULTI  3 /* +multiopt{string} +multiopt{string2} */
#define AV_REM_MULTI  4 /* -multiopt{string} -multiopt          */

/*
 * We need a structure to hold the name, flag changes,
 * type, and string index.
 */
struct action_name
{
   const char * name;
   unsigned long mask;   /* a bit set to "0" = remove action */
   unsigned long add;    /* a bit set to "1" = add action */
   int takes_value;      /* an AV_... constant */
   int index;            /* index into strings[] or multi[] */
};

/*
 * And with those building blocks in place, here's the array.
 */
static const struct action_name action_names[] =
{
   /*
    * Well actually there's no data here - it's in actionlist.h
    * This keeps it together to make it easy to change.
    *
    * Here's the macros used to format it:
    */
#define DEFINE_ACTION_MULTI(name,index)                   \
   { "+" name, ACTION_MASK_ALL, 0, AV_ADD_MULTI, index }, \
   { "-" name, ACTION_MASK_ALL, 0, AV_REM_MULTI, index },
#define DEFINE_ACTION_STRING(name,flag,index)                 \
   { "+" name, ACTION_MASK_ALL, flag, AV_ADD_STRING, index }, \
   { "-" name, ~flag, 0, AV_REM_STRING, index },
#define DEFINE_ACTION_BOOL(name,flag)   \
   { "+" name, ACTION_MASK_ALL, flag }, \
   { "-" name, ~flag, 0 },
#define DEFINE_ACTION_ALIAS 1 /* Want aliases please */

#include "actionlist.h"

#undef DEFINE_ACTION_MULTI
#undef DEFINE_ACTION_STRING
#undef DEFINE_ACTION_BOOL
#undef DEFINE_ACTION_ALIAS

   { NULL, 0, 0 } /* End marker */
};


static int load_one_actions_file(struct client_state *csp, int fileid);


/*********************************************************************
 *
 * Function    :  merge_actions
 *
 * Description :  Merge two actions together.
 *                Similar to "dest += src".
 *
 * Parameters  :
 *          1  :  dest = Actions to modify.
 *          2  :  src = Action to add.
 *
 * Returns     :  JB_ERR_OK or JB_ERR_MEMORY
 *
 *********************************************************************/
jb_err merge_actions (struct action_spec *dest,
                      const struct action_spec *src)
{
   int i;
   jb_err err;

   dest->mask &= src->mask;
   dest->add  &= src->mask;
   dest->add  |= src->add;

   for (i = 0; i < ACTION_STRING_COUNT; i++)
   {
      char * str = src->string[i];
      if (str)
      {
         freez(dest->string[i]);
         dest->string[i] = strdup(str);
         if (NULL == dest->string[i])
         {
            return JB_ERR_MEMORY;
         }
      }
   }

   for (i = 0; i < ACTION_MULTI_COUNT; i++)
   {
      if (src->multi_remove_all[i])
      {
         /* Remove everything from dest */
         list_remove_all(dest->multi_remove[i]);
         dest->multi_remove_all[i] = 1;

         err = list_duplicate(dest->multi_add[i], src->multi_add[i]);
      }
      else if (dest->multi_remove_all[i])
      {
         /*
          * dest already removes everything, so we only need to worry
          * about what we add.
          */
         list_remove_list(dest->multi_add[i], src->multi_remove[i]);
         err = list_append_list_unique(dest->multi_add[i], src->multi_add[i]);
      }
      else
      {
         /* No "remove all"s to worry about. */
         list_remove_list(dest->multi_add[i], src->multi_remove[i]);
         err = list_append_list_unique(dest->multi_remove[i], src->multi_remove[i]);
         if (!err) err = list_append_list_unique(dest->multi_add[i], src->multi_add[i]);
      }

      if (err)
      {
         return err;
      }
   }

   return JB_ERR_OK;
}


/*********************************************************************
 *
 * Function    :  copy_action
 *
 * Description :  Copy an action_specs.
 *                Similar to "dest = src".
 *
 * Parameters  :
 *          1  :  dest = Destination of copy.
 *          2  :  src = Source for copy.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
jb_err copy_action (struct action_spec *dest,
                    const struct action_spec *src)
{
   int i;
   jb_err err = JB_ERR_OK;

   free_action(dest);
   memset(dest, '\0', sizeof(*dest));

   dest->mask = src->mask;
   dest->add  = src->add;

   for (i = 0; i < ACTION_STRING_COUNT; i++)
   {
      char * str = src->string[i];
      if (str)
      {
         str = strdup(str);
         if (!str)
         {
            return JB_ERR_MEMORY;
         }
         dest->string[i] = str;
      }
   }

   for (i = 0; i < ACTION_MULTI_COUNT; i++)
   {
      dest->multi_remove_all[i] = src->multi_remove_all[i];
      err = list_duplicate(dest->multi_remove[i], src->multi_remove[i]);
      if (err)
      {
         return err;
      }
      err = list_duplicate(dest->multi_add[i],    src->multi_add[i]);
      if (err)
      {
         return err;
      }
   }
   return err;
}

/*********************************************************************
 *
 * Function    :  free_action_spec
 *
 * Description :  Frees an action_spec and the memory used by it.
 *
 * Parameters  :
 *          1  :  src = Source to free.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void free_action_spec(struct action_spec *src)
{
   free_action(src);
   freez(src);
}


/*********************************************************************
 *
 * Function    :  free_action
 *
 * Description :  Destroy an action_spec.  Frees memory used by it,
 *                except for the memory used by the struct action_spec
 *                itself.
 *
 * Parameters  :
 *          1  :  src = Source to free.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void free_action (struct action_spec *src)
{
   int i;

   if (src == NULL)
   {
      return;
   }

   for (i = 0; i < ACTION_STRING_COUNT; i++)
   {
      freez(src->string[i]);
   }

   for (i = 0; i < ACTION_MULTI_COUNT; i++)
   {
      destroy_list(src->multi_remove[i]);
      destroy_list(src->multi_add[i]);
   }

   memset(src, '\0', sizeof(*src));
}


/*********************************************************************
 *
 * Function    :  get_action_token
 *
 * Description :  Parses a line for the first action.
 *                Modifies it's input array, doesn't allocate memory.
 *                e.g. given:
 *                *line="  +abc{def}  -ghi "
 *                Returns:
 *                *line="  -ghi "
 *                *name="+abc"
 *                *value="def"
 *
 * Parameters  :
 *          1  :  line = [in] The line containing the action.
 *                       [out] Start of next action on line, or
 *                       NULL if we reached the end of line before
 *                       we found an action.
 *          2  :  name = [out] Start of action name, null
 *                       terminated.  NULL on EOL
 *          3  :  value = [out] Start of action value, null
 *                        terminated.  NULL if none or EOL.
 *
 * Returns     :  JB_ERR_OK => Ok
 *                JB_ERR_PARSE => Mismatched {} (line was trashed anyway)
 *
 *********************************************************************/
jb_err get_action_token(char **line, char **name, char **value)
{
   char * str = *line;
   char ch;

   /* set default returns */
   *line = NULL;
   *name = NULL;
   *value = NULL;

   /* Eat any leading whitespace */
   while ((*str == ' ') || (*str == '\t'))
   {
      str++;
   }

   if (*str == '\0')
   {
      return 0;
   }

   if (*str == '{')
   {
      /* null name, just value is prohibited */
      return JB_ERR_PARSE;
   }

   *name = str;

   /* parse option */
   while (((ch = *str) != '\0') &&
          (ch != ' ') && (ch != '\t') && (ch != '{'))
   {
      if (ch == '}')
      {
         /* error, '}' without '{' */
         return JB_ERR_PARSE;
      }
      str++;
   }
   *str = '\0';

   if (ch != '{')
   {
      /* no value */
      if (ch == '\0')
      {
         /* EOL - be careful not to run off buffer */
         *line = str;
      }
      else
      {
         /* More to parse next time. */
         *line = str + 1;
      }
      return JB_ERR_OK;
   }

   str++;
   *value = str;

   str = strchr(str, '}');
   if (str == NULL)
   {
      /* error */
      *value = NULL;
      return JB_ERR_PARSE;
   }

   /* got value */
   *str = '\0';
   *line = str + 1;

   chomp(*value);

   return JB_ERR_OK;
}

/*********************************************************************
 *
 * Function    :  action_used_to_be_valid
 *
 * Description :  Checks if unrecognized actions were valid in earlier
 *                releases.
 *
 * Parameters  :
 *          1  :  action = The string containing the action to check.
 *
 * Returns     :  True if yes, otherwise false.
 *
 *********************************************************************/
static int action_used_to_be_valid(const char *action)
{
   static const char *formerly_valid_actions[] = {
      "inspect-jpegs",
      "kill-popups",
      "send-vanilla-wafer",
      "send-wafer",
      "treat-forbidden-connects-like-blocks",
      "vanilla-wafer",
      "wafer"
   };
   unsigned int i;

   for (i = 0; i < SZ(formerly_valid_actions); i++)
   {
      if (0 == strcmpic(action, formerly_valid_actions[i]))
      {
         return TRUE;
      }
   }

   return FALSE;
}

/*********************************************************************
 *
 * Function    :  get_actions
 *
 * Description :  Parses a list of actions.
 *
 * Parameters  :
 *          1  :  line = The string containing the actions.
 *                       Will be written to by this function.
 *          2  :  alias_list = Custom alias list, or NULL for none.
 *          3  :  cur_action = Where to store the action.  Caller
 *                             allocates memory.
 *
 * Returns     :  JB_ERR_OK => Ok
 *                JB_ERR_PARSE => Parse error (line was trashed anyway)
 *                nonzero => Out of memory (line was trashed anyway)
 *
 *********************************************************************/
jb_err get_actions(char *line,
                   struct action_alias * alias_list,
                   struct action_spec *cur_action)
{
   jb_err err;
   init_action(cur_action);
   cur_action->mask = ACTION_MASK_ALL;

   while (line)
   {
      char * option = NULL;
      char * value = NULL;

      err = get_action_token(&line, &option, &value);
      if (err)
      {
         return err;
      }

      if (option)
      {
         /* handle option in 'option' */

         /* Check for standard action name */
         const struct action_name * action = action_names;

         while ( (action->name != NULL) && (0 != strcmpic(action->name, option)) )
         {
            action++;
         }
         if (action->name != NULL)
         {
            /* Found it */
            cur_action->mask &= action->mask;
            cur_action->add  &= action->mask;
            cur_action->add  |= action->add;

            switch (action->takes_value)
            {
            case AV_NONE:
               /* ignore any option. */
               break;
            case AV_ADD_STRING:
               {
                  /* add single string. */

                  if ((value == NULL) || (*value == '\0'))
                  {
                     if (0 != strcmpic(action->name, "block"))
                     {
                        /*
                         * XXX: Temporary backwards compatibility hack.
                         * XXX: should include line number.
                         */
                        value = "No reason specified.";
                        log_error(LOG_LEVEL_ERROR,
                           "block action without reason found. This may "
                           "become a fatal error in future versions.");
                     }
                     else
                     {
                        return JB_ERR_PARSE;
                     }
                  }
                  /* FIXME: should validate option string here */
                  freez (cur_action->string[action->index]);
                  cur_action->string[action->index] = strdup(value);
                  if (NULL == cur_action->string[action->index])
                  {
                     return JB_ERR_MEMORY;
                  }
                  break;
               }
            case AV_REM_STRING:
               {
                  /* remove single string. */

                  freez (cur_action->string[action->index]);
                  break;
               }
            case AV_ADD_MULTI:
               {
                  /* append multi string. */

                  struct list * remove_p = cur_action->multi_remove[action->index];
                  struct list * add_p    = cur_action->multi_add[action->index];

                  if ((value == NULL) || (*value == '\0'))
                  {
                     return JB_ERR_PARSE;
                  }

                  list_remove_item(remove_p, value);
                  err = enlist_unique(add_p, value, 0);
                  if (err)
                  {
                     return err;
                  }
                  break;
               }
            case AV_REM_MULTI:
               {
                  /* remove multi string. */

                  struct list * remove_p = cur_action->multi_remove[action->index];
                  struct list * add_p    = cur_action->multi_add[action->index];

                  if ( (value == NULL) || (*value == '\0')
                     || ((*value == '*') && (value[1] == '\0')) )
                  {
                     /*
                      * no option, or option == "*".
                      *
                      * Remove *ALL*.
                      */
                     list_remove_all(remove_p);
                     list_remove_all(add_p);
                     cur_action->multi_remove_all[action->index] = 1;
                  }
                  else
                  {
                     /* Valid option - remove only 1 option */

                     if ( !cur_action->multi_remove_all[action->index] )
                     {
                        /* there isn't a catch-all in the remove list already */
                        err = enlist_unique(remove_p, value, 0);
                        if (err)
                        {
                           return err;
                        }
                     }
                     list_remove_item(add_p, value);
                  }
                  break;
               }
            default:
               /* Shouldn't get here unless there's memory corruption. */
               assert(0);
               return JB_ERR_PARSE;
            }
         }
         else
         {
            /* try user aliases. */
            const struct action_alias * alias = alias_list;

            while ( (alias != NULL) && (0 != strcmpic(alias->name, option)) )
            {
               alias = alias->next;
            }
            if (alias != NULL)
            {
               /* Found it */
               merge_actions(cur_action, alias->action);
            }
            else if (((size_t)2 < strlen(option)) && action_used_to_be_valid(option+1))
            {
               log_error(LOG_LEVEL_ERROR, "Action '%s' is no longer valid "
                  "in this pAds release. Ignored.", option+1);
            }
            else if (((size_t)2 < strlen(option)) && 0 == strcmpic(option+1, "hide-forwarded-for-headers"))
            {
               log_error(LOG_LEVEL_FATAL, "The action 'hide-forwarded-for-headers' "
                  "is no longer valid in this pAds release. "
                  "Use 'change-x-forwarded-for' instead.");
            }
            else
            {
               /* Bad action name */
               /*
                * XXX: This is a fatal error and pAds will later on exit
                * in load_one_actions_file() because of an "invalid line".
                *
                * It would be preferable to name the offending option in that
                * error message, but currently there is no way to do that and
                * we have to live with two error messages for basically the
                * same reason.
                */
               log_error(LOG_LEVEL_ERROR, "Unknown action or alias: %s", option);
               return JB_ERR_PARSE;
            }
         }
      }
   }

   return JB_ERR_OK;
}


/*********************************************************************
 *
 * Function    :  init_current_action
 *
 * Description :  Zero out an action.
 *
 * Parameters  :
 *          1  :  dest = An uninitialized current_action_spec.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void init_current_action (struct current_action_spec *dest)
{
   memset(dest, '\0', sizeof(*dest));

   dest->flags = ACTION_MOST_COMPATIBLE;
}


/*********************************************************************
 *
 * Function    :  init_action
 *
 * Description :  Zero out an action.
 *
 * Parameters  :
 *          1  :  dest = An uninitialized action_spec.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void init_action (struct action_spec *dest)
{
   memset(dest, '\0', sizeof(*dest));
}


/*********************************************************************
 *
 * Function    :  merge_current_action
 *
 * Description :  Merge two actions together.
 *                Similar to "dest += src".
 *                Differences between this and merge_actions()
 *                is that this one doesn't allocate memory for
 *                strings (so "src" better be in memory for at least
 *                as long as "dest" is, and you'd better free
 *                "dest" using "free_current_action").
 *                Also, there is no  mask or remove lists in dest.
 *                (If we're applying it to a URL, we don't need them)
 *
 * Parameters  :
 *          1  :  dest = Current actions, to modify.
 *          2  :  src = Action to add.
 *
 * Returns  0  :  no error
 *        !=0  :  error, probably JB_ERR_MEMORY.
 *
 *********************************************************************/
jb_err merge_current_action (struct current_action_spec *dest,
                             const struct action_spec *src)
{
   int i;
   jb_err err = JB_ERR_OK;

   dest->flags  &= src->mask;
   dest->flags  |= src->add;

   for (i = 0; i < ACTION_STRING_COUNT; i++)
   {
      char * str = src->string[i];
      if (str)
      {
         str = strdup(str);
         if (!str)
         {
            return JB_ERR_MEMORY;
         }
         freez(dest->string[i]);
         dest->string[i] = str;
      }
   }

   for (i = 0; i < ACTION_MULTI_COUNT; i++)
   {
      if (src->multi_remove_all[i])
      {
         /* Remove everything from dest, then add src->multi_add */
         err = list_duplicate(dest->multi[i], src->multi_add[i]);
         if (err)
         {
            return err;
         }
      }
      else
      {
         list_remove_list(dest->multi[i], src->multi_remove[i]);
         err = list_append_list_unique(dest->multi[i], src->multi_add[i]);
         if (err)
         {
            return err;
         }
      }
   }
   return err;
}

#if 0
/*********************************************************************
 *
 * Function    :  update_action_bits_for_all_tags
 *
 * Description :  Updates the action bits based on all matching tags.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  0 if no tag matched, or
 *                1 otherwise
 *
 *********************************************************************/
int update_action_bits_for_all_tags(struct client_state *csp)
{
   struct list_entry *tag;
   int updated = 0;

   for (tag = csp->tags->first; tag != NULL; tag = tag->next)
   {
      if (update_action_bits_for_tag(csp, tag->str))
      {
         updated = 1;
      }
   }

   return updated;
}
#endif

/*********************************************************************
 *
 * Function    :  update_action_bits_for_tag
 *
 * Description :  Updates the action bits based on the action sections
 *                whose tag patterns match a provided tag.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  tag = The tag on which the update should be based on
 *
 * Returns     :  0 if no tag matched, or
 *                1 otherwise
 *
 *********************************************************************/
int update_action_bits_for_tag(struct client_state *csp, const char *tag)
{
   struct file_list *fl;
   struct url_actions *b;

   int updated = 0;
   int i;

   assert(tag);
   assert(list_contains_item(csp->tags, tag));

   /* Run through all action files, */
   for (i = 0; i < MAX_AF_FILES; i++)
   {
      if (((fl = csp->actions_list[i]) == NULL) || ((b = fl->f) == NULL))
      {
         /* Skip empty files */
         continue;
      }

      /* and through all the action patterns, */
      for (b = b->next; NULL != b; b = b->next)
      {
         /* skip the URL patterns, */
         if (NULL == b->url->tag_regex)
         {
            continue;
         }

         /* and check if one of the tag patterns matches the tag, */
         if (0 == regexec(b->url->tag_regex, tag, 0, NULL, 0))
         {
            /* if it does, update the action bit map, */
            if (merge_current_action(csp->action, b->action))
            {
               log_error(LOG_LEVEL_ERROR,
                  "Out of memory while changing action bits");
            }
            /* and signal the change. */
            updated = 1;
         }
      }
   }

   return updated;
}


/*********************************************************************
 *
 * Function    :  free_current_action
 *
 * Description :  Free memory used by a current_action_spec.
 *                Does not free the current_action_spec itself.
 *
 * Parameters  :
 *          1  :  src = Source to free.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void free_current_action(struct current_action_spec *src)
{
   int i;

   for (i = 0; i < ACTION_STRING_COUNT; i++)
   {
      freez(src->string[i]);
   }

   for (i = 0; i < ACTION_MULTI_COUNT; i++)
   {
      destroy_list(src->multi[i]);
   }

   memset(src, '\0', sizeof(*src));
}


static struct file_list *current_actions_file[MAX_AF_FILES]  = {
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL
};


#ifdef FEATURE_GRACEFUL_TERMINATION
/*********************************************************************
 *
 * Function    :  unload_current_actions_file
 *
 * Description :  Unloads current actions file - reset to state at
 *                beginning of program.
 *
 * Parameters  :  None
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void unload_current_actions_file(void)
{
   int i;

   for (i = 0; i < MAX_AF_FILES; i++)
   {
      if (current_actions_file[i])
      {
         current_actions_file[i]->unloader = unload_actions_file;
         current_actions_file[i] = NULL;
      }
   }
}
#endif /* FEATURE_GRACEFUL_TERMINATION */


/*********************************************************************
 *
 * Function    :  unload_actions_file
 *
 * Description :  Unloads an actions module.
 *
 * Parameters  :
 *          1  :  file_data = the data structure associated with the
 *                            actions file.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void unload_actions_file(void *file_data)
{
   struct url_actions * next;
   struct url_actions * cur = (struct url_actions *)file_data;
   while (cur != NULL)
   {
      next = cur->next;
      free_url_spec(cur->url);
      if ((next == NULL) || (next->action != cur->action))
      {
         /*
          * As the action settings might be shared,
          * we can only free them if the current
          * url pattern is the last one, or if the
          * next one is using different settings.
          */
         free_action_spec(cur->action);
      }
      freez(cur);
      cur = next;
   }
}


/*********************************************************************
 *
 * Function    :  free_alias_list
 *
 * Description :  Free memory used by a list of aliases.
 *
 * Parameters  :
 *          1  :  alias_list = Linked list to free.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void free_alias_list(struct action_alias *alias_list)
{
   while (alias_list != NULL)
   {
      struct action_alias * next = alias_list->next;
      alias_list->next = NULL;
      freez(alias_list->name);
      free_action(alias_list->action);
      free(alias_list);
      alias_list = next;
   }
}


/*********************************************************************
 *
 * Function    :  load_action_files
 *
 * Description :  Read and parse all the action files and add to files
 *                list.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  0 => Ok, everything else is an error.
 *
 *********************************************************************/
int load_action_files(struct client_state *csp)
{
   int i;
   int result;

   for (i = 0; i < MAX_AF_FILES; i++)
   {
      if (csp->config->actions_file[i])
      {
         result = load_one_actions_file(csp, i);
         if (result)
         {
            return result;
         }
      }
      else if (current_actions_file[i])
      {
         current_actions_file[i]->unloader = unload_actions_file;
         current_actions_file[i] = NULL;
      }
   }

   return 0;
}

/*********************************************************************
 *
 * Function    :  load_one_actions_file
 *
 * Description :  Read and parse a action file and add to files
 *                list.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  fileid = File index to load.
 *
 * Returns     :  0 => Ok, everything else is an error.
 *
 *********************************************************************/
static int load_one_actions_file(struct client_state *csp, int fileid)
{

   /*
    * Parser mode.
    * Note: Keep these in the order they occur in the file, they are
    * sometimes tested with <=
    */
#define MODE_START_OF_FILE 1
#define MODE_SETTINGS      2
#define MODE_DESCRIPTION   3
#define MODE_ALIAS         4
#define MODE_ACTIONS       5

   int mode = MODE_START_OF_FILE;

   FILE *fp;
   struct url_actions *last_perm;
   struct url_actions *perm;
   char  buf[BUFFER_SIZE];
   struct file_list *fs;
   struct action_spec * cur_action = NULL;
   int cur_action_used = 0;
   struct action_alias * alias_list = NULL;
   unsigned long linenum = 0;

   if (!check_file_changed(current_actions_file[fileid], csp->config->actions_file[fileid], &fs))
   {
      /* No need to load */
      csp->actions_list[fileid] = current_actions_file[fileid];
      return 0;
   }
   if (!fs)
   {
      log_error(LOG_LEVEL_FATAL, "can't load actions file '%s': %E. "
         "Note that beginning with pAds 3.0.7, actions files have to be specified "
         "with their complete file names.", csp->config->actions_file[fileid]);
      return 1; /* never get here */
   }

   fs->f = last_perm = (struct url_actions *)zalloc(sizeof(*last_perm));
   if (last_perm == NULL)
   {
      log_error(LOG_LEVEL_FATAL, "can't load actions file '%s': out of memory!",
                csp->config->actions_file[fileid]);
      return 1; /* never get here */
   }

   fp = fopen(csp->config->actions_file[fileid], "r");
#if 0
   if (fp == NULL)
   {
      log_error(LOG_LEVEL_FATAL, "can't load actions file '%s': error opening file: %E",
                csp->config->actions_file[fileid]);
      return 1; /* never get here */
   }
#endif

   log_error(LOG_LEVEL_INFO, "Loading actions file: %s", csp->config->actions_file[fileid]);

   while (read_config_line(buf, sizeof(buf), fp, &linenum, csp->config->actions_file[fileid]) != NULL)
   {
      if (*buf == '{')
      {
         /* It's a header block */
         if (buf[1] == '{')
         {
            /* It's {{settings}} or {{alias}} */
            size_t len = strlen(buf);
            char * start = buf + 2;
            char * end = buf + len - 1;
            if ((len < (size_t)5) || (*end-- != '}') || (*end-- != '}'))
            {
               /* too short */
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                  "can't load actions file '%s': invalid line (%lu): %s", 
                  csp->config->actions_file[fileid], linenum, buf);
               return 1; /* never get here */
            }

            /* Trim leading and trailing whitespace. */
            end[1] = '\0';
            chomp(start);

            if (*start == '\0')
            {
               /* too short */
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                  "can't load actions file '%s': invalid line (%lu): {{ }}",
                  csp->config->actions_file[fileid], linenum);
               return 1; /* never get here */
            }

            /*
             * An actionsfile can optionally contain the following blocks.
             * They *MUST* be in this order, to simplify processing:
             *
             * {{settings}}
             * name=value...
             *
             * {{description}}
             * ...free text, format TBD, but no line may start with a '{'...
             *
             * {{alias}}
             * name=actions...
             *
             * The actual actions must be *after* these special blocks.
             * None of these special blocks may be repeated.
             *
             */
            if (0 == strcmpic(start, "settings"))
            {
               /* it's a {{settings}} block */
               if (mode >= MODE_SETTINGS)
               {
                  /* {{settings}} must be first thing in file and must only
                   * appear once.
                   */
                  if (fp) fclose(fp);
                  log_error(LOG_LEVEL_FATAL,
                     "can't load actions file '%s': line %lu: {{settings}} must only appear once, and it must be before anything else.",
                     csp->config->actions_file[fileid], linenum);
               }
               mode = MODE_SETTINGS;
            }
            else if (0 == strcmpic(start, "description"))
            {
               /* it's a {{description}} block */
               if (mode >= MODE_DESCRIPTION)
               {
                  /* {{description}} is a singleton and only {{settings}} may proceed it
                   */
                  if (fp) fclose(fp);
                  log_error(LOG_LEVEL_FATAL,
                     "can't load actions file '%s': line %lu: {{description}} must only appear once, and only a {{settings}} block may be above it.",
                     csp->config->actions_file[fileid], linenum);
               }
               mode = MODE_DESCRIPTION;
            }
            else if (0 == strcmpic(start, "alias"))
            {
               /* it's an {{alias}} block */
               if (mode >= MODE_ALIAS)
               {
                  /* {{alias}} must be first thing in file, possibly after
                   * {{settings}} and {{description}}
                   *
                   * {{alias}} must only appear once.
                   *
                   * Note that these are new restrictions introduced in
                   * v2.9.10 in order to make actionsfile editing simpler.
                   * (Otherwise, reordering actionsfile entries without
                   * completely rewriting the file becomes non-trivial)
                   */
                  if (fp) fclose(fp);
                  log_error(LOG_LEVEL_FATAL,
                     "can't load actions file '%s': line %lu: {{alias}} must only appear once, and it must be before all actions.",
                     csp->config->actions_file[fileid], linenum);
               }
               mode = MODE_ALIAS;
            }
            else
            {
               /* invalid {{something}} block */
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                  "can't load actions file '%s': invalid line (%lu): {{%s}}",
                  csp->config->actions_file[fileid], linenum, start);
               return 1; /* never get here */
            }
         }
         else
         {
            /* It's an actions block */

            char  actions_buf[BUFFER_SIZE];
            char * end;

            /* set mode */
            mode    = MODE_ACTIONS;

            /* free old action */
            if (cur_action)
            {
               if (!cur_action_used)
               {
                  free_action_spec(cur_action);
               }
               cur_action = NULL;
            }
            cur_action_used = 0;
            cur_action = (struct action_spec *)zalloc(sizeof(*cur_action));
            if (cur_action == NULL)
            {
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                  "can't load actions file '%s': out of memory",
                  csp->config->actions_file[fileid]);
               return 1; /* never get here */
            }
            init_action(cur_action);

            /* trim { */
            strlcpy(actions_buf, buf + 1, sizeof(actions_buf));

            /* check we have a trailing } and then trim it */
            end = actions_buf + strlen(actions_buf) - 1;
            if (*end != '}')
            {
               /* No closing } */
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                  "can't load actions file '%s': invalid line (%lu): %s",
                  csp->config->actions_file[fileid], linenum, buf);
               return 1; /* never get here */
            }
            *end = '\0';

            /* trim any whitespace immediately inside {} */
            chomp(actions_buf);

            if (get_actions(actions_buf, alias_list, cur_action))
            {
               /* error */
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                  "can't load actions file '%s': invalid line (%lu): %s",
                  csp->config->actions_file[fileid], linenum, buf);
               return 1; /* never get here */
            }
         }
      }
      else if (mode == MODE_SETTINGS)
      {
         /*
          * Part of the {{settings}} block.
          * For now only serves to check if the file's minimum pAds
          * version requirement is met, but we may want to read & check
          * permissions when we go multi-user.
          */
         if (!strncmp(buf, "for-pads-version=", 17))
         {
            char *version_string, *fields[3];
            int num_fields;

            if ((version_string = strdup(buf + 17)) == NULL)
            {
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                         "can't load actions file '%s': out of memory!",
                         csp->config->actions_file[fileid]);
               return 1; /* never get here */
            }
            
            num_fields = ssplit(version_string, ".", fields, 3, TRUE, FALSE);

            if (num_fields < 1 || atoi(fields[0]) == 0)
            {
               log_error(LOG_LEVEL_ERROR,
                 "While loading actions file '%s': invalid line (%lu): %s",
                  csp->config->actions_file[fileid], linenum, buf);
            }
            else if (                      atoi(fields[0]) > VERSION_MAJOR
                     || (num_fields > 1 && atoi(fields[1]) > VERSION_MINOR)
                     || (num_fields > 2 && atoi(fields[2]) > VERSION_POINT))
            {
               if (fp) fclose(fp);
               log_error(LOG_LEVEL_FATAL,
                         "Actions file '%s', line %lu requires newer pAds version: %s",
                         csp->config->actions_file[fileid], linenum, buf );
               return 1; /* never get here */
            }
            free(version_string);
         }
      }
      else if (mode == MODE_DESCRIPTION)
      {
         /*
          * Part of the {{description}} block.
          * Ignore for now.
          */
      }
      else if (mode == MODE_ALIAS)
      {
         /*
          * define an alias
          */
         char  actions_buf[BUFFER_SIZE];
         struct action_alias * new_alias;

         char * start = strchr(buf, '=');
         char * end = start;

         if ((start == NULL) || (start == buf))
         {
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': invalid alias line (%lu): %s",
               csp->config->actions_file[fileid], linenum, buf);
            return 1; /* never get here */
         }

         if ((new_alias = zalloc(sizeof(*new_alias))) == NULL)
         {
            if (fp) fclose(fp);
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': out of memory!",
               csp->config->actions_file[fileid]);
            return 1; /* never get here */
         }

         /* Eat any the whitespace before the '=' */
         end--;
         while ((*end == ' ') || (*end == '\t'))
         {
            /*
             * we already know we must have at least 1 non-ws char
             * at start of buf - no need to check
             */
            end--;
         }
         end[1] = '\0';

         /* Eat any the whitespace after the '=' */
         start++;
         while ((*start == ' ') || (*start == '\t'))
         {
            start++;
         }
         if (*start == '\0')
         {
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': invalid alias line (%lu): %s",
               csp->config->actions_file[fileid], linenum, buf);
            return 1; /* never get here */
         }

         if ((new_alias->name = strdup(buf)) == NULL)
         {
            if (fp) fclose(fp);
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': out of memory!",
               csp->config->actions_file[fileid]);
            return 1; /* never get here */
         }

         strlcpy(actions_buf, start, sizeof(actions_buf));

         if (get_actions(actions_buf, alias_list, new_alias->action))
         {
            /* error */
            if (fp) fclose(fp);
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': invalid alias line (%lu): %s = %s",
               csp->config->actions_file[fileid], linenum, buf, start);
            return 1; /* never get here */
         }

         /* add to list */
         new_alias->next = alias_list;
         alias_list = new_alias;
      }
      else if (mode == MODE_ACTIONS)
      {
         /* it's a URL pattern */

         /* allocate a new node */
         if ((perm = zalloc(sizeof(*perm))) == NULL)
         {
            if (fp) fclose(fp);
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': out of memory!",
               csp->config->actions_file[fileid]);
            return 1; /* never get here */
         }

         perm->action = cur_action;
         cur_action_used = 1;

         /* Save the URL pattern */
         if (create_url_spec(perm->url, buf))
         {
            if (fp) fclose(fp);
            log_error(LOG_LEVEL_FATAL,
               "can't load actions file '%s': line %lu: cannot create URL pattern from: %s",
               csp->config->actions_file[fileid], linenum, buf);
            return 1; /* never get here */
         }

         /* add it to the list */
         last_perm->next = perm;
         last_perm = perm;
      }
      else if (mode == MODE_START_OF_FILE)
      {
         /* oops - please have a {} line as 1st line in file. */
         if (fp) fclose(fp);
         log_error(LOG_LEVEL_FATAL,
            "can't load actions file '%s': first needed line (%lu) is invalid: %s",
            csp->config->actions_file[fileid], linenum, buf);
         return 1; /* never get here */
      }
      else
      {
         /* How did we get here? This is impossible! */
         if (fp) fclose(fp);
         log_error(LOG_LEVEL_FATAL,
            "can't load actions file '%s': INTERNAL ERROR - mode = %d",
            csp->config->actions_file[fileid], mode);
         return 1; /* never get here */
      }
   }

   if (fp) fclose(fp);

   if (!cur_action_used)
   {
      free_action_spec(cur_action);
   }
   free_alias_list(alias_list);

   /* the old one is now obsolete */
   if (current_actions_file[fileid])
   {
      current_actions_file[fileid]->unloader = unload_actions_file;
   }

   fs->next    = files->next;
   files->next = fs;
   current_actions_file[fileid] = fs;

   csp->actions_list[fileid] = fs;

   return(0);

}


/*********************************************************************
 *
 * Function    :  actions_to_text
 *
 * Description :  Converts a actionsfile entry from the internal
 *                structure into a text line.  The output is split
 *                into one line for each action with line continuation. 
 *
 * Parameters  :
 *          1  :  action = The action to format.
 *
 * Returns     :  A string.  Caller must free it.
 *                NULL on out-of-memory error.
 *
 *********************************************************************/
char * actions_to_text(const struct action_spec *action)
{
   unsigned long mask = action->mask;
   unsigned long add  = action->add;
   char *result = strdup("");
   struct list_entry * lst;

   /* sanity - prevents "-feature +feature" */
   mask |= add;


#define DEFINE_ACTION_BOOL(__name, __bit)          \
   if (!(mask & __bit))                            \
   {                                               \
      string_append(&result, " -" __name " \\\n"); \
   }                                               \
   else if (add & __bit)                           \
   {                                               \
      string_append(&result, " +" __name " \\\n"); \
   }

#define DEFINE_ACTION_STRING(__name, __bit, __index)   \
   if (!(mask & __bit))                                \
   {                                                   \
      string_append(&result, " -" __name " \\\n");     \
   }                                                   \
   else if (add & __bit)                               \
   {                                                   \
      string_append(&result, " +" __name "{");         \
      string_append(&result, action->string[__index]); \
      string_append(&result, "} \\\n");                \
   }

#define DEFINE_ACTION_MULTI(__name, __index)         \
   if (action->multi_remove_all[__index])            \
   {                                                 \
      string_append(&result, " -" __name " \\\n");   \
   }                                                 \
   else                                              \
   {                                                 \
      lst = action->multi_remove[__index]->first;    \
      while (lst)                                    \
      {                                              \
         string_append(&result, " -" __name "{");    \
         string_append(&result, lst->str);           \
         string_append(&result, "} \\\n");           \
         lst = lst->next;                            \
      }                                              \
   }                                                 \
   lst = action->multi_add[__index]->first;          \
   while (lst)                                       \
   {                                                 \
      string_append(&result, " +" __name "{");       \
      string_append(&result, lst->str);              \
      string_append(&result, "} \\\n");              \
      lst = lst->next;                               \
   }

#define DEFINE_ACTION_ALIAS 0 /* No aliases for output */

#include "actionlist.h"

#undef DEFINE_ACTION_MULTI
#undef DEFINE_ACTION_STRING
#undef DEFINE_ACTION_BOOL
#undef DEFINE_ACTION_ALIAS

   return result;
}


/*********************************************************************
 *
 * Function    :  actions_to_html
 *
 * Description :  Converts a actionsfile entry from numeric form
 *                ("mask" and "add") to a <br>-seperated HTML string
 *                in which each action is linked to its chapter in
 *                the user manual.
 *
 * Parameters  :
 *          1  :  csp    = Client state (for config)
 *          2  :  action = Action spec to be converted
 *
 * Returns     :  A string.  Caller must free it.
 *                NULL on out-of-memory error.
 *
 *********************************************************************/
char * actions_to_html(const struct client_state *csp,
                       const struct action_spec *action)
{
   unsigned long mask = action->mask;
   unsigned long add  = action->add;
   char *result = strdup("");
   struct list_entry * lst;

   /* sanity - prevents "-feature +feature" */
   mask |= add;


#define DEFINE_ACTION_BOOL(__name, __bit)       \
   if (!(mask & __bit))                         \
   {                                            \
      string_append(&result, "\n<br>-");        \
      string_join(&result, add_help_link(__name, csp->config)); \
   }                                            \
   else if (add & __bit)                        \
   {                                            \
      string_append(&result, "\n<br>+");        \
      string_join(&result, add_help_link(__name, csp->config)); \
   }

#define DEFINE_ACTION_STRING(__name, __bit, __index) \
   if (!(mask & __bit))                              \
   {                                                 \
      string_append(&result, "\n<br>-");             \
      string_join(&result, add_help_link(__name, csp->config)); \
   }                                                 \
   else if (add & __bit)                             \
   {                                                 \
      string_append(&result, "\n<br>+");             \
      string_join(&result, add_help_link(__name, csp->config)); \
      string_append(&result, "{");                   \
      string_join(&result, html_encode(action->string[__index])); \
      string_append(&result, "}");                   \
   }

#define DEFINE_ACTION_MULTI(__name, __index)          \
   if (action->multi_remove_all[__index])             \
   {                                                  \
      string_append(&result, "\n<br>-");              \
      string_join(&result, add_help_link(__name, csp->config)); \
   }                                                  \
   else                                               \
   {                                                  \
      lst = action->multi_remove[__index]->first;     \
      while (lst)                                     \
      {                                               \
         string_append(&result, "\n<br>-");           \
         string_join(&result, add_help_link(__name, csp->config)); \
         string_append(&result, "{");                 \
         string_join(&result, html_encode(lst->str)); \
         string_append(&result, "}");                 \
         lst = lst->next;                             \
      }                                               \
   }                                                  \
   lst = action->multi_add[__index]->first;           \
   while (lst)                                        \
   {                                                  \
      string_append(&result, "\n<br>+");              \
      string_join(&result, add_help_link(__name, csp->config)); \
      string_append(&result, "{");                    \
      string_join(&result, html_encode(lst->str));    \
      string_append(&result, "}");                    \
      lst = lst->next;                                \
   }

#define DEFINE_ACTION_ALIAS 0 /* No aliases for output */

#include "actionlist.h"

#undef DEFINE_ACTION_MULTI
#undef DEFINE_ACTION_STRING
#undef DEFINE_ACTION_BOOL
#undef DEFINE_ACTION_ALIAS

   /* trim leading <br> */
   if (result && *result)
   {
      char * s = result;
      result = strdup(result + 5);
      free(s);
   }

   return result;
}


/*********************************************************************
 *
 * Function    :  current_actions_to_html
 *
 * Description :  Converts a curren action spec to a <br> seperated HTML
 *                text in which each action is linked to its chapter in
 *                the user manual.
 *
 * Parameters  :
 *          1  :  csp    = Client state (for config) 
 *          2  :  action = Current action spec to be converted
 *
 * Returns     :  A string.  Caller must free it.
 *                NULL on out-of-memory error.
 *
 *********************************************************************/
char *current_action_to_html(const struct client_state *csp,
                             const struct current_action_spec *action)
{
   unsigned long flags  = action->flags;
   struct list_entry * lst;
   char *result   = strdup("");
   char *active   = strdup("");
   char *inactive = strdup("");

#define DEFINE_ACTION_BOOL(__name, __bit)  \
   if (flags & __bit)                      \
   {                                       \
      string_append(&active, "\n<br>+");   \
      string_join(&active, add_help_link(__name, csp->config)); \
   }                                       \
   else                                    \
   {                                       \
      string_append(&inactive, "\n<br>-"); \
      string_join(&inactive, add_help_link(__name, csp->config)); \
   }

#define DEFINE_ACTION_STRING(__name, __bit, __index)   \
   if (flags & __bit)                                  \
   {                                                   \
      string_append(&active, "\n<br>+");               \
      string_join(&active, add_help_link(__name, csp->config)); \
      string_append(&active, "{");                     \
      string_join(&active, html_encode(action->string[__index])); \
      string_append(&active, "}");                     \
   }                                                   \
   else                                                \
   {                                                   \
      string_append(&inactive, "\n<br>-");             \
      string_join(&inactive, add_help_link(__name, csp->config)); \
   }

#define DEFINE_ACTION_MULTI(__name, __index)           \
   lst = action->multi[__index]->first;                \
   if (lst == NULL)                                    \
   {                                                   \
      string_append(&inactive, "\n<br>-");             \
      string_join(&inactive, add_help_link(__name, csp->config)); \
   }                                                   \
   else                                                \
   {                                                   \
      while (lst)                                      \
      {                                                \
         string_append(&active, "\n<br>+");            \
         string_join(&active, add_help_link(__name, csp->config)); \
         string_append(&active, "{");                  \
         string_join(&active, html_encode(lst->str));  \
         string_append(&active, "}");                  \
         lst = lst->next;                              \
      }                                                \
   }

#define DEFINE_ACTION_ALIAS 0 /* No aliases for output */

#include "actionlist.h"

#undef DEFINE_ACTION_MULTI
#undef DEFINE_ACTION_STRING
#undef DEFINE_ACTION_BOOL
#undef DEFINE_ACTION_ALIAS

   if (active != NULL)
   {
      string_append(&result, active);
      freez(active);
   }
   string_append(&result, "\n<br>");
   if (inactive != NULL)
   {
      string_append(&result, inactive);
      freez(inactive);
   }
   return result;
}