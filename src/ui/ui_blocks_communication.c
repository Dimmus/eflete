/**
 * Edje Editor Tool
 * Copyright (C) 2013 Samsung Electronics.
 *
 * This file is part of Edje Editor Tool.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include "ui_main_window.h"

void
ui_part_back(App_Data *ap)
{
   Evas_Object *prop;
   if (ap->ws->groupspace)
     {
        ui_groupspace_unset(ap->ws->groupspace);
        ui_object_highlight_hide (ap->ws);
     }
   else
     WARN ("Groupspace object always delete");
   ap->ws->zoom_step = 1.0;
   elm_genlist_clear(ui_block_state_list_get(ap));
   elm_genlist_clear(ui_block_signal_list_get(ap));
   prop = ui_block_property_get(ap);
   ui_property_group_unset(prop);
}

/**
  * may be usable for future API.
  */
void
ui_group_back(App_Data *ap __UNUSED__)
{
}

void
ui_state_select(App_Data *ap,
                Evas_Object *obj,
                Eina_Stringshare *state)
{
   Part *part = NULL;
   Evas_Object *prop_view;

   prop_view = ui_block_property_get(ap);
   part = ui_state_list_part_get(obj);
   wm_part_current_state_set(part, state);
   ui_property_state_set(prop_view, part);
}

Evas_Object *
ui_part_select(App_Data *ap,
               Part* part)
{
   Evas_Object *prop;
   Evas_Object *gl_states;

   if (!part)
     {
        ERR("Coud not get acess to part object");
        return NULL;
     }
   prop = ui_block_property_get(ap);
   if (!prop)
     {
        ERR("Property view is missing!");
        return NULL;
     }

   ui_property_part_set(prop, part);

   gl_states = ui_states_list_add(ap->win);
   ui_states_list_data_set(gl_states, ap->project->current_group, part);
   ui_block_state_list_set(ap, gl_states);
   evas_object_show(gl_states);

   elm_genlist_item_selected_set(elm_genlist_first_item_get(gl_states), EINA_TRUE);
   ui_object_highlight_set(ap->ws, part);

   return gl_states;
}

void
ui_group_clicked(App_Data *ap, Group *group)
{
   Evas_Object *gl_signals = NULL;
   Evas_Object *prop = NULL;
   Eina_List *signals = NULL;

   /* Get signals list of a group and show them */
   gl_signals = ui_signal_list_add(ap->win);
   ui_signal_list_data_set(gl_signals, group);
   wm_program_signals_list_free(signals);
   ui_block_signal_list_set(ap, gl_signals);

   /* group properties */
   prop = ui_block_property_get(ap);
   if (prop)
     {
        ui_property_group_set(prop, group);
        evas_object_show(prop);
     }
   else
     {
        prop = ui_property_add(ap->win);
        ui_property_group_set(prop, group);
        ui_block_property_set(ap, prop);
        evas_object_show(prop);
     }

   ui_groupspace_set(ap->ws, ap->project, group);
   ui_groupspace_update(ap->ws->groupspace);
}

Evas_Object *
ui_edj_load_done(App_Data* ap, Evas_Object* obj, const char *selected)
{
  Evas_Object *wd_list = NULL;
  if (selected)
     {
        if (eina_str_has_suffix(selected, ".edj"))
          {
             INFO("Select file: %s", selected);
             NOTIFY_INFO(3, "Select file: %s", selected);
             ap->project = pm_open_project_edj(selected, selected);
             wd_list = ui_widget_list_add(ap->win);
             ui_widget_list_title_set(wd_list, ap->project->name);
             ui_widget_list_data_set(wd_list, ap->project);
             ui_block_widget_list_set(ap, wd_list);
             evas_object_show(wd_list);
             ui_panes_show(ap);
          }
        else
          {
             NOTIFY_ERROR("The file must have a extension '.edj'");
          }
     }
   evas_object_hide(elm_object_parent_widget_get(obj));
   evas_object_del(obj);
   return wd_list;
}

Evas_Object *
ui_edc_load_done(App_Data* ap,
                 const char *project_name,
                 const char *path_edc,
                 const char *path_id,
                 const char *path_sd,
                 const char *path_fd)
{
   Evas_Object *wd_list = NULL;

   if (eina_str_has_suffix(path_edc, ".edc"))
     {
        INFO("Select file: %s", path_edc);
        ap->project = pm_open_project_edc(project_name,
                                          path_edc,
                                          path_id,
                                          path_sd,
                                          path_fd);
        wd_list = ui_widget_list_add(ap->win);
        ui_widget_list_title_set(wd_list, ap->project->name);
        ui_widget_list_data_set(wd_list, ap->project);
        ui_block_widget_list_set(ap, wd_list);
        evas_object_show(wd_list);
        ui_panes_show(ap);
     }
   else
     {
        ERR("The file must have a extension '.edc'");
        NOTIFY_ERROR("The file must have a extension '.edc'");
     }

   evas_object_del(elm_object_content_get(ap->inwin));
   evas_object_hide(ap->inwin);

   return wd_list;
}


Eina_Bool
ui_part_state_delete(App_Data *ap)
{
   Evas_Object *state_list = NULL;
   Part *part = NULL;
   Group *group = NULL;
   Elm_Object_Item *eoi = NULL;
   char *full_state_name = NULL;
   char *state_name = NULL;
   char *state_value = NULL;
   double value = 0;

   if (!ap) return EINA_FALSE;

   state_list = ui_block_state_list_get(ap);
   part = ui_state_list_part_get(state_list);
   group = ui_groupspace_group_get(ap->ws->groupspace);

   eoi = elm_genlist_selected_item_get(state_list);
   if (!eoi)
     {
        NOTIFY_INFO(3, "Please select part state");
        return EINA_FALSE;
     }

   full_state_name = elm_object_item_data_get(eoi);
   if (!full_state_name) return EINA_FALSE;

   state_name = strdup(strtok(full_state_name, " "));
   state_value = strdup(strtok(NULL, " "));
   value = atof(state_value);

   if (!edje_edit_state_del(group->obj, part->name, state_name, value))
     {
        if (!strcmp(state_name, "default"))
          {
             NOTIFY_WARNING("Coud'nt delete default state");
          }
        else
          NOTIFY_WARNING("Failed delete state \n[%s %3.2f]", state_name, value);
        free(state_name);
        free(state_value);
        return EINA_FALSE;
     }
  elm_object_item_del(eoi);
  elm_genlist_item_selected_set(elm_genlist_first_item_get(state_list), EINA_TRUE);
  free(state_name);
  free(state_value);
  return EINA_TRUE;
}
