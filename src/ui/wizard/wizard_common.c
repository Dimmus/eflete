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

#include "wizard_common.h"

static Elm_Entry_Filter_Accept_Set accept_name = {
   .accepted = NULL,
   .rejected = STYLE_NAME_BANNED_SYMBOLS
};

static void
_on_cancel(void *data,
           Evas_Object *obj __UNUSED__,
           void *event_info __UNUSED__)
{
   Wizard_Import_Edj_Win *wiew;
   wiew = (Wizard_Import_Edj_Win *)data;

   mw_del(wiew->win);
   free(wiew);
}

void
_fs_close(void *data __UNUSED__,
          Evas_Object *obj,
          void *event_info __UNUSED__)
{
   evas_object_del(obj);
}

FILESELCTOR_WINDOW(_on_path_bt, _("Select path for new project"), true, false, NULL, path)

/************************** Project import functions **************************/

Eina_Bool
_progress_print(void *data, Eina_Stringshare *progress_string)
{
   Wizard_Import_Edj_Win *wiew;
   wiew = (Wizard_Import_Edj_Win *)data;
   elm_object_part_text_set(wiew->splash, "label.info", progress_string);

   if (wiew->progress_log)
     eina_strbuf_append_printf(wiew->progress_log,
                               "%s<br>", progress_string);
   return true;
}

void
_progress_end(void *data, PM_Project_Result result)
{
   Wizard_Import_Edj_Win *wiew;
   Project *pro;
   App_Data *ap;

   wiew = (Wizard_Import_Edj_Win *)data;
   ap = app_data_get();

   if (result == PM_PROJECT_SUCCESS)
     {
        pro = pm_project_thread_project_get(wiew->thread);
        ap->project = pro;

        wm_widgets_list_objects_load(pro->widgets,
                                     evas_object_evas_get(ap->win),
                                     pro->dev);
        wm_layouts_list_objects_load(pro->layouts,
                                     evas_object_evas_get(ap->win),
                                     pro->dev);

        blocks_show(ap);

        ui_menu_disable_set(ap->menu, MENU_FILE_CLOSE_PROJECT, false);
        if (!eina_inlist_count(ap->project->widgets))
          ui_widget_list_tab_activate(ui_block_widget_list_get(ap), 1);

        STATUSBAR_PROJECT_PATH(ap, eet_file_get(ap->project->pro));
        STATUSBAR_PROJECT_SAVE_TIME_UPDATE(ap);

     }

   ecore_file_recursive_rm(wiew->tmp_dir_path);

   if (result == PM_PROJECT_CANCEL)
     {
       DBG("Canceled by user! 'thread: %p'", wiew->thread);
     }
   else if (result == PM_PROJECT_ERROR)
     {
        if (wiew->progress_log)
          {
             // TODO: add new style of notify with align to the left and smth like "IMPORT ERROR" header
             NOTIFY_ERROR(_("Errors occured on importing: <br><br>%s"),
                          eina_strbuf_string_get(wiew->progress_log));
          }
        else
          {
             NOTIFY_ERROR(_("Errors occured on importing."));
          }
     }

   if (wiew->progress_log)
     {
        eina_strbuf_free(wiew->progress_log);
        wiew->progress_log = NULL;
     }

   splash_del(wiew->splash);
}

static Eina_Bool
_teardown_splash(void *data, Splash_Status status)
{
   Wizard_Import_Edj_Win *wiew;
   wiew = (Wizard_Import_Edj_Win *)data;
   App_Data *app = app_data_get();

   mw_del(wiew->win);
   if ((status == SPLASH_SUCCESS) && (app->project))
     {
        STATUSBAR_PROJECT_PATH(app, eet_file_get(app->project->pro));
        STATUSBAR_PROJECT_SAVE_TIME_UPDATE(app);
     }
   else return false;
   return true;
}

/******************************************************************************/
static Eina_Strbuf *
_path_to_project_build(Wizard_Import_Edj_Win *wiew)
{
   Eina_Strbuf *path_to_project = eina_strbuf_new();

   // remove trailing slashes
   char *path = strdup(elm_entry_entry_get(wiew->path));
   char *path_end = path + strlen(path) -1;
   while (*path_end == '/')
     {
        *path_end = '\0';
        path_end--;
     }

   eina_strbuf_append_printf(path_to_project, "%s/%s",
                             path, elm_entry_entry_get(wiew->name));

   free(path);
   return path_to_project;
}

static Eina_Bool
_project_directory_check(Wizard_Import_Edj_Win *wiew)
{
   Eina_Strbuf *path_to_project = _path_to_project_build(wiew);
   Eina_Strbuf *request_str = NULL;
   Eina_Bool ret = true;

   if ((ecore_file_exists(eina_strbuf_string_get(path_to_project))) &&
       (ecore_file_is_dir(eina_strbuf_string_get(path_to_project))))
     {
        request_str = eina_strbuf_new();
        eina_strbuf_append_printf(request_str,
                                  _("The <color=#80BFFF>'%s'</color> directory "
                                    "already exists.<br>Do you want to "
                                    "<b><color=#FFBF80>delete all</color></b>"
                                    " contents of this folder and create"
                                    "new project in it?"),
                                  eina_strbuf_string_get(path_to_project));

        ret = export_replace_request(wiew->win,
                                     eina_strbuf_string_get(request_str));

        if (ret)
          {
             ret = ecore_file_recursive_rm(eina_strbuf_string_get(path_to_project));
             if (!ret) NOTIFY_ERROR(_("Can not delete folder %s!"),
                                    eina_strbuf_string_get(path_to_project));
          }
     }
   if (request_str)
     eina_strbuf_free(request_str);
   eina_strbuf_free(path_to_project);

   return ret;
}

static Eina_Bool
_required_fields_check(Wizard_Import_Edj_Win *wiew)
{
   if (elm_entry_is_empty(wiew->name))
     {
        NOTIFY_WARNING(_("Please enter the name of the project"));
        return false;
     }
   if ((wiew->edj) && (elm_entry_is_empty(wiew->edj)))
     {
        NOTIFY_WARNING(_("Please enter the name of the file for exporting"));
        return false;
     }
   if (elm_entry_is_empty(wiew->path))
     {
        NOTIFY_WARNING(_("Please enter the path to the project"));
        return false;
     }
   if ((wiew->edj) && !ecore_file_exists(elm_entry_entry_get(wiew->edj)))
     {
        NOTIFY_WARNING(_("The path to file for importing does not exist!"));
        return false;
     }
   if (!ecore_file_is_dir(elm_entry_entry_get(wiew->path)))
     {
        //TODO: create folder for project if user entered path that does not exist yet
        NOTIFY_WARNING(_("Wrong path to the project!<br>"
                         "Please path to existing folder."));
        return false;
     }
   return true;
}

static Eina_Bool
_cancel_splash(void *data, Splash_Status status __UNUSED__)
{
   Wizard_Import_Edj_Win *wiew;

   wiew = (Wizard_Import_Edj_Win *)data;
   return pm_project_thread_cancel(wiew->thread);
}

static void
_on_apply(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   App_Data *ap;
   Wizard_Import_Edj_Win *wiew;

   ap = app_data_get();
   wiew = (Wizard_Import_Edj_Win *)data;

   if ((!_required_fields_check(wiew)) || (!wiew->splash_setup_func) ||
       (!_project_directory_check(wiew)))
     return;

   wiew->splash = splash_add(ap->win, wiew->splash_setup_func,
                             _teardown_splash, _cancel_splash, wiew);
   evas_object_focus_set(wiew->splash, true);
   evas_object_show(wiew->splash);
}

static void
_elipsis_btn_add(Evas_Object *entry, Evas_Smart_Cb cb_func, void *data)
{
   Evas_Object *bt;

   bt = elm_button_add(entry);
   elm_object_style_set(bt, "eflete/elipsis");
   elm_object_focus_allow_set(bt, false);
   evas_object_show(bt);
   evas_object_smart_callback_add(bt, "clicked", cb_func, data);
   elm_object_part_content_set(entry, "elm.swallow.end", bt);
}

void
project_path_item_add(Wizard_Import_Edj_Win *wiew,
                      const char *label,
                      Evas_Smart_Cb cb_func)
{
   elm_object_part_text_set(wiew->layout, "label.edj", label);

   ENTRY_ADD(wiew->layout, wiew->edj, true, DEFAULT_STYLE)
   elm_object_part_content_set(wiew->layout, "swallow.edj", wiew->edj);

   _elipsis_btn_add(wiew->edj, cb_func, wiew);
}

Wizard_Import_Edj_Win *
wizard_import_common_add(const char *layout_name)
{
   Evas_Object *mwin, *layout;
   Evas_Object *bt;
   Wizard_Import_Edj_Win *wiew;

   wiew = (Wizard_Import_Edj_Win *)mem_malloc(sizeof(Wizard_Import_Edj_Win));

   mwin = mw_add(_on_cancel, wiew);
   if (!mwin) return NULL;

   wiew->win = mwin;

   layout = elm_layout_add(mwin);
   elm_layout_file_set(layout, EFLETE_EDJ, layout_name);

   elm_object_content_set(mwin, layout);
   evas_object_show(layout);

   BUTTON_ADD(layout, bt, _("Apply"))
   elm_object_part_content_set(layout, "swallow.button1", bt);
   evas_object_smart_callback_add(bt, "clicked", _on_apply, wiew);
   BUTTON_ADD(layout, bt, _("Cancel"))
   elm_object_part_content_set(layout, "swallow.button2", bt);
   evas_object_smart_callback_add(bt, "clicked", _on_cancel, wiew);

   //label.name
   elm_object_part_text_set(layout, "label.name", _("Project name:"));
   EWE_ENTRY_ADD(layout, wiew->name, true, DEFAULT_STYLE)
   elm_entry_markup_filter_append(wiew->name,
                                  elm_entry_filter_accept_set, &accept_name);
   elm_object_part_content_set(layout, "swallow.name", wiew->name);
   //label.path
   elm_object_part_text_set(layout, "label.path", _("Path to project:"));
   ENTRY_ADD(layout, wiew->path, true, DEFAULT_STYLE)
   elm_object_part_content_set(layout, "swallow.path", wiew->path);
   elm_entry_entry_set(wiew->path, profile_get()->general.projects_folder);

   _elipsis_btn_add(wiew->path, _on_path_bt, wiew);

   //label.meta_version
   elm_object_part_text_set(layout, "label.meta_version", _("Version of file:"));
   ENTRY_ADD(layout, wiew->meta_version, true, DEFAULT_STYLE)
   elm_object_part_content_set(layout, "swallow.meta_version", wiew->meta_version);
   //label.meta_authors
   elm_object_part_text_set(layout, "label.meta_authors", _("Authors:"));
   ENTRY_ADD(layout, wiew->meta_authors, false, DEFAULT_STYLE)
   elm_object_part_content_set(layout, "swallow.meta_authors", wiew->meta_authors);
   //label.meta_licenses
   elm_object_part_text_set(layout, "label.meta_licenses", _("Licenses:"));
   ENTRY_ADD(layout, wiew->meta_licenses, false, DEFAULT_STYLE)
   elm_object_part_content_set(layout, "swallow.meta_licenses", wiew->meta_licenses);
   //label.meta_comment
   elm_object_part_text_set(layout, "label.meta_comment", _("Comment:"));
   ENTRY_ADD(layout, wiew->meta_comment, false, DEFAULT_STYLE)
   elm_object_part_content_set(layout, "swallow.meta_comment", wiew->meta_comment);
   elm_entry_entry_set(wiew->meta_comment, _("Created with Eflete!"));

   wiew->layout = layout;

   evas_object_show(mwin);

   return wiew;
}

