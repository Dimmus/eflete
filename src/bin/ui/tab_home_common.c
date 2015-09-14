/*
 * Edje Theme Editor
 * Copyright (C) 2013-2015 Samsung Electronics.
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

#include "tabs_private.h"
#include "tabs.h"
#include "signals.h"

Eina_Bool
progress_print(void *data __UNUSED__, Eina_Stringshare *progress_string)
{
   elm_object_part_text_set(ap.splash, "label.info", progress_string);
   return true;
}

void
progress_end(void *data __UNUSED__, PM_Project_Result result)
{

   switch (result)
     {
      case PM_PROJECT_ERROR:
        {
           NOTIFY_INFO(3, _("Can't open project."));
           break;
        }
      case PM_PROJECT_CANCEL:
        {
           NOTIFY_INFO(3, _("Project opening canceled."));
           break;
        }
      case PM_PROJECT_SUCCESS:
        {
           ap.project = pm_project_thread_project_get();
           assert(ap.project);

           ui_menu_items_list_disable_set(ap.menu, MENU_ITEMS_LIST_BASE, false);
           ui_menu_disable_set(ap.menu, MENU_FILE_CLOSE_PROJECT, false);
           navigator_project_set();
           tabs_menu_tab_open(TAB_HOME_PROJECT_INFO);

           NOTIFY_INFO(3, _("Project '%s' is opened."), ap.project->name);
           STATUSBAR_PROJECT_PATH(ap.project->pro_path);
           STATUSBAR_PROJECT_SAVE_TIME_UPDATE();

           config_recent_add(ap.project->name, ap.project->pro_path);
           _tab_open_project_recents_update();
           evas_object_smart_callback_call(ap.win, SIGNAL_PROJECT_OPENED, NULL);
           break;
        }
      default:
        {
           ERR("Wrong result");
           abort();
        }
     }

   splash_del(ap.splash);
   ap.splash = NULL;
}

void
elipsis_btn_add(Evas_Object *entry, Evas_Smart_Cb cb_func, void *data)
{
   Evas_Object *bt;

   assert(entry != NULL);

   bt = elm_button_add(entry);
   elm_object_style_set(bt, "elipsis");
   elm_object_focus_allow_set(bt, false);
   evas_object_show(bt);
   evas_object_smart_callback_add(bt, "clicked", cb_func, data);
   elm_object_part_content_set(entry, "elm.swallow.elipsis", bt);
}

void
exist_permission_check(const char *path, const char *name, const char *title)
{
   Eina_Strbuf *buf, *buf_msg;
   Popup_Button btn_res;

   /* we alwayes imported and exported project to folder by given path, means
    * that we alwayes create a new folder for project or exported source.
    * So need to check there is the folder "path/name" */
   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s/%s", path, name);
   if (!ecore_file_exists(eina_strbuf_string_get(buf))) return;
   buf_msg = eina_strbuf_new();
   eina_strbuf_append_printf(buf_msg,
                             _("<font_size=16>A project folder named '%s' already exist."
                               "Do you want to replace it?</font_size><br>"
                               "The project folder '%s' already exist in '%s'. Replacing it will overwrite"
                               "<b>all</b> contents."), name, name, path);
   btn_res = popup_want_action(title, eina_strbuf_string_get(buf_msg),
                               NULL, BTN_REPLACE | BTN_CANCEL);
   if (btn_res == BTN_CANCEL) return;
   if (!ecore_file_can_write(eina_strbuf_string_get(buf)))
     {
        eina_strbuf_reset(buf_msg);
        eina_strbuf_append_printf(buf_msg, _("Haven't permision to overwrite '%s' in '%s'"), name, path);
        popup_want_action("New project", eina_strbuf_string_get(buf_msg), NULL, BTN_OK);
     }
   ecore_file_recursive_rm(eina_strbuf_string_get(buf));
   eina_strbuf_free(buf_msg);
   eina_strbuf_free(buf);
}
