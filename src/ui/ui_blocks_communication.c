#include "ui_main_window.h"

void
ui_part_back(App_Data *ap)
{
   if (ap->ws->groupspace)
     {
        ui_groupspace_unset(ap->ws->groupspace);
        ui_object_highlight_hide (ap->ws);
     }
  else
     WARN ("Groupspace object always delete");

  elm_genlist_clear(ui_block_state_list_get(ap));
  evas_object_hide(ui_property_part_view_get(ui_block_property_get(ap)));
}

void
ui_group_back(App_Data *ap)
{
   elm_genlist_clear(ui_block_signal_list_get(ap));
   evas_object_hide(ui_property_group_view_get(ui_block_property_get(ap)));
}

void
ui_state_select(App_Data *ap, Elm_Object_Item *glit, Evas_Object *obj)
{
   Part_State *state = NULL;
   Part *part = NULL;
   Evas_Object *prop_view, *part_view, *state_view;

   state = elm_object_item_data_get(glit);

   prop_view = ui_block_property_get(ap);
   part_view = ui_property_part_view_get(prop_view);
   state_view = ui_prop_part_info_state_view_add(part_view, state);
   ui_prop_part_info_state_set(part_view, state_view);
   part = ui_state_list_part_get(obj);
   ui_groupspace_part_state_update(part, state, ap->ws->groupspace);
   ui_object_highlight_set(ap->ws, part->obj);
   evas_object_show(state_view);
}

Evas_Object *
ui_part_select(App_Data *ap, Part* part)
{
   Evas_Object *prop = NULL;
   Evas_Object *part_prop = NULL;
   Evas_Object *gl_states = NULL;

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

   part_prop = ui_prop_part_info_view_add(prop, part);
   ui_property_part_view_set(prop, part_prop);
   evas_object_show(part_prop);

   gl_states = ui_states_list_add(ap->win);
   ui_states_list_data_set(gl_states, part);
   ui_block_state_list_set(ap, gl_states);
   evas_object_show(gl_states);

   /* FIXME: it bad */
   elm_genlist_item_selected_set(elm_genlist_first_item_get(gl_states), EINA_TRUE);
   ui_object_highlight_set(ap->ws, part->obj);

   return gl_states;
}

void
ui_group_clicked(App_Data *ap, Group *group)
{
   Evas_Object *gl_signals = NULL;
   Evas_Object *prop = NULL;
   Evas_Object *group_prop = NULL;
   Eina_List *signals = NULL;

   /* Get signals list of a group and show them */
   signals = wm_program_signals_list_get(group->programs);
   gl_signals = ui_signal_list_add(ap->win);
   ui_signal_list_data_set(gl_signals, signals);
   wm_program_signals_list_free(signals);
   ui_block_signal_list_set(ap, gl_signals);

   /* group properties */
   prop = ui_block_property_get(ap);
   if (prop)
     {
        ui_prop_group_info_view_update(prop, group);
        evas_object_show(prop);
     }
   else
     {
        prop = ui_property_view_new(ap->win);
        ui_block_property_set(ap, prop);
        evas_object_show(prop);

        group_prop = ui_prop_group_info_view_add(prop, group);
        ui_property_group_view_set(prop, group_prop);
        evas_object_show(group_prop);
     }

   ui_groupspace_set (ap->ws, ap->project, group);
   ui_groupspace_update (ap->ws->groupspace);
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
             NOTIFY_INFO(ap->win, 3, "Select file: %s", selected);
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
             NOTIFY_ERROR(ap->win, "The file must have a extension '.edj'");
          }
     }
   else
     ui_panes_hide(ap);

   evas_object_hide(elm_object_parent_widget_get(obj));
   evas_object_del(obj);
   return wd_list;
}

Evas_Object *
ui_edc_load_done(App_Data* ap, const char *project_name,
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
        NOTIFY_ERROR(ap->win, "The file must have a extension '.edc'");
     }

   evas_object_del(elm_object_content_get(ap->inwin));
   evas_object_hide(ap->inwin);

   return wd_list;
}
