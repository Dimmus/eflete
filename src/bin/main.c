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

#include <Ecore_Getopt.h>
#include <regex.h>
#include "main_window.h"

static char *open = NULL;
static char *import_edj = NULL;
static char *pro_name = NULL;
static char *pro_path = NULL;
static Eina_Bool pro_replace = false;

static const Ecore_Getopt options = {
   PACKAGE_NAME,
   "%prog [options]",
   VERSION,
   "(C) 2013-2014 Samsung Electronics.",
   "GNU Library General Public License version 2",
   N_("This application was written for Enlightenment, to use EFL\n"
   "and design to create and modify Elementary widgets styles.\n"),
   EINA_TRUE,
   {
      ECORE_GETOPT_STORE_STR('o', "open", N_("Eflete project file")),
      ECORE_GETOPT_STORE_STR(0, "import-edj", N_("Import the edj file as new project")),
      ECORE_GETOPT_STORE_STR(0, "name", N_("Name for new project that would be created in import process")),
      ECORE_GETOPT_STORE_STR(0, "path", N_("Path for project")),
      ECORE_GETOPT_STORE_TRUE(0, "replace", N_("Replace existing project")),
      ECORE_GETOPT_VERSION  ('v', "version"),
      ECORE_GETOPT_COPYRIGHT('c', "copyright"),
      ECORE_GETOPT_LICENSE  ('l', "license"),
      ECORE_GETOPT_HELP     ('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};

void
_import_end(void *data __UNUSED__, PM_Project_Result result)
{
   Project *pro;
   App_Data *ap;

   ap = app_data_get();

   if (result == PM_PROJECT_SUCCESS)
     {
        pro = pm_project_thread_project_get(ap->pr_thread);
        ap->project = pro;

        wm_widgets_list_objects_load(pro->widgets,
                                     evas_object_evas_get(ap->win),
                                     pro->mmap_file);
        wm_layouts_list_objects_load(pro->layouts,
                                     evas_object_evas_get(ap->win),
                                     pro->mmap_file);
        wm_styles_build_alias(pro->widgets,
                              pro->layouts);

        blocks_show(ap);

        if (!eina_inlist_count(ap->project->widgets))
          ui_widget_list_tab_activate(ui_block_widget_list_get(ap), 1);

        STATUSBAR_PROJECT_PATH(ap, ap->project->pro_path);
        STATUSBAR_PROJECT_SAVE_TIME_UPDATE(ap);

        NOTIFY_INFO(3, _("Project '%s' is opened."), pro->name);
     }
     evas_object_show(ap->win);
}

Eina_Bool
_message_print(void *data __UNUSED__, Eina_Stringshare *progress_string)
{
   fprintf(stdout, "%s\n", progress_string);
   return true;
}

static void
_import_edj(void *data __UNUSED__)
{
   /* Ugly hack, but we need to do this.
    * When we try to import edj file while ecore main loop not fully started
    * we get a freeze. So for use the ecore and eina threads in the project
    * manager need to itarate the main loop for initialize it.
    * DO NOT DELETE: whith out iterate import from command line not worked! */
   ecore_main_loop_iterate();

   App_Data *ap = app_data_get();
   ap->pr_thread = pm_project_import_edj(pro_name, pro_path, import_edj,
                                         _message_print, _import_end, NULL);

}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Eina_Bool info_only = false;
   Eina_Stringshare *pro_folder;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(open),
     ECORE_GETOPT_VALUE_STR(import_edj),
     ECORE_GETOPT_VALUE_STR(pro_name),
     ECORE_GETOPT_VALUE_STR(pro_path),
     ECORE_GETOPT_VALUE_BOOL(pro_replace),
     ECORE_GETOPT_VALUE_BOOL(info_only),
     ECORE_GETOPT_VALUE_BOOL(info_only),
     ECORE_GETOPT_VALUE_BOOL(info_only),
     ECORE_GETOPT_VALUE_BOOL(info_only),
     ECORE_GETOPT_VALUE_NONE
   };

   if (!app_init()) return -1;

#ifdef HAVE_ENVENTOR
   elm_app_compile_data_dir_set(EFLETE_EDJ_PATH);
   elm_app_info_set(NULL, EFLETE_EDJ_PATH, NULL);
   enventor_init(argc, argv);
#endif

   ecore_getopt_parse(&options, values, argc, argv);
   if (!info_only)
     {
#ifdef HAVE_CONFIG_H
        INFO("%s: %s - Started...", PACKAGE_NAME, VERSION);
#else
        CRIT("Could not find 'eflete_config.h'");
#endif

        App_Data *ap = app_data_get();
        if (!ui_main_window_add(ap))
          {
             app_shutdown();
             return -1;
          }

        if (open)
          {
             if ((eina_str_has_suffix(open, ".pro")) &&
                 (ecore_file_exists(open)))
               {
                  ap->project = pm_project_open(open);
                  blocks_show(ap);
                  wm_widgets_list_objects_load(ap->project->widgets,
                                               evas_object_evas_get(ap->win),
                                               ap->project->mmap_file);
                  wm_layouts_list_objects_load(ap->project->layouts,
                                               evas_object_evas_get(ap->win),
                                               ap->project->mmap_file);
                  wm_styles_build_alias(ap->project->widgets,
                                        ap->project->layouts);

                  if (!eina_inlist_count(ap->project->widgets))
                    ui_widget_list_tab_activate(ui_block_widget_list_get(ap), 1);
                  evas_object_show(ap->win);
               }
             else
               {
                  ERR(_("Can not open file '%s'. Wrong path or file format."), open);
                  return 1;
               }
          }
        if (import_edj)
          {
            if (!ecore_file_exists(import_edj))
              {
                 fprintf(stderr, N_("ERROR: cann't find file: '%s'.\n"), import_edj);
                 return -1;
              }
            if (!pro_name)
              {
                 fprintf(stderr, N_("ERROR: missing name for imported project.\n"));
                 fprintf(stderr, N_("ERROR: invalid options found. See --help.\n"));
                 return -1;
              }
            else
              {
                 regex_t regex;
                 regcomp(&regex, NAME_REGEX, REG_EXTENDED | REG_NOSUB);
                 if (regexec(&regex, pro_name, (size_t)0, NULL, 0))
                   {
                      fprintf(stderr, N_("ERROR: invalid name. Name can contain only next characters: a-zA-z0-9_.\n"));
                      return -1;
                   }
                 regfree(&regex);
              }
            if (!pro_path) pro_path = eina_file_path_sanitize(".");
            pro_folder = eina_stringshare_printf("%s/%s", pro_path, pro_name);
            if (ecore_file_exists(pro_folder))
              {
                 if (!pro_replace)
                   {
                      fprintf(stderr, N_("ERROR: can't import file '%s'. Project '%s' exist.\n"),
                              import_edj, pro_folder);
                      fprintf(stderr, N_("ERROR: to replace existing project use option '--replace'.\n"));
                      fprintf(stderr, N_("ERROR: invalid options found. See --help.\n"));
                      return -1;
                   }
                 else  ecore_file_recursive_rm(pro_folder);
              }
            ecore_job_add(_import_edj, NULL);
            eina_stringshare_del(pro_folder); pro_folder = NULL;
          }
        else evas_object_show(ap->win);

        elm_run();
#ifdef HAVE_ENVENTOR
        enventor_shutdown();
#endif
        app_shutdown();
     }

   return 0;
}
ELM_MAIN();
