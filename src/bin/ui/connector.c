/*
 * Edje Theme Editor
 * Copyright (C) 2013-2014 Samsung Electronics.
 *
 * This file is part of Edje Theme Editor.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; If not, see www.gnu.org/licenses/lgpl.html.
 */

#include "main_window.h"
#include "preference.h"

#define makefile "#! bin/sh\nedje_cc -id ./images -fd ./fonts -sd ./sounds  "

static Elm_Entry_Filter_Accept_Set accept_name = {
   .accepted = NULL,
   .rejected = STYLE_NAME_BANNED_SYMBOLS
};

static Eina_Bool
_project_close_request(App_Data *ap, Eina_Bool changed);

static void
_add_part_dialog(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   part_dialog_add(ap);
}

static void
_add_part_item_dialog(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info)
{
   Part *part = (Part *)event_info;
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   item_dialog_add(ap, part);
}

static void
_on_ws_part_unselect(void *data,
                     Evas_Object *obj __UNUSED__,
                     void *event_info)
{
   App_Data *ap = (App_Data *)data;
   const char *part = (const char *)event_info;

   assert(ap != NULL);
   assert(part != NULL);

   ui_widget_list_part_selected_set(ui_block_widget_list_get(ap), part, false);
   ui_states_list_data_unset(ap->block.state_list);
   ui_property_part_unset(ui_block_property_get(ap));
}

static void
_del_part(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;
   Style *style = ap->project->current_style;
   char *part_name = NULL;
   Evas_Object *prop_view = NULL;

   assert(ap != NULL);
   assert(style != NULL);

   Part *part = ui_widget_list_selected_part_get(ui_block_widget_list_get(ap));
   if (!part)
     {
        NOTIFY_INFO(3, _("No part selected"));
        return;
     }
   part_name = strdup(part->name);

   /* In case when deleting part which is dragable area for another part,
      reloading style into liveview crash application */
   history_diff_add(style->obj, PART_TARGET, DEL, part_name);
   if (workspace_edit_object_part_del(ap->workspace, part->name))
     {
        live_view_part_del(ap->live_view, part);
        ui_widget_list_selected_part_del(ui_block_widget_list_get(ap), style);
     }

   if (!style->parts)
     {
        ui_signal_list_data_unset(ap->block.signal_list);
        _on_ws_part_unselect(ap, ap->workspace, part_name);
        workspace_highlight_unset(ap->workspace);
        /* Source code is updated on part selection.
           If this was last part we should update it manualy */
        prop_view = ui_block_property_get(ap);
        ui_property_code_of_group_setup(prop_view);
     }

   free(part_name);
   project_changed(true);
}

static void
_del_part_item(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info)
{
   Eina_Stringshare *item_name = (Eina_Stringshare *)event_info;
   App_Data *ap = (App_Data *)data;
   Evas_Object *widget_tabs = ui_block_widget_list_get(ap);

   assert(ap != NULL);
   assert(widget_tabs != NULL);

   Part *part = ui_widget_list_selected_part_get(widget_tabs);

   assert(part != NULL);

   if (workspace_edit_object_part_item_del(ap->workspace, part->name, item_name))
     {
        edje_edit_string_list_free(part->items);
        part->items = edje_edit_part_items_list_get(ap->project->current_style->obj, part->name);
        ui_widget_list_part_items_refresh(widget_tabs, part, false);
     }
   else
     {
        ERR("Failed to delete item")
        abort();
     }
   workspace_edit_object_recalc(ap->workspace);
   project_changed(true);
}

static void
_above_part(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   Eina_Inlist *tmp_list = NULL;
   App_Data *ap = (App_Data *)data;
   Style *style = ap->project->current_style;
   if (!ui_widget_list_selected_part_above(ui_block_widget_list_get(ap), style))
      return;
   Part *part = ui_widget_list_selected_part_get(ui_block_widget_list_get(ap));
   if (!part) return;
   history_diff_add(style->obj, PART_TARGET, RESTACK, part->name);
   if (!workspace_edit_object_part_above(ap->workspace, part->name))
     {
        NOTIFY_ERROR(_("Internal edje error occurred on part move"));
        ui_widget_list_selected_part_below(ui_block_widget_list_get(ap), style);
     }
   tmp_list = eina_inlist_find(style->parts, EINA_INLIST_GET(part));
   Part *rel = EINA_INLIST_CONTAINER_GET(tmp_list->next, Part);
   live_view_part_restack_above(ap->live_view, part, rel);
   project_changed(true);
}

static void
_below_part(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   Eina_Inlist *tmp_list = NULL;
   App_Data *ap = (App_Data *)data;
   Style *style = ap->project->current_style;
   if (!ui_widget_list_selected_part_below(ui_block_widget_list_get(ap), style))
      return;
   Part *part = ui_widget_list_selected_part_get(ui_block_widget_list_get(ap));
   if (!part) return;
   history_diff_add(style->obj, PART_TARGET, RESTACK, part->name);
   if (!workspace_edit_object_part_below(ap->workspace, part->name))
     {
        NOTIFY_ERROR(_("Internal edje error occurred on part move"));
        ui_widget_list_selected_part_above(ui_block_widget_list_get(ap), style);
     }

   tmp_list = eina_inlist_find(style->parts, EINA_INLIST_GET(part));
   Part *rel = EINA_INLIST_CONTAINER_GET(tmp_list->prev, Part);
   live_view_part_restack_below(ap->live_view, part, rel);
   project_changed(true);
}

static void
_restack_part_above(void *data,
                    Evas_Object *obj __UNUSED__,
                    void *event_info)
{
   App_Data *ap = (App_Data *)data;
   Part *rel = (Part *)event_info;
   Part *part = ui_widget_list_selected_part_get(ui_block_widget_list_get(ap));
   Style *style = ap->project->current_style;
   Eina_Inlist *tmp_list = NULL, *tmp_prev = NULL;

   if ((!part) || (!style)) return;
   history_diff_add(style->obj, PART_TARGET, RESTACK, part->name);
   workspace_edit_object_part_restack(ap->workspace, part->name, rel->name, false);
   live_view_part_restack_above(ap->live_view, part, rel);

   tmp_list = eina_inlist_find(style->parts, EINA_INLIST_GET(part));
   tmp_prev = eina_inlist_find(style->parts, EINA_INLIST_GET(rel));
   project_changed(true);
   if (!tmp_list) return;

   style->parts = eina_inlist_remove(style->parts, tmp_list);
   style->parts = eina_inlist_prepend_relative(style->parts, tmp_list, tmp_prev);
}

static void
_restack_part_below(void *data,
                    Evas_Object *obj __UNUSED__,
                    void *event_info)
{
   App_Data *ap = (App_Data *)data;
   Part *rel = (Part *)event_info;
   Style *style = ap->project->current_style;
   Part *part = ui_widget_list_selected_part_get(ui_block_widget_list_get(ap));
   Eina_Inlist *tmp_list = NULL, *tmp_prev = NULL;

   if ((!part) || (!style)) return;
   history_diff_add(style->obj, PART_TARGET, RESTACK, part->name);
   workspace_edit_object_part_restack(ap->workspace, part->name, rel->name, true);
   live_view_part_restack_below(ap->live_view, part, rel);

   tmp_list = eina_inlist_find(style->parts, EINA_INLIST_GET(part));
   tmp_prev = eina_inlist_find(style->parts, EINA_INLIST_GET(rel));
   project_changed(true);
   if (!tmp_list) return;

   style->parts = eina_inlist_remove(style->parts, tmp_list);
   style->parts = eina_inlist_append_relative(style->parts, tmp_list, tmp_prev);
}

static void
_show_part(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info)
{
   App_Data *ap = (App_Data *)data;
   const char *part_name = (const char *)event_info;

   assert(ap != NULL);
   assert(part_name != NULL);

   if (!workspace_edit_object_visible_set(ap->workspace, part_name, true))
     {
        ERR("Can't make workspace_edit_object visible");
        abort();
     }
}

static void
_signal_select(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info)
{
   App_Data *ap = (App_Data *)data;
   Signal *sig = (Signal *)event_info;
   Part *part = NULL;
   Evas_Object *prop_view = NULL;

   assert(ap != NULL);
   assert(sig != NULL);
   assert(sig->style != NULL);

   evas_object_freeze_events_set(sig->style->obj, false);
   wm_style_state_parts_reset(sig->style);
   edje_edit_program_run(sig->style->obj, sig->program);
   wm_style_current_state_parts_update(sig->style);

   prop_view = ui_block_property_get(ap);
   part = ui_states_list_part_get(ap->block.state_list);
   TODO("Explain to me this piece of code")
   if (part)
     {
        ui_states_list_data_set(ap->block.state_list, sig->style, part);
        ui_property_state_set(prop_view, part);
        workspace_edit_object_part_state_set(ap->workspace, part);
     }
   else
     workspace_edit_object_recalc(ap->workspace);
   evas_object_freeze_events_set(sig->style->obj, true);
}


static void
_hide_part(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info)
{
   App_Data *ap = (App_Data *)data;
   const char *part_name = (const char *)event_info;

   assert(ap != NULL);
   assert(part_name != NULL);

   if (!workspace_edit_object_visible_set(ap->workspace, part_name, false))
     {
        ERR("Can't hide workspace_edit_object");
        abort();
     }
}

static void
_add_state_dialog(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   state_dialog_state_add(ap);
}

static void
_del_state_dialog(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   state_dialog_state_del(ap);
}

static void
_del_layout(void *data,
            Evas_Object *obj __UNUSED__,
            void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   ui_group_delete(ap, LAYOUT);
}

static Evas_Object *
_widgetlist_current_genlist_get(App_Data *ap, Type group_type)
{
   Evas_Object *tabs = NULL;
   const Evas_Object *nf = NULL;
   Elm_Object_Item *nf_item = NULL;
   Ewe_Tabs_Item *current_tab_item = NULL;

   assert(ap != NULL);

   tabs = ui_block_widget_list_get(ap);
   current_tab_item = ewe_tabs_active_item_get(tabs);
   nf = ewe_tabs_item_content_get(tabs, current_tab_item);

   if ((group_type == STYLE) || (group_type == CLASS))
      nf_item = elm_naviframe_top_item_get(nf);
   else
      nf_item = elm_naviframe_bottom_item_get(nf);

   return elm_object_item_part_content_get(nf_item, NULL);
}

static void
_job_popup_close(void *data)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   evas_object_del(ap->popup);
   ap->popup = NULL;
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
}

static void
_popup_close_cb(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   ecore_job_add(_job_popup_close, data);
}

static void
_add_layout_cb(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   Evas_Object *widget_list, *eoi;
   App_Data *ap = app_data_get();
   Style *layout = NULL;
   Eina_Stringshare *name = NULL;
   Evas_Object *en;

   widget_list = _widgetlist_current_genlist_get(ap, LAYOUT);
   en = (Evas_Object *)data;

   assert(en != NULL);

   name = eina_stringshare_add(elm_entry_entry_get(en));
   if (eina_stringshare_strlen(name) <= 0)
     {
        NOTIFY_WARNING(_("Please type layout name"));
        eina_stringshare_del(name);
        return;
     }

   if (edje_edit_group_exist(ap->project->global_object, name))
     {
        NOTIFY_INFO(3, _("Failed to create new layout: layout with this name already exist"));
        eina_stringshare_del(name);
        return;
     }

   if (!edje_edit_group_add(ap->project->global_object, name))
     {
        ERR("Can't add group");
        abort();
     }

   pm_save_to_dev(ap->project, NULL, true);
   layout = wm_style_add(name, name, LAYOUT, NULL);
   ap->project->layouts = eina_inlist_append(ap->project->layouts,
                                             EINA_INLIST_GET(layout));
   wm_style_data_load(layout, evas_object_evas_get(widget_list),
                      ap->project->mmap_file);
   ui_widget_list_layouts_reload(widget_list, ap->project);
   widget_list = _widgetlist_current_genlist_get(ap, LAYOUT);
   eoi = elm_genlist_last_item_get(widget_list);
   elm_genlist_item_selected_set(eoi, true);
   eina_stringshare_del(name);

   ecore_job_add(_job_popup_close, ap);
   project_changed(true);
   eina_stringshare_del(name);
   return;
}

static void
_popup_layout_add(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   Evas_Object *item, *en, *bt;
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   /* Checking if the source style is an alias.
      We can't clone aliases right now, it need lots of difficult code for that.
   */
   ap->popup = elm_popup_add(ap->win_layout);
   elm_object_part_text_set(ap->popup, "title,text", _("Add new layout"));
   elm_popup_orient_set(ap->popup, ELM_POPUP_ORIENT_CENTER);

   LAYOUT_PROP_ADD(ap->popup, "Style name:", "property", "1swallow")
   ENTRY_ADD(item, en, true)
   elm_object_part_text_set(en, "guide", _("Type a new layout name"));
   elm_entry_markup_filter_append(en, elm_entry_filter_accept_set, &accept_name);
   elm_object_part_content_set(item, "elm.swallow.content", en);
   elm_object_content_set(ap->popup, item);

   BUTTON_ADD(ap->popup, bt, _("Add"));
   evas_object_smart_callback_add(bt, "clicked", _add_layout_cb, en);
   elm_object_part_content_set(ap->popup, "button1", bt);

   BUTTON_ADD(ap->popup, bt, _("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _popup_close_cb, ap);
   elm_object_part_content_set(ap->popup, "button2", bt);

   evas_object_show(ap->popup);
   elm_object_focus_set(en, true);
}

static void
_del_style(void *data,
           Evas_Object *obj __UNUSED__,
           void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   ui_group_delete(ap, STYLE);
}

static void
_add_style_dailog(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   style_dialog_add(ap);
}

static void
_part_name_change(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   const char *part = (const char *)event_info;
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);
   assert(part != NULL);

   ui_widget_list_part_update(ui_block_widget_list_get(ap), part);
}

static void
_property_change(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info)
{
   App_Data *ap = (App_Data *)data;
   Part *part = (Part *)event_info;

   assert(ap != NULL);
   assert(part != NULL);

   Evas_Object *prop = ui_block_property_get(ap);

   assert(prop != NULL);

   ui_property_state_set(prop, part);
}

static void
_on_ws_part_select(void *data,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   App_Data *ap = (App_Data *)data;
   const char *part = (const char *)event_info;

   assert(ap != NULL);
   assert(part != NULL);

   ui_widget_list_part_selected_set(ui_block_widget_list_get(ap), part, true);
}

Widget *
ui_widget_from_ap_get(App_Data *ap)
{
   Widget *widget = NULL;
   Evas_Object *gl_widget = NULL;
   Elm_Object_Item *eoi = NULL;

   assert(ap != NULL);

   gl_widget = _widgetlist_current_genlist_get(ap, WIDGET);

   assert(gl_widget != NULL);

   eoi = elm_genlist_selected_item_get(gl_widget);

   assert(eoi != NULL);

   widget = elm_object_item_data_get(eoi);

   assert(widget != NULL);

   return widget;
}

Class *
ui_class_from_ap_get(App_Data *ap)
{
   Evas_Object *gl_class = NULL;
   Elm_Object_Item *eoi = NULL;
   Elm_Object_Item *parent_eoi = NULL;

   assert(ap != NULL);

   gl_class = _widgetlist_current_genlist_get(ap, CLASS);

   assert(gl_class != NULL);

   eoi = elm_genlist_selected_item_get(gl_class);
   TODO("Check logic here");
   if (!eoi)
     {
        eoi = elm_genlist_first_item_get(gl_class);
        if (!eoi) return NULL;
     }
   parent_eoi = elm_genlist_item_parent_get(eoi);
   if (parent_eoi) eoi = parent_eoi;
   return elm_object_item_data_get(eoi);
}


void
ui_part_back(App_Data *ap)
{
   Evas_Object *wl_list, *history_list;

   assert(ap != NULL);

   wl_list = ui_block_widget_list_get(ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,item,add", _add_part_item_dialog, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,item,del", _del_part_item, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,add", _add_part_dialog, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,del", _del_part, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,above", _above_part, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,below", _below_part, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,show", _show_part, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,hide", _hide_part, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,moved,up",
                                       _restack_part_above, ap);
   evas_object_smart_callback_del_full(wl_list, "wl,part,moved,down",
                                       _restack_part_below, ap);
   history_list = ui_block_history_get(ap);
   evas_object_hide(history_list);

   evas_object_smart_callback_del_full(ap->workspace, "part,name,changed",
                                       _part_name_change, ap);

   workspace_highlight_unset(ap->workspace);
   workspace_edit_object_unset(ap->workspace);
   ui_states_list_data_unset(ap->block.state_list);
   ui_signal_list_data_unset(ap->block.signal_list);
   ui_block_content_visible(ap->block.right_bottom, false);
   ui_block_content_visible(ap->block.right_top, false);
   live_view_widget_style_unset(ap->live_view);

   elm_object_disabled_set(ap->block.right_top_btn, true);

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_STYLE_ONLY, true);

   evas_object_smart_callback_del_full(ap->workspace, "ws,part,selected",
                                       _on_ws_part_select, ap);
   evas_object_smart_callback_del_full(ap->workspace, "ws,part,unselected",
                                       _on_ws_part_unselect, ap);
   evas_object_smart_callback_del_full(ap->workspace, "part,changed", _property_change, ap);
#ifdef HAVE_ENVENTOR
   enventor_object_project_unload(ap->project);
#endif /* HAVE_ENVENTOR */
}

/**
  * may be usable for future API.
  */
void
ui_style_back(App_Data *ap __UNUSED__)
{
}

void
ui_state_select(App_Data *ap,
                Evas_Object *obj,
                Eina_Stringshare *state)
{
   Part *part = NULL;
   Evas_Object *prop_view;

   assert(ap != NULL);
   assert(obj != NULL);

   prop_view = ui_block_property_get(ap);
   part = ui_states_list_part_get(obj);

   assert(part != NULL);

   wm_part_current_state_set(part, state);
   ui_property_state_set(prop_view, part);
   workspace_edit_object_part_state_set(ap->workspace, part);
}

Evas_Object *
ui_part_select(App_Data *ap, Part* part)
{
   Evas_Object *prop;

   assert(ap != NULL);
   assert(part != NULL);

   prop = ui_block_property_get(ap);

   assert(prop != NULL);

   ui_property_part_set(prop, part);

   ui_states_list_data_set(ap->block.state_list, ap->project->current_style, part);

   evas_object_smart_callback_del_full(ap->workspace, "part,name,changed",
                                       _part_name_change, ap);
   evas_object_smart_callback_add(ap->workspace, "part,name,changed",
                                  _part_name_change, ap);

   ui_property_state_set(prop, part);
   workspace_edit_object_part_state_set(ap->workspace, part);
   workspace_highlight_unset(ap->workspace);
   workspace_highlight_set(ap->workspace, part);
   evas_object_smart_callback_del_full(ap->workspace, "part,changed", _property_change, ap);
   evas_object_smart_callback_add(ap->workspace, "part,changed", _property_change, ap);

   return ap->block.state_list;
}

/* FIXME: rename to style_clicked */
Eina_Bool
ui_style_clicked(App_Data *ap, Style *style)
{
   Evas_Object *wl_list, *prop, *history_list;
   Style *_style, *_alias_style;

   assert(ap != NULL);
   assert(ap->project != NULL);
   assert(style != NULL);

   _alias_style = style;
   _style = style;
   if (_alias_style->isAlias) _style = _alias_style->main_group;

   wl_list = ui_block_widget_list_get(ap);
   evas_object_smart_callback_add(wl_list, "wl,part,add", _add_part_dialog, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,del", _del_part, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,above", _above_part, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,below", _below_part, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,show", _show_part, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,hide", _hide_part, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,moved,up", _restack_part_above, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,moved,down",_restack_part_below, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,item,add", _add_part_item_dialog, ap);
   evas_object_smart_callback_add(wl_list, "wl,part,item,del", _del_part_item, ap);

   TODO("need to move this callbacks to his blocks")
   evas_object_smart_callback_del_full(ap->block.state_list, "stl,state,add", _add_state_dialog, ap);
   evas_object_smart_callback_add(ap->block.state_list, "stl,state,add", _add_state_dialog, ap);
   evas_object_smart_callback_del_full(ap->block.state_list, "stl,state,del", _del_state_dialog, ap);
   evas_object_smart_callback_add(ap->block.state_list, "stl,state,del", _del_state_dialog, ap);

   evas_object_smart_callback_del_full(ap->block.signal_list, "sl,signal,select", _signal_select, ap);
   evas_object_smart_callback_add(ap->block.signal_list, "sl,signal,select", _signal_select, ap);
   ui_signal_list_data_set(ap->block.signal_list, _style);

   workspace_edit_object_set(ap->workspace, _style, ap->project->dev);
   evas_object_smart_callback_add(ap->workspace, "ws,part,selected",
                                  _on_ws_part_select, ap);
   evas_object_smart_callback_add(ap->workspace, "ws,part,unselected",
                                  _on_ws_part_unselect, ap);

   /* style properties */
   prop = ui_block_property_get(ap);
   if (!prop)
     {
        prop = ui_property_add(ap->win);
        ui_block_property_set(ap, prop);
     }


   ui_block_content_visible(ap->block.right_bottom, true);
   ui_property_style_set(prop, _alias_style, ap->workspace);
   evas_object_show(prop);
   ap->project->current_style = _style;

   pm_save_to_dev(ap->project, ap->project->current_style, true);

   history_list = history_genlist_get(ap->history, ap->block.right_top);
   history_module_add(_style->obj);
   ui_block_history_set(ap, history_list);
   ui_block_content_visible(ap->block.right_top, true);
   elm_object_disabled_set(ap->block.right_top_btn, false);

   live_view_widget_style_set(ap->live_view, ap->project, _style);
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_STYLE_ONLY, false);

   return true;
}

static inline void
_widget_list_layouts_tab_activate(App_Data *ap)
{
   assert(ap != NULL);

   if (!eina_inlist_count(ap->project->widgets))
     ui_widget_list_tab_activate(ui_block_widget_list_get(ap), 1);
}

Eina_Bool
blocks_show(App_Data *ap)
{
   Evas_Object *wd_list;

   assert(ap != NULL);

   wd_list = ui_widget_list_add(ap->block.left_top);
   ui_widget_list_title_set(wd_list, ap->project->name);
   ui_widget_list_data_set(wd_list, ap->project);
   ui_block_widget_list_set(ap, wd_list);
   add_callbacks_wd(wd_list, ap);

   _widget_list_layouts_tab_activate(ap);

   ui_panes_show(ap);

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_BASE, false);
   ui_menu_disable_set(ap->menu, MENU_FILE_SAVE, true);

   code_edit_mode_switch(ap, false);

   return true;
}

static void
_blocks_data_unset(App_Data *ap)
{
   assert(ap != NULL);

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_BASE, true);
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_STYLE_ONLY, true);

   ui_block_content_visible(ap->block.right_bottom, false);
   ui_signal_list_data_unset(ap->block.signal_list);
   ui_states_list_data_unset(ap->block.state_list);
   history_clear(ap->history);
   workspace_highlight_unset(ap->workspace);
   workspace_edit_object_unset(ap->workspace);
   live_view_widget_style_unset(ap->live_view);
}

static Eina_Bool
_eflete_filter(const char *path,
               Eina_Bool dir,
               void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, ".pro"))
     return true;
   return false;
}

static void
_fs_close(void *data,
          Evas_Object *obj,
          void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   ap->modal_editor--;
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
   evas_object_del(obj);
}

static void
_on_open_done(void *data,
              Evas_Object *obj __UNUSED__,
              void *event_info)
{
   Evas_Object *win;
   const char *selected;
   App_Data *ap;

   win = (Evas_Object *)data;

   assert(win != NULL);

   selected = (const char *)event_info;
   ap = app_data_get();

   if (!selected)
     {
        ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_STYLE_ONLY, true);
        ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_BASE, true);
        _fs_close(ap, win, NULL);
        return;
     }

   ap->project = pm_project_open(selected);
   if (!ap->project)
     {
        NOTIFY_WARNING(_("Project is not selected."));
        return;
     }

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
   wm_widgets_list_objects_load(ap->project->widgets,
                                evas_object_evas_get(ap->win),
                                ap->project->mmap_file);
   wm_layouts_list_objects_load(ap->project->layouts,
                                evas_object_evas_get(ap->win),
                                ap->project->mmap_file);
   wm_styles_build_alias(ap->project->widgets,
                         ap->project->layouts);
   blocks_show(ap);

   evas_object_del(win);
   ap->modal_editor--;
   NOTIFY_INFO(3, _("Project '%s' is opened."), ap->project->name);
   STATUSBAR_PROJECT_PATH(ap, ap->project->pro_path);
   STATUSBAR_PROJECT_SAVE_TIME_UPDATE(ap);

   _widget_list_layouts_tab_activate(ap);
}

Eina_Bool
project_close(App_Data *ap)
{
   assert(ap != NULL);

   if (ap->project)
     {
        if (!_project_close_request(ap, ap->project->changed))
          return false;

        STATUSBAR_PROJECT_PATH(ap, _("No project opened"));
        ui_panes_hide(ap);
        _blocks_data_unset(ap);
        pm_project_close(ap->project);
        ap->project = NULL;
     }
   return true;
}

void
project_open(void)
{
   Evas_Object *win, *fs, *ic;
   App_Data *ap = app_data_get();
   if (!project_close(ap))
     return;
   win  = mw_add(NULL, _fs_close, ap);

   assert(win != NULL);

   evas_object_show(win);
   mw_title_set(win, "Select a project file");
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "folder");
   mw_icon_set(win, ic);

   FILESELECTOR_ADD(fs, win, _on_open_done, win);
   elm_fileselector_custom_filter_append(fs, _eflete_filter, NULL, "Eflete Files");
   elm_fileselector_mime_types_filter_append(fs, "*", "All Files");
   elm_win_inwin_content_set(win, fs);
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, true);

   ap->modal_editor++;
}

/****************************** Project save **********************************/

static Eina_Bool
_progress_print(void *data, Eina_Stringshare *progress_string)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   elm_object_part_text_set(ap->splash, "label.info", progress_string);

   return true;
}

static void
_progress_end(void *data, PM_Project_Result result)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   switch (result)
     {
      case PM_PROJECT_ERROR:
        {
           NOTIFY_INFO(3, _("Error: project not saved."));
           break;
        }
      case PM_PROJECT_CANCEL:
        {
           NOTIFY_INFO(3, _("Saving canceled."));
           break;
        }
      case PM_PROJECT_SUCCESS:
        {
           ap->project->changed = false;
           break;
        }
      default:
        {
           ERR("Wrong result");
           abort();
        }
     }

#ifdef HAVE_ENVENTOR
   if (ap->enventor_mode)
     {
        wm_widgets_list_objects_load(ap->project->widgets,
                                     evas_object_evas_get(ap->win),
                                     ap->project->mmap_file);
        wm_layouts_list_objects_load(ap->project->layouts,
                                     evas_object_evas_get(ap->win),
                                     ap->project->mmap_file);
        wm_styles_build_alias(ap->project->widgets,
                              ap->project->layouts);
        enventor_object_focus_set(ap->enventor, true);
        pm_save_to_dev(ap->project, ap->project->current_style, true);
     }
#endif /* HAVE_ENVENTOR */

   splash_del(ap->splash);
   ap->splash = NULL;
}

static Eina_Bool
_setup_save_splash(void *data, Splash_Status status __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

#ifdef HAVE_ENVENTOR
   if (ap->enventor_mode)
     {
        enventor_object_file_version_update(ap->enventor, ap->project, "110");

        ap->pr_thread = pm_project_enventor_save(ap->project,
                                                 _progress_print,
                                                 _progress_end,
                                                 data);
        pm_save_to_dev(ap->project, ap->project->current_style, true);
     }
   else
     {
#endif /* HAVE_ENVENTOR */
         ap->pr_thread = pm_project_save(ap->project,
                                         _progress_print,
                                         _progress_end,
                                         data);
#ifdef HAVE_ENVENTOR
     }
#endif /* HAVE_ENVENTOR */
   if (!ap->pr_thread) return false;

   return true;
}

static Eina_Bool
_teardown_save_splash(void *data, Splash_Status status)
{
   App_Data *ap = (App_Data *) data;

   assert(ap != NULL);

   if (status == SPLASH_SUCCESS)
     STATUSBAR_PROJECT_SAVE_TIME_UPDATE(ap);

   ap->project->changed = false;

   TODO("Check if this recalc is necessary");
   if (ap->project->current_style)
     workspace_edit_object_recalc(ap->workspace);
   pm_project_thread_free(ap->pr_thread);
   ap->pr_thread = NULL;

   ecore_main_loop_quit();
   return true;
}

void
project_save(void)
{
   App_Data *ap;

   ap = app_data_get();

   assert(ap->project != NULL);

#ifdef HAVE_ENVENTOR
   if (!ap->enventor_mode)
#endif /* HAVE_ENVENTOR */
     if (!ap->project->changed) return;
   if (ap->splash) return;

   ap->splash = splash_add(ap->win, _setup_save_splash, _teardown_save_splash, NULL, ap);
   evas_object_focus_set(ap->splash, true);
   evas_object_show(ap->splash);

#ifdef HAVE_ENVENTOR
   if (!ap->enventor_mode)
#endif /* HAVE_ENVENTOR */
     ui_menu_disable_set(ap->menu, MENU_FILE_SAVE, true);
     ecore_main_loop_begin();
}

/******************************************************************************/

void
project_changed(Eina_Bool save)
{
   App_Data *ap;

   ap = app_data_get();

   pm_save_to_dev(ap->project, ap->project->current_style, save);
   ap->project->changed = true;
   ui_menu_disable_set(ap->menu, MENU_FILE_SAVE, false);
   /* for example this function will be called after adding layout, so no need
      in updating live view */
   if (ap->project->current_style)
     live_view_theme_update(ap->live_view, ap->project);
}

/******************************************************************************/

TODO("I think, this functionality need move to dialogs")
static void
_replace_cb(void *data,
            Evas_Object *obj __UNUSED__,
            void *ei __UNUSED__)
{
   Eina_Bool *res = data;
   *res = true;
   ecore_main_loop_quit();
}

static void
_ecancel_cb(void *data,
            Evas_Object *obj __UNUSED__,
            void *ei __UNUSED__)
{
   Eina_Bool *res = data;
   *res = false;
   ecore_main_loop_quit();
}


Eina_Bool
export_replace_request(Evas_Object *parent, const char *msg)
{
   Eina_Bool result = false;
   Evas_Object *popup, *btn;
   Eina_Stringshare *title;

   assert(parent != NULL);

   title = eina_stringshare_printf(_("Export project"));
   popup = elm_popup_add(parent);
   elm_object_part_text_set(popup, "title,text", title);
   elm_popup_content_text_wrap_type_set(popup, ELM_WRAP_WORD);
   elm_object_text_set(popup, msg);
   BUTTON_ADD(popup, btn, _("Replace"));
   evas_object_smart_callback_add(btn, "clicked", _replace_cb, &result);
   elm_object_part_content_set(popup, "button1", btn);
   BUTTON_ADD(popup, btn, _("Cancel"));
   evas_object_smart_callback_add(btn, "clicked", _ecancel_cb, &result);
   elm_object_part_content_set(popup, "button2", btn);
   evas_object_show(popup);
   eina_stringshare_del(title);

   ecore_main_loop_begin();

   evas_object_del(popup);

   return result;
}

Eina_Bool
export_warning(Evas_Object *parent, const char *title, const char *msg)
{
   Eina_Bool result = false;
   Evas_Object *popup, *btn;

   assert(parent != NULL);

   popup = elm_popup_add(parent);
   elm_object_part_text_set(popup, "title,text", title);
   elm_popup_content_text_wrap_type_set(popup, ELM_WRAP_WORD);
   elm_object_text_set(popup, msg);
   BUTTON_ADD(popup, btn, _("Ok"));
   evas_object_smart_callback_add(btn, "clicked", _ecancel_cb, &result);
   elm_object_part_content_set(popup, "button1", btn);
   evas_object_show(popup);

   ecore_main_loop_begin();

   evas_object_del(popup);

   return result;
}


static Eina_Bool
_export_splash_setup(void *data, Splash_Status status __UNUSED__)
{
   App_Data *ap;
   Project_Thread *thread;
   const char *path;

   ap = app_data_get();
   path = (const char *)data;

   assert(path != NULL);

   thread = pm_project_develop_export(ap->project, path,
                                      _progress_print, _progress_end,
                                      ap);
   assert(thread != NULL);

   return true;
}

static Eina_Bool
_export_splash_teardown(void *data, Splash_Status status __UNUSED__)
{
   eina_stringshare_del(data);
   return true;
}

static void
_on_export_done(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info)
{
   Eina_Stringshare *selected, *path, *dest_file;
   Evas_Object *win;
   App_Data *ap;

   ap = app_data_get();
   win = (Evas_Object *)data;

   assert(win != NULL);

   selected = (const char *)event_info;
   path = elm_fileselector_path_get(obj);
   if (!selected)
     {
        ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
        evas_object_del(win);
        return;
     }

   if (!strcmp(selected, path))
     {
        export_warning(win, _("Export edj develop"),
                       _("Name field is empty! Please type a file name to export."));
        return;
     }

   dest_file = eina_stringshare_add(selected);
   if (!eina_str_has_extension(selected, ".edj"))
     dest_file = eina_stringshare_printf("%s.edj", selected);
   if ((ecore_file_exists(dest_file)) &&
       (!export_replace_request(win, _("The file already exists."
                                       "Replacement will overwrite its contents."))))
     return;

   ap->splash = splash_add(ap->win, _export_splash_setup, _export_splash_teardown,
                           NULL, (void *)eina_stringshare_add(dest_file));
   evas_object_focus_set(ap->splash, true);
   evas_object_show(ap->splash);

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
   eina_stringshare_del(dest_file);
   evas_object_del(win);
}

static Eina_Bool
_edje_filter(const char *path,
             Eina_Bool dir,
             void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, ".edj"))
     return true;
   return false;
}

void
project_export_develop(void)
{
   Evas_Object *win, *fs;
   App_Data *ap = app_data_get();

   win  = mw_add(NULL, _fs_close, ap);

   assert(win != NULL);

   evas_object_show(win);
   FILESELECTOR_ADD(fs, win, _on_export_done, win);
   elm_object_text_set(fs, "Select a file");
   elm_fileselector_is_save_set(fs, true);
   elm_fileselector_custom_filter_append(fs, _edje_filter, NULL, "Edje Files");
   elm_fileselector_mime_types_filter_append(fs, "*", "All Files");
   elm_win_inwin_content_set(win, fs);
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, true);
}

static void
_on_export_edc_group_done(void *data,
                          Evas_Object *obj,
                          void *event_info)
{
   const char *selected;
   Evas_Object *win;
   Eina_Stringshare *path = NULL;
   Eina_Stringshare *dir_path = NULL;
   App_Data *ap;
   Style *style = NULL;

   char **tmp;
   unsigned int tokens_count, i;

   ap = app_data_get();
   win = (Evas_Object *)data;

   assert(win != NULL);

   selected = (const char *)event_info;
   if (!selected)
     {
        evas_object_del(win);
        return;
     }
   style = ap->project->current_style;

   Eina_Stringshare *file = NULL;

   tmp = eina_str_split_full(style->full_group_name, "/", 0, &tokens_count);
   if (!tmp[0]) return;
   file = eina_stringshare_add(tmp[0]);

   for (i = 1; tokens_count - 1 > 0; i++, tokens_count--)
     file = eina_stringshare_printf("%s_%s", file, tmp[i]);
   free(tmp[0]);
   free(tmp);

   dir_path = eina_stringshare_printf("%s/%s", elm_fileselector_path_get(obj), file);
   if (!ecore_file_exists(dir_path))
     ecore_file_mkdir(dir_path);
   path = eina_stringshare_printf("%s/%s.edc", dir_path, file);
   if (!pm_project_style_source_code_export(ap->project, style, path))
     ERR("Source code of the current style was not exported to edc file %s", path);
   pm_style_resource_export(ap->project, style, dir_path);

   eina_stringshare_del(file);
   eina_stringshare_del(path);
   eina_stringshare_del(dir_path);
   evas_object_del(win);
}

void
project_export_edc_group(void)
{
   Evas_Object *win, *fs;
   App_Data *ap = app_data_get();

   win  = mw_add(NULL, _fs_close, ap);

   assert(win != NULL);

   evas_object_show(win);
   FILESELECTOR_ADD(fs, win, _on_export_edc_group_done, win);
   elm_fileselector_is_save_set(fs, false);
   elm_fileselector_folder_only_set(fs, true);
   elm_win_inwin_content_set(win, fs);
}

static void
_on_export_edc_project_done(void *data,
                            Evas_Object *obj __UNUSED__,
                            void *event_info)
{
   const char *selected;
   Evas_Object *win;
   Eina_Stringshare *dir_path = NULL;
   Eina_Stringshare *build_sh = NULL;
   Eina_Stringshare *build_str = NULL;
   Eina_Stringshare *current_dev_path = NULL;
   Eina_Strbuf *request_str = NULL;
   Eina_Bool chk = true;
   FILE *fbuild;
   App_Data *ap;

   ap = app_data_get();
   win = (Evas_Object *)data;

   assert(win != NULL);

   selected = (const char *)event_info;
   if (!selected)
     {
        evas_object_del(win);
        ap->modal_editor--;
        return;
     }
   dir_path = eina_stringshare_printf("%s/%s", selected, ap->project->name);

   if (ecore_file_exists(dir_path))
     {
        if (ecore_file_is_dir(dir_path))
          {
             current_dev_path = eina_stringshare_printf("%s/develop", dir_path);
             if (!strcmp(ap->project->develop_path, current_dev_path))
               {
                  export_warning(win, _("Export edc project"),
                                 _("Can not delete current project folder!"));
                  return;
               }
             request_str = eina_strbuf_new();
             eina_strbuf_append_printf(request_str,
                                       _("The <path>'%s'</path> directory "
                                         "already exists.<br>Do you want to "
                                         "<b><orange>delete all</orange></b>"
                                         " content of this folder and create "
                                         "new for project export edc in it?"),
                                       dir_path);

             chk = export_replace_request(win, eina_strbuf_string_get(request_str));
             if (chk)
               {
                  chk = ecore_file_recursive_rm(dir_path);
                  if (!chk)
                    {
                       NOTIFY_ERROR(_("Can not delete folder %s!"),
                                    dir_path);
                    }
                  else
                    ecore_file_mkdir(dir_path);
               }
             else
               return;
          }
        else
          {
             NOTIFY_ERROR(_("The name <path>'%s'</path> is "
                            "already used at this location."),
                          dir_path);
             chk = false;
          }
     }
   else
     ecore_file_mkdir(dir_path);

   if (request_str)
     eina_strbuf_free(request_str);

   pm_project_source_code_export(ap->project, dir_path);
   pm_project_resource_export(ap->project, dir_path);
   build_sh = eina_stringshare_printf("%s/build.sh", dir_path);
   fbuild = fopen(build_sh, "w");
   if (!fbuild)
     ERR("Could't open file '%s'", build_sh);
   build_str = eina_stringshare_printf("%s%s.edc", makefile, ap->project->name);
   fputs(build_str, fbuild);
   fclose(fbuild);
   chmod(build_sh, 777);

   eina_stringshare_del(dir_path);
   eina_stringshare_del(build_sh);
   eina_stringshare_del(build_str);
   if (current_dev_path)
     eina_stringshare_del(current_dev_path);
   evas_object_del(win);
   ap->modal_editor--;
}

void
project_export_edc_project(void)
{
   Evas_Object *win, *fs;
   App_Data *ap = app_data_get();

   win  = mw_add(NULL, _fs_close, ap);

   assert(win != NULL);

   evas_object_show(win);
   FILESELECTOR_ADD(fs, win, _on_export_edc_project_done, win);
   elm_fileselector_is_save_set(fs, false);
   elm_fileselector_folder_only_set(fs, true);
   elm_win_inwin_content_set(win, fs);
}

/*************************** Close request popup ******************************/
TODO("I think, this functionality need move to dialogs")
static void
_discard_cb(void *data,
            Evas_Object *obj __UNUSED__,
            void *ei __UNUSED__)
{
   Eina_Bool *res = data;

   assert(res != NULL);

   *res = true;
   ecore_main_loop_quit();
}

static void
_cancel_cb(void *data,
           Evas_Object *obj __UNUSED__,
           void *ei __UNUSED__)
{
   Eina_Bool *res = data;

   assert(res != NULL);

   *res = false;
   ecore_main_loop_quit();
}

static void
_save_cb(void *data,
         Evas_Object *obj __UNUSED__,
         void *ei __UNUSED__)
{
   Eina_Bool *res = data;

   assert(res != NULL);

   App_Data *ap = app_data_get();
   evas_object_hide(ap->popup);
   project_save();
   *res = true;
   ecore_main_loop_quit();
}

static void
_close_cb(void *data,
         Evas_Object *obj __UNUSED__,
         void *ei __UNUSED__)
{
   Eina_Bool *res = data;

   assert(res != NULL);

   App_Data *ap = app_data_get();
   evas_object_hide(ap->popup);
   *res = true;
   ecore_main_loop_quit();
}

static Eina_Bool
_project_close_request(App_Data *ap, Eina_Bool changed)
{
   assert(ap != NULL);

   if (ap->project->close_request) return false;
   ap->project->close_request = true;

   Eina_Bool result = false;
   Evas_Object *btn, *label;
   Eina_Stringshare *title;
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, true);
   title = eina_stringshare_printf(_("Close project %s"), ap->project->name);
   ap->popup = elm_popup_add(ap->win_layout);
   if (changed)
     {
        BUTTON_ADD(ap->popup, btn, _("Save"));
        evas_object_smart_callback_add(btn, "clicked", _save_cb, &result);
        elm_object_part_content_set(ap->popup, "button1", btn);
        BUTTON_ADD(ap->popup, btn, _("Don't save"));
        evas_object_smart_callback_add(btn, "clicked", _discard_cb, &result);
        elm_object_part_content_set(ap->popup, "button2", btn);

        LABEL_ADD(ap->popup, label, _("Do you want to save changes?"));
     }
   else
     {
        BUTTON_ADD(ap->popup, btn, _("Close"));
        evas_object_smart_callback_add(btn, "clicked", _close_cb, &result);
        elm_object_part_content_set(ap->popup, "button1", btn);

        LABEL_ADD(ap->popup, label, _("Do you want to close project?"));
     }
   elm_object_part_text_set(ap->popup, "title,text", title);
   elm_object_content_set(ap->popup, label);

   BUTTON_ADD(ap->popup, btn, _("Cancel"));
   evas_object_smart_callback_add(btn, "clicked", _cancel_cb, &result);
   if (changed)
     elm_object_part_content_set(ap->popup, "button3", btn);
   else
     elm_object_part_content_set(ap->popup, "button2", btn);
   evas_object_show(ap->popup);
   eina_stringshare_del(title);

   ecore_main_loop_begin();

   ap->project->close_request = false;
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
   evas_object_del(ap->popup);
   ap->popup = NULL;

   return result;
}

/******************************************************************************/
static Eina_Bool
_selected_layout_delete(Evas_Object *genlist, App_Data *ap)
{
   assert(genlist != NULL);
   assert(ap != NULL);

   Elm_Object_Item *item_to_del = elm_genlist_selected_item_get(genlist), *eoi;
   Style *style = elm_object_item_data_get(item_to_del);;
   int i;

   if (!item_to_del)
     {
        NOTIFY_INFO(3, _("Select item to be deleted"));
        return false;
     }

   eoi = elm_genlist_item_next_get(item_to_del);
   if (!eoi)
     {
        eoi = elm_genlist_item_prev_get(item_to_del);
        i = elm_genlist_item_index_get(eoi);
     }
   else
     {
        i = elm_genlist_item_index_get(eoi);
        i--;
     }

   pm_save_to_dev(ap->project, NULL, true);
   evas_object_del(style->obj);
   if (!edje_edit_group_del(ap->project->global_object, style->full_group_name))
     {
        ERR("Can't del group");
        abort();
     }
   ap->project->layouts = eina_inlist_remove(ap->project->layouts,
                                             EINA_INLIST_GET(style));
   ui_widget_list_layouts_reload(genlist, ap->project);
   genlist = _widgetlist_current_genlist_get(ap, LAYOUT);

   for (eoi = elm_genlist_first_item_get(genlist);
        eoi != elm_genlist_item_next_get(elm_genlist_last_item_get(genlist));
        eoi = elm_genlist_item_next_get(eoi))
     {
        if (elm_genlist_item_index_get(eoi) == i)
          {
             style = elm_object_item_data_get(eoi);
             elm_genlist_item_selected_set(eoi, true);
             style = elm_object_item_data_get(eoi);
             break;
          }
     }

   project_changed(true);
   return true;
}

static Eina_Bool
_selected_style_delete(Evas_Object *genlist, App_Data *ap)
{
   Widget *widget = ui_widget_from_ap_get(ap);
   Class *class_st = NULL;
   Style *style = NULL;
   Style *alias_style = NULL;

   assert(genlist != NULL);
   assert(ap != NULL);

   Elm_Object_Item *eoi = elm_genlist_selected_item_get(genlist);
   Eina_List *l = NULL;

   if (!eoi)
     {
        NOTIFY_INFO(3, _("No one style is selected"));
        return false;
     }

   /* Check which type of node is selected: style or class */
   if (elm_genlist_item_parent_get(eoi))
     {
        style = elm_object_item_data_get(eoi);
        class_st = style->parent;
     }
   else
     class_st = elm_object_item_data_get(eoi);

   if (class_st->__type != CLASS) return false;

   if (style) /* Deleting style */
     {
        Evas_Object *st_cur = style->obj;

        if (style->isAlias) st_cur = style->main_group->obj;

        if (!edje_edit_group_exist(st_cur, style->full_group_name))
          {
             NOTIFY_INFO(3, _("Style [%s] don't exist"), style->name);
             return false;
          }
        /* Before delete groups from edj file need free tree of aliasses structures*/
        EINA_LIST_FOREACH(style->aliasses, l, alias_style)
          {
              if (eina_inlist_count(alias_style->parent->styles) <= 1)
                wm_class_free(alias_style->parent);
              else
                wm_style_free(alias_style);
          }
         /*Delete loaded object for unlock group in edj file*/
        pm_save_to_dev(ap->project, NULL, true);
        evas_object_del(style->obj);
        if (!edje_edit_group_del(ap->project->global_object, style->full_group_name))
          {
             NOTIFY_INFO(3, _("Failed to delete style[%s]"), style->name);
             return false;
          }
        if (eina_inlist_count(class_st->styles) <= 1)
          wm_class_free(class_st);
        else
          wm_style_free(style);
     }
   else
     return true;
   /*else  Deleting all styles in class. Work slow! Need patch for edje_edit_group_del
     {
        EINA_INLIST_FOREACH_SAFE(class_st->styles, l, style)
          {
             evas_object_del(style->obj);
             if (!edje_edit_group_del(ap->project->global_object, style->full_group_name))
               NOTIFY_INFO(3, _("Failed to delete style[%s] in class [%s]"),
                           style->name, class_st->name);
          }
        wm_class_free(class_st);
     }*/

   project_changed(true);

   ui_widget_list_class_data_reload(genlist, widget->classes);

   /* deleting widget */
   if (elm_genlist_items_count(genlist) == 0)
     {
        Evas_Object *widget_list = ui_block_widget_list_get(ap);
        Evas_Object *naviframe = evas_object_data_get(widget_list, "nf_widgets");
        Elm_Object_Item *item_gl_widgets = elm_naviframe_item_pop(naviframe);
        elm_naviframe_item_pop_to(item_gl_widgets);
        genlist = elm_object_item_part_content_get(elm_naviframe_top_item_get(naviframe),
                                                   "elm.swallow.content");
        eoi = elm_genlist_selected_item_get(genlist);
        elm_object_item_del(eoi);
     }

   return true;
}

Eina_Bool
ui_group_delete(App_Data *ap, Type group_type)
{
   Evas_Object *gl_groups = NULL;

   assert(ap != NULL);

   gl_groups = _widgetlist_current_genlist_get(ap, group_type);

   Elm_Object_Item *eoi = elm_genlist_selected_item_get(gl_groups);
   if (!eoi)
     return true;

   if (group_type != LAYOUT)
     return _selected_style_delete(gl_groups, ap);
   else
     return _selected_layout_delete(gl_groups, ap);
   return true;
}

#ifdef HAVE_ENVENTOR
static void
_on_enventor_mode_on(void *data,
                     Evas_Object *enventor __UNUSED__,
                     void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   workspace_highlight_unset(ap->workspace);
   ui_property_part_unset(ui_block_property_get(ap));
   ui_states_list_data_unset(ap->block.state_list);
}

static void
_on_enventor_mode_off(void *data,
                      Evas_Object *enventor __UNUSED__,
                      void *event_info)
{
   App_Data *ap = (App_Data *)data;
   Style *style = (Style *)event_info;

   assert(ap != NULL);
   assert(style != NULL);

   wm_style_data_reload(style, ap->project->mmap_file);
   workspace_edit_object_set(ap->workspace, style, ap->project->dev);
   workspace_edit_object_recalc(ap->workspace);
   ui_widget_list_style_parts_reload(ui_block_widget_list_get(ap), style);
   ui_property_style_set(ui_block_property_get(ap), style, ap->workspace);
}
#endif /* HAVE_ENVENTOR */

Eina_Bool
register_callbacks(App_Data *ap)
{
   assert(ap != NULL);
   assert(ap->block.left_top != NULL);

   evas_object_smart_callback_add(ap->block.left_top, "wl,style,add",
                                  _add_style_dailog, ap);
   evas_object_smart_callback_add(ap->block.left_top, "wl,style,del",
                                  _del_style, ap);

   evas_object_smart_callback_add(ap->block.left_top, "wl,layout,add",
                                  _popup_layout_add, ap); //_add_layout_cb, ap);
   evas_object_smart_callback_add(ap->block.left_top, "wl,layout,del",
                                  _del_layout, ap);

#ifdef HAVE_ENVENTOR
   evas_object_smart_callback_add(ap->enventor, "enventor,mode,on",
                                  _on_enventor_mode_on, ap);
   evas_object_smart_callback_add(ap->enventor, "enventor,mode,off",
                                  _on_enventor_mode_off, ap);
#endif /* HAVE_ENVENTOR */
   return true;
}

static void
_on_state_selected(void *data,
                   Evas_Object *obj,
                   void *event_info)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   Elm_Object_Item *eoit = (Elm_Object_Item *)event_info;
   Eina_Stringshare *state = elm_object_item_data_get(eoit);
   ui_state_select(ap, obj, state);
}

static void
_on_style_clicked(void *data,
                         Evas_Object *obj __UNUSED__,
                         void *event_data)
{
   App_Data *ap = (App_Data *)data;
   Style *_style = (Style *)event_data;

   assert(ap != NULL);
   assert(_style != NULL);

   ui_style_clicked(ap, _style);
   workspace_zoom_factor_set(ap->workspace, 1.0);
}

static void
_on_part_selected(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_data)
{
   App_Data *ap = (App_Data *)data;
   Part *part = (Part *) event_data;

   assert(ap != NULL);
   assert(part != NULL);

   Evas_Object *gl_states = ui_part_select(ap, part);

   if (gl_states)
     evas_object_smart_callback_add(gl_states, "stl,state,select", _on_state_selected, ap);
}

static void
_on_part_back(void *data,
              Evas_Object *obj __UNUSED__,
              void *event_data __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   workspace_zoom_factor_set(ap->workspace, 1.0);
   ui_part_back(ap);
}

static void
_on_style_back(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_data __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   ui_style_back(ap);
}

static void
_on_part_item_selected(void *data,
                       Evas_Object *obj __UNUSED__,
                       void *event_data)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   Eina_Stringshare *item_name = (Eina_Stringshare *) event_data;
   workspace_edit_object_part_item_selected_set(ap->workspace, item_name, true);
   ui_property_item_set(ui_block_property_get(ap), item_name);
}

static void
_on_part_item_unselected(void *data,
                         Evas_Object *obj __UNUSED__,
                         void *event_data)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   Eina_Stringshare *item_name = (Eina_Stringshare *) event_data;
   workspace_edit_object_part_item_selected_set(ap->workspace, item_name, false);
   ui_property_item_unset(ui_block_property_get(ap));
}

Eina_Bool
add_callbacks_wd(Evas_Object *wd_list, App_Data *ap)
{
   assert(wd_list != NULL);
   assert(ap != NULL);

   evas_object_smart_callback_add(wd_list, "wl,style,select", _on_style_clicked, ap);
   evas_object_smart_callback_add(wd_list, "wl,part,select", _on_part_selected, ap);
   evas_object_smart_callback_add(wd_list, "wl,part,back", _on_part_back, ap);
   evas_object_smart_callback_add(wd_list, "wl,style,back", _on_style_back, ap);
   evas_object_smart_callback_add(wd_list, "wl,part,item,select",
                                  _on_part_item_selected, ap);
   evas_object_smart_callback_add(wd_list, "wl,part,item,unselect",
                                  _on_part_item_unselected, ap);

   return true;
}

static void
_panes_pos_setup(Evas_Object *panes, double value, Eina_Bool fixed)
{
   assert(panes != NULL);

   elm_panes_content_left_size_set(panes, value);
   elm_panes_fixed_set(panes, fixed);
}

Eina_Bool
code_edit_mode_switch(App_Data *ap, Eina_Bool is_on)
{
   Config *config;
   double center = 0.0,
          left = 0.0,
          right_hor = 0.0;

   assert(ap != NULL);

    if (is_on)
      {
         if (!config_panes_sizes_data_update(ap)) return false;
      }
    else
      {
         config = config_get();

         assert(config != NULL);

         center = config->panes.center;
         left = config->panes.left;
         right_hor = config->panes.right_hor;
      }

   _panes_pos_setup(ap->panes.center, center, is_on);
   _panes_pos_setup(ap->panes.left, left, is_on);
   _panes_pos_setup(ap->panes.right_hor, right_hor, is_on);
   ui_panes_left_panes_min_size_toggle(ap, !is_on);

   return true;
}
