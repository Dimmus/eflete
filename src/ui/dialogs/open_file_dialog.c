/* Edje Theme Editor
* Copyright (C) 2013 Samsung Electronics.
*
* This file is part of Edje Theme Editor.
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
* along with this program; If not, see http://www.gnu.org/licenses/gpl-2.0.html.
*/

#include "open_file_dialog.h"

struct _fs_entries
{
   char *project_name;
   Evas_Object *edc;
   Evas_Object *edj;
   Evas_Object *id;
   Evas_Object *fd;
   Evas_Object *sd;
   Evas_Object *log;

   Evas_Object *parent;
};

typedef struct _fs_entries fs_entries;

static fs_entries *fs_ent = NULL;

static void
_on_part_back(void *data,
              Evas_Object *obj __UNUSED__,
              void *event_data __UNUSED__)
{
   App_Data *ap = (App_Data *)data;
   ui_part_back(ap);
}

static void
_on_group_back(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_data __UNUSED__)
{
   App_Data *ap = (App_Data *)data;
   ui_group_back(ap);
}

static void
_on_state_selected(void *data,
                   Evas_Object *obj,
                   void *event_info)
{
   App_Data *ap = (App_Data *)data;
   Elm_Object_Item *eoit = (Elm_Object_Item *)event_info;
   Eina_Stringshare *state = elm_object_item_data_get(eoit);
   ui_state_select(ap, obj, state);
}

static void
_on_part_selected(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_data)
{
   App_Data *ap = (App_Data *)data;
   Part *part = (Part *) event_data;
   Evas_Object *gl_states = ui_part_select(ap, part);

   if (gl_states)
     {
        evas_object_smart_callback_add(gl_states, "stl,state,select",
                                       _on_state_selected, ap);
        evas_object_smart_callback_call(gl_states, "stl,state,select",
                                        elm_genlist_first_item_get(gl_states));
     }
}

static void
_on_group_clicked(void *data,
                         Evas_Object *obj __UNUSED__,
                         void *event_data)
{
   App_Data *ap = (App_Data *)data;
   Group *_group = (Group *)event_data;
   ui_group_clicked(ap, _group);
}

static void
_on_cancel_cb(void *data,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;
   if (ap)
     {
        if (ap->project)
          {
             ui_menu_base_disabled_set(ap->menu_hash, false);
             ui_menu_locked_set(ap->menu_hash, true);
          }
        else
          {
             ui_menu_base_disabled_set(ap->menu_hash, true);
             ui_menu_locked_set(ap->menu_hash, false);
          }
     }
   loop_quit(false);
}

static void
_on_edj_done(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *wd_list;
   const char *selected = (const char *)event_info;
   App_Data *ap = (App_Data *)data;

   if (selected)
     {
        if (eina_str_has_suffix(selected, ".edj"))
          {
             wd_list = ui_edj_load_done(ap, selected);
             if (!wd_list) return;
             add_callbacks_wd(wd_list, ap);
          }
        else
          {
             NOTIFY_ERROR("The file must have a extension '.edj'");
             return;
          }
     }

   evas_object_hide(elm_object_parent_widget_get(obj));
   loop_quit(false);
}

Eina_Bool
open_edj_file(App_Data *ap)
{
   Evas_Object *fs;

   if ((!ap) || (!ap->win)) return EINA_FALSE;

   Evas_Object *inwin = mw_add(_on_cancel_cb, ap);
   OPEN_DIALOG_ADD(inwin, fs, "Open EDJ file dialog");
   evas_object_smart_callback_add(fs, "done", _on_edj_done, ap);
   evas_object_smart_callback_add(fs, "activated", _on_edj_done, ap);

   elm_win_inwin_activate(inwin);

   loop_begin(NULL, NULL);

   evas_object_del(fs);
   evas_object_del(inwin);

   return true;
}

static void
_on_open_edj_cb(void *data,
                Evas_Object *obj,
                void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;
   Evas_Object *wd_list = NULL;

   const char *path_edj = elm_object_text_get(fs_ent->edj);

   wd_list = ui_edj_load_done(ap, path_edj);
   if (wd_list)
     {
        add_callbacks_wd(wd_list, ap);
        evas_object_hide(elm_object_parent_widget_get(obj));
        loop_quit(false);
     }
}

static void
_log_cb(time_t time,
       Eina_Stringshare* msg,
       int type __UNUSED__)
{
   elm_entry_cursor_end_set(fs_ent->log);
   static char buf[BUFF_MAX];
   struct tm *timeinfo;
   timeinfo = localtime(&time);
   if (type == 14) //stderr (almost all output from edje_cc)
      snprintf(buf, BUFF_MAX, "<br> %02d:%02d:%02d:    %s",
               timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, msg);
   else //stdout (SUMMARY)
      snprintf(buf, BUFF_MAX, "<br>     <b>%s</b>", msg);
   elm_entry_entry_insert(fs_ent->log, buf);
}

static void
_on_compile_cb(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   const char *path_edc = elm_object_text_get(fs_ent->edc);
   const char *path_edj = elm_object_text_get(fs_ent->edj);
   const char *path_id = elm_object_text_get(fs_ent->id);
   const char *path_sd = elm_object_text_get(fs_ent->sd);
   const char *path_fd = elm_object_text_get(fs_ent->fd);

   if (!eina_str_has_suffix(path_edc, ".edc"))
     {
        NOTIFY_ERROR("Input file must have an extension '.edc'");
        return;
     }
   if (!eina_str_has_suffix(path_edj, ".edj"))
     {
        NOTIFY_ERROR("Output file must have an extension '.edj'");
        return;
     }
   Evas_Object *bt = (Evas_Object *) data;
   elm_object_disabled_set(bt, true);

   elm_entry_cursor_end_set(fs_ent->log);
   elm_entry_entry_insert(fs_ent->log, "<b>Compilation started...</b>");
   int exit_code = compile(path_edc, path_edj, path_id, path_fd, path_sd, _log_cb);
   elm_entry_cursor_end_set(fs_ent->log);
   elm_entry_entry_insert(fs_ent->log, "<br><br>");
   elm_object_disabled_set(bt, false);
   DBG("Compile exit code: %d", exit_code);
}

void
add_callbacks_wd(Evas_Object *wd_list, App_Data *ap)
{
   if (!wd_list) return;

   evas_object_smart_callback_add(wd_list, "wl,group,select", _on_group_clicked, ap);
   evas_object_smart_callback_add(wd_list, "wl,part,select", _on_part_selected, ap);
   evas_object_smart_callback_add(wd_list, "wl,part,back", _on_part_back, ap);
   evas_object_smart_callback_add(wd_list, "wl,group,back", _on_group_back, ap);
}

static void
_on_compile_edc_done(void *data,
             Evas_Object *obj,
             void *event_info)
{
   const char *selected = event_info;
   char *edj;
   char tmp_path[PATH_MAX];
   Eina_Array *array;
   int i, j;

   if ((!data) || (!selected))
     {
        evas_object_del(obj);
        loop_quit(false);
        return;
     }

   if (eina_str_has_suffix(selected, ".edc"))
     {
        array = eina_file_split(strdup(selected));

        i = eina_array_count(array);
        eina_strlcpy(tmp_path, "/", sizeof("/"));
        for (j = 0; j < i - 1; j++)
          {
             eina_strlcat(tmp_path, eina_array_data_get(array, j), PATH_MAX);
             eina_strlcat(tmp_path, "/", PATH_MAX);
          }
        fs_ent->project_name = strdup(eina_array_data_get(array, i - 1));
        eina_array_free(array);

        elm_object_text_set(fs_ent->id, tmp_path);
        elm_object_text_set(fs_ent->sd, tmp_path);
        elm_object_text_set(fs_ent->fd, tmp_path);

        elm_object_text_set(fs_ent->edc, selected);
        edj = strdup(selected);
        edj[strlen(selected) - 1] = 'j';
        elm_object_text_set(fs_ent->edj, edj);
        free(edj);

        loop_quit(false);
     }
   else
     NOTIFY_ERROR("The file must have an extension '.edc'");
}

static void
_on_path_done(void *data,
             Evas_Object *obj __UNUSED__,
             void *event_info)
{
   const char *selected = (const char *)event_info;
   Evas_Object *entry = (Evas_Object *)data;
   if ((data) && (selected))
     elm_object_text_set(entry, selected);

   loop_quit(false);
}

static void
_edx_select(void *data,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   Evas_Object *fs;
   Eina_Bool edc = (Evas_Object *)data == fs_ent->edc;
   Evas_Object *entry;

   entry = (edc) ? fs_ent->edc : fs_ent->edj;

   const char *path = elm_object_text_get(entry);

   if (!ecore_file_exists(path))
      path = getenv("HOME");

   if (!fs_ent->parent) return;

   Evas_Object *inwin;
   inwin = mw_add(NULL, NULL);

   OPEN_DIALOG_ADD(inwin, fs, evas_object_data_get(entry, FS_TITLE));
   elm_fileselector_selected_set(fs, path);
   if (edc)
     {
        evas_object_smart_callback_add(fs, "done", _on_compile_edc_done, fs);
        evas_object_smart_callback_add(fs, "activated", _on_compile_edc_done, fs);
     }
   else
     {
        elm_fileselector_is_save_set(fs, true);
        evas_object_smart_callback_add(fs, "done", _on_path_done, fs_ent->edj);
        evas_object_smart_callback_add(fs, "activated", _on_path_done, fs_ent->edj);
     }

   elm_win_inwin_activate(inwin);

   loop_begin(NULL, NULL);

   evas_object_del(fs);
   evas_object_del(inwin);
}

static void
_clean_log(void *data,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   Evas_Object *entry = (Evas_Object *)data;
   elm_object_text_set(entry, "");
}

static void
_path_select(void *data,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   Evas_Object *fs;
   Evas_Object *entry = (Evas_Object *)data;
   const char *path = elm_object_text_get(entry);

   if (!ecore_file_exists(path))
     path = getenv("HOME");

   if (!fs_ent->parent) return;

   Evas_Object *inwin = mw_add(NULL, NULL);
   OPEN_DIALOG_ADD(inwin, fs, evas_object_data_get(entry, FS_TITLE));
   elm_fileselector_path_set(fs, path);
   elm_fileselector_folder_only_set(fs, EINA_TRUE);
   evas_object_smart_callback_add(fs, "done", _on_path_done, entry);

   elm_win_inwin_activate(inwin);

   loop_begin(NULL, NULL);

   evas_object_del(fs);
   evas_object_del(inwin);
}

Eina_Bool
open_edc_file(App_Data *ap)
{
   Evas_Object *button_box, *layout;
   Evas_Object *label, *fs_entry;
   Evas_Object *bt;

   if ((!ap) || (!ap->win))
     {
        ERR("App Data or Window is NULL. Something is wrong.");
        return EINA_FALSE;
     }

   Evas_Object *inwin = mw_add(_on_cancel_cb, ap);
   mw_title_set(inwin, "Compile EDC file");
   evas_object_focus_set(inwin, EINA_TRUE);

   if (!fs_ent)
     fs_ent = mem_malloc(sizeof(fs_entries));
   fs_ent->parent = ap->win;
   fs_ent->project_name = NULL;

   layout = elm_layout_add(inwin);
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_layout_file_set(layout, TET_EDJ, "compile_dialog");

   #define _BUTTON_ADD(box, text, func, data) \
      bt = elm_button_add(box); \
      elm_object_text_set(bt, text); \
      elm_object_style_set(bt, DEFAULT_STYLE); \
      evas_object_smart_callback_add(bt, "clicked", func, data); \
      evas_object_show(bt); \
      elm_box_pack_end(box, bt);

   #define _ITEM_ADD(parent, label_text, button_text, title, _fs, func, data) \
      LABEL_ADD(inwin, label, label_text) \
      fs_entry = elm_entry_add(parent); \
      elm_entry_single_line_set(fs_entry, true); \
      elm_object_style_set(fs_entry, DEFAULT_STYLE); \
      evas_object_data_set(fs_entry, FS_TITLE, title); \
      elm_object_text_set(fs_entry, getenv("HOME")); \
      evas_object_size_hint_weight_set(fs_entry, EVAS_HINT_EXPAND, 0.0); \
      evas_object_size_hint_align_set(fs_entry, EVAS_HINT_FILL, EVAS_HINT_FILL); \
      elm_entry_scrollable_set(fs_entry, true); \
      _fs = fs_entry; \
      evas_object_show(fs_entry); \
      _BUTTON_ADD(parent, button_text, func, data);

   _ITEM_ADD(inwin, "Path to EDC:", "[Select]", "Select EDC file",
         fs_ent->edc, _edx_select, fs_ent->edc);
   elm_object_part_content_set(layout, "label_edc", label);
   elm_object_part_content_set(layout, "button_edc", bt);
   elm_object_part_content_set(layout, "entry_edc", fs_ent->edc);

   _ITEM_ADD(inwin, "Path to EDJ:", "[Select]", "Select EDJ file",
         fs_ent->edj, _edx_select, fs_ent->edj);
   elm_object_part_content_set(layout, "label_edj", label);
   elm_object_part_content_set(layout, "button_edj", bt);
   elm_object_part_content_set(layout, "entry_edj", fs_ent->edj);

   _ITEM_ADD(inwin, "Image directory:", "[Select]", "Select images directory",
         fs_ent->id, _path_select, fs_ent->id);
   elm_object_part_content_set(layout, "label_img", label);
   elm_object_part_content_set(layout, "button_img", bt);
   elm_object_part_content_set(layout, "entry_img", fs_ent->id);

   _ITEM_ADD(inwin, "Sound directory:", "[Select]", "Select sounds directory",
         fs_ent->sd, _path_select, fs_ent->sd);
   elm_object_part_content_set(layout, "label_snd", label);
   elm_object_part_content_set(layout, "button_snd", bt);
   elm_object_part_content_set(layout, "entry_snd", fs_ent->sd);

   _ITEM_ADD(inwin, "Font directory:", "[Select]", "Select fonts directory",
         fs_ent->fd, _path_select, fs_ent->fd);
   elm_object_part_content_set(layout, "label_fnt", label);
   elm_object_part_content_set(layout, "button_fnt", bt);
   elm_object_part_content_set(layout, "entry_fnt", fs_ent->fd);

   _ITEM_ADD(inwin, "Log:", "[Clean]", "",
         fs_ent->log, _clean_log, fs_ent->log);
   elm_object_part_content_set(layout, "label_log", label);
   elm_object_part_content_set(layout, "button_log", bt);
   elm_object_part_content_set(layout, "entry_log", fs_ent->log);

   elm_entry_single_line_set(fs_ent->log, false);
   elm_entry_editable_set(fs_ent->log, false);
   elm_scroller_policy_set(fs_ent->log, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_ON);
   elm_entry_entry_set(fs_ent->log, "");
   elm_entry_cursor_end_set(fs_ent->log);
   #undef _ITEM_ADD

   BOX_ADD(inwin, button_box, true, true);

   _BUTTON_ADD(button_box, "Compile", _on_compile_cb, bt);
   _BUTTON_ADD(button_box, "Open EDJ", _on_open_edj_cb, ap);
   _BUTTON_ADD(button_box, "Cancel", _on_cancel_cb, ap);
   #undef _BUTTON_ADD

   elm_object_part_content_set(layout, "button_box", button_box);

   elm_win_inwin_content_set(inwin, layout);
   elm_win_inwin_activate(inwin);

   loop_begin(NULL, NULL);

   free(fs_ent->project_name);
   free(fs_ent);
   fs_ent = NULL;
   evas_object_del(inwin);

   return EINA_TRUE;
}
