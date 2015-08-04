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
#define _GNU_SOURCE
#include "enventor_module.h"
#include "project_manager.h"
#include "alloc.h"
#ifndef _WIN32
#include <sys/wait.h>
#else
#include <win32.h>
#endif
#define PROJECT_FILE_KEY      "project"

#define PROJECT_KEY_NAME         "edje/name"
#define PROJECT_KEY_AUTHORS      "edje/authors"
#define PROJECT_KEY_FILE_VERSION "edje/file_version"
#define PROJECT_KEY_LICENSE      "edje/license"
#define PROJECT_KEY_COMMENT      "edje/comment"

#define S3 "   "
#define TOP_LEVEL_NUMBER 9
#define GROUP "   group "

static Eet_Compression compess_level = EET_COMPRESSION_HI;

const char *top_levels[] = { "collections",
                             "styles",
                             "color_classes",
                             "fonts",
                             "images",
                             "data",
                             "externals",
                             "sounds",
                             "group",
                             NULL};
typedef struct
{
   Eina_Stringshare *str;
   PM_Project_Progress_Cb func_print;
   void *data;
} Progress_Message;

#define WORKER_CREATE(FUNC_PROGRESS, FUNC_END, DATA, PROJECT, \
                      NAME, PATH, EDJ, EDC, BUILD_OPTIONS) \
{ \
   worker = (Project_Thread *)mem_calloc(1, sizeof(Project_Thread)); \
   worker->func_progress = FUNC_PROGRESS; \
   worker->func_end = FUNC_END; \
   worker->data = (void *)DATA; \
   worker->project = PROJECT; \
   worker->result = PM_PROJECT_LAST; \
   worker->name = eina_stringshare_add(NAME); \
   worker->path = eina_stringshare_add(PATH); \
   worker->edj = eina_stringshare_add(EDJ); \
   worker->edc = eina_stringshare_add(EDC); \
   worker->build_options = eina_stringshare_add(BUILD_OPTIONS); \
}

#define WORKER_FREE() \
{ \
   eina_stringshare_del(worker->name); \
   eina_stringshare_del(worker->path); \
   eina_stringshare_del(worker->edj); \
   eina_stringshare_del(worker->edc); \
   eina_stringshare_del(worker->build_options); \
   free(worker); \
   worker = NULL; \
}

#define THREAD_TESTCANCEL pthread_testcancel()

#define PROGRESS_SEND(FMT, ...) \
{ \
   if (worker->func_progress) \
      { \
         Progress_Message *message = mem_malloc(sizeof(Progress_Message)); \
         message->str = eina_stringshare_printf(FMT, ## __VA_ARGS__); \
         message->func_print = worker->func_progress; \
         message->data = worker->data; \
         ecore_main_loop_thread_safe_call_async(_progress_send, message); \
      } \
}

#define END_SEND(STATUS) \
{ \
   if (worker->func_end) \
     { \
        worker->result = STATUS; \
        ecore_main_loop_thread_safe_call_async(_end_send, worker); \
     } \
}

static Eina_Bool
_project_resource_export(Project *pro, const char* dir_path,
                         Project_Thread *worker);

static Eina_Bool
_image_resources_export(Project *project,
                        Eina_List *images, Eina_Stringshare *destination,
                        Eina_Stringshare *source, Eina_Stringshare *dev,
                        Evas_Object *edje_edit, Project_Thread *worker);

static Eina_Bool
_sound_resources_export(Eina_List *sounds, Eina_Stringshare *destination,
                        Eina_Stringshare *source, Evas_Object *edje_edit,
                        Project_Thread *worker);

static Eina_Bool
_font_resources_export(Eina_List *fonts, Eina_Stringshare *destination,
                       Eina_Stringshare *source, Eina_Stringshare *dev,
                       Evas_Object *edje_edit, Project_Thread *worker);

static Eina_Bool
_project_dev_file_create(Project *pro)
{
   Eina_Bool result;

   result = ecore_file_cp(pro->saved_edj, pro->dev);
   DBG("Create the .dev file.");
   return result;
}

static Eet_Data_Descriptor *eed_project = NULL;

static void
_project_descriptor_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   if (eed_project) return;

   eet_eina_stream_data_descriptor_class_set(&eddc, sizeof(eddc),
                                             "Project", sizeof(Project));
   eed_project = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(eed_project, Project, "version", version, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eed_project, Project, "dev", dev, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eed_project, Project, "saved_edj", saved_edj, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eed_project, Project, "develop_path", develop_path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(eed_project, Project, "release_options", release_options, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(eed_project, Project, "images", res.images);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(eed_project, Project, "sounds", res.sounds);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(eed_project, Project, "fonts", res.fonts);
}

static void
_pm_project_descriptor_shutdown(void)
{
   eet_data_descriptor_free(eed_project);
   eed_project = NULL;
}

static Eina_Bool
_pm_project_descriptor_data_write(const char *path, Project *project)
{
   Eina_Bool ok = false;

   Eet_File *ef = eet_open(path, EET_FILE_MODE_READ_WRITE);
   if (ef)
     ok = eet_data_write(ef, eed_project, PROJECT_FILE_KEY,
                         project, compess_level);
   eet_close(ef);

   return ok;
}

static void
_progress_send(void *data)
{
   Progress_Message *message;

   message = (Progress_Message *)data;
   assert(message != NULL);

   message->func_print(message->data, message->str);
   eina_stringshare_del(message->str);
   free(message);
}

static void
_end_send(void *data)
{
   Project_Thread *worker;
   PM_Project_End_Cb func;
   PM_Project_Result result;
   void *udata;

   worker = (Project_Thread *)data;

   assert(worker != NULL);

   /** Copy the links to callback and meesage, to fast release worker resource. */
   worker->func_progress = NULL;
   func = worker->func_end;
   result = worker->result;
   udata = worker->data;
   func(udata, result);
}

#define MKDIR(NAME) \
   tmp = eina_stringshare_printf("%s/"#NAME, pro->develop_path); \
   ecore_file_mkdir(tmp); \
   pro->res.NAME = eina_list_append(pro->res.NAME, eina_stringshare_add(tmp)); \
   eina_stringshare_del(tmp)

static Project *
_project_files_create(Project_Thread *worker)
{
   Project *pro;
   Eina_Stringshare *folder_path, *pro_path, *tmp;
   Eina_Bool error = false;

   assert(worker != NULL);

   _project_descriptor_init();

   folder_path = eina_stringshare_printf("%s/%s", worker->path, worker->name);
   if (ecore_file_mkdir(folder_path))
     {
        DBG("Create the folder '%s' for new project '%s'", folder_path, worker->name);
     }
   else
     {
        ERR("Could't create a project folder!");
        error = true;
     }
   eina_stringshare_del(folder_path);
   if (error) return NULL;

   THREAD_TESTCANCEL;
   pro = (Project *)mem_calloc(1, sizeof(Project));
   folder_path = eina_stringshare_printf("%s/%s", worker->path, worker->name);
   pro->version = PROJECT_FILE_VERSION;
   pro->name = eina_stringshare_add(worker->name);
   pro->dev = eina_stringshare_printf("%s/%s.dev", folder_path, worker->name);
   pro->saved_edj = eina_stringshare_printf("%s/%s.edj", folder_path, worker->name);
   pro->develop_path = eina_stringshare_printf("%s/develop", folder_path);

   pro_path = eina_stringshare_printf("%s/%s.pro", folder_path, worker->name);
   ecore_file_mkdir(pro->develop_path);
   MKDIR(images);
   MKDIR(sounds);
   MKDIR(fonts);
   eina_stringshare_del(folder_path);
   if (!_pm_project_descriptor_data_write(pro_path, pro))
     error = true;

   DBG("Create a specific project file '%s': %s", pro_path,
       error ? "failsed" : "success");
   THREAD_TESTCANCEL;
   _pm_project_descriptor_shutdown();
   eina_stringshare_del(pro_path);
   if (error)
     {
        ERR("Could't create a .pro file! ")
        eina_stringshare_del(pro->name);
        eina_stringshare_del(pro->dev);
        eina_stringshare_del(pro->saved_edj);
        eina_stringshare_del(pro->develop_path);
        free(pro);
        pro = NULL;
     }
   return pro;
}

#undef MKDIR

static void
_copy_meta_data_to_pro(Project_Thread *worker)
{
   Eet_File *ef;
   char *name, *authors, *version, *license, *comment;

   assert(worker != NULL);

   ef = eet_open(worker->edj, EET_FILE_MODE_READ_WRITE);

   name = strdup(worker->name);
   authors = eet_read(ef, PROJECT_KEY_AUTHORS, NULL);
   version = eet_read(ef, PROJECT_KEY_FILE_VERSION, NULL);
   license = eet_read(ef, PROJECT_KEY_LICENSE, NULL);
   comment = eet_read(ef, PROJECT_KEY_COMMENT, NULL);
   eet_close(ef);

   pm_project_meta_data_set(worker->project, name, authors,
                            version, license, comment);

   if (name) free(name);
   if (authors) free(authors);
   if (version) free(version);
   if (license) free(license);
   if (comment) free(comment);
}

static Eina_Bool
_project_edj_file_copy(Project_Thread *worker)
{
   Eina_Stringshare *src, *dst;
   Eina_Bool result;

   assert(worker != NULL);

   src = eina_stringshare_ref(worker->edj);
   dst = eina_stringshare_ref(worker->project->saved_edj);
   result = ecore_file_cp(src, dst);
   DBG("Copy the .edj file to project folder.");
   eina_stringshare_del(src);
   eina_stringshare_del(dst);
   return result;
}

static Eina_Bool
_project_linked_images_copy(Project_Thread *worker)
{
   Eina_List *list, *l;
   Evas_Object *edje_edit_obj;
   Evas *e;
   const char *name;
   const char *file_name;
   Eina_Strbuf *strbuf_to = eina_strbuf_new();
   Eina_Strbuf *strbuf_from = eina_strbuf_new();
   Edje_Edit_Image_Comp comp;
   Eina_Bool is_changed = false;

   assert(worker != NULL);

   ecore_thread_main_loop_begin();
   e = ecore_evas_get(worker->project->ecore_evas);
   list = edje_file_collection_list(worker->project->saved_edj);
   edje_edit_obj = edje_edit_object_add(e);

   if (!edje_object_file_set(edje_edit_obj, worker->project->saved_edj, eina_list_data_get(list)))
     {
        ERR("Can't set object file");
        abort();
     }

   ecore_thread_main_loop_end();
   edje_file_collection_list_free(list);

   list = edje_edit_images_list_get(edje_edit_obj);
   EINA_LIST_FOREACH(list, l, name)
     {
        /* for supporting themes that were compilled
           with edje_cc version less than 1.10 */
        if (!name) continue;

        comp = edje_edit_image_compression_type_get(edje_edit_obj, name);
        if (comp != EDJE_EDIT_IMAGE_COMP_USER)
          continue;
        is_changed = true;

        eina_strbuf_reset(strbuf_to);
        eina_strbuf_reset(strbuf_from);

        if (name[0] == '/')
          eina_strbuf_append(strbuf_from, name);
        else
          eina_strbuf_append_printf(strbuf_from, "%s/%s", worker->path, name);
        if (!ecore_file_exists(eina_strbuf_string_get(strbuf_from)))
          {
             edje_edit_image_rename(edje_edit_obj,
                                    name, eina_strbuf_string_get(strbuf_from));
             continue;
          }

        file_name = ecore_file_file_get(name);

        eina_strbuf_append_printf(strbuf_to, "%s/images/%s",
                                  worker->project->develop_path, file_name);

        eina_file_copy(eina_strbuf_string_get(strbuf_from),
                       eina_strbuf_string_get(strbuf_to),
                       EINA_FILE_COPY_PERMISSION | EINA_FILE_COPY_XATTR,
                       NULL, NULL);
        edje_edit_image_rename(edje_edit_obj,
                               name, eina_strbuf_string_get(strbuf_to));
     }
   if (is_changed)
     pm_save_to_dev(worker->project, NULL, true);
   edje_edit_string_list_free(list);
   eina_strbuf_free(strbuf_to);
   eina_strbuf_free(strbuf_from);
   ecore_thread_main_loop_begin();
   evas_object_del(edje_edit_obj);
   ecore_thread_main_loop_end();
   return true;
}

static void
_project_open_internal(Project *project)
{
   assert(project != NULL);

   _project_dev_file_create(project);
   project->mmap_file = eina_file_open(project->dev, false);

   project->changed = false;
   project->close_request = false;
   project->widgets = wm_widgets_list_new(project->dev);
   project->layouts = wm_layouts_list_new(project->dev);
   project->ecore_evas = ecore_evas_buffer_new(0, 0);
   project->global_object = edje_edit_object_add(ecore_evas_get(project->ecore_evas));
   edje_object_mmap_set(project->global_object, project->mmap_file, EFLETE_INTERNAL_GROUP_NAME);
}

static void
_project_special_group_add(Project *project)
{
   Evas *e;
   Evas_Object *edje_edit_obj;
   Eina_List *list;

   assert(project != NULL);

   ecore_thread_main_loop_begin();

   Ecore_Evas *ee = ecore_evas_buffer_new(0, 0);
   e = ecore_evas_get(ee);
   list = edje_file_collection_list(project->saved_edj);
   edje_edit_obj = edje_edit_object_add(e);

   edje_object_file_set(edje_edit_obj, project->saved_edj, eina_list_data_get(list));

   if (!edje_edit_group_exist(edje_edit_obj, EFLETE_INTERNAL_GROUP_NAME))
     {
        edje_edit_group_add(edje_edit_obj, EFLETE_INTERNAL_GROUP_NAME);
        edje_edit_without_source_save(edje_edit_obj, false);
     }
   edje_edit_string_list_free(list);
   evas_object_del(edje_edit_obj);
   ecore_evas_free(project->ecore_evas);

   ecore_thread_main_loop_end();
}

static void *
_project_import_edj(void *data,
                    Eina_Thread *thread __UNUSED__)
{
   Project_Thread *worker;

   worker = (Project_Thread *)data;

   assert(worker != NULL);

   THREAD_TESTCANCEL;
   PROGRESS_SEND(_("Start import '%s' file as new project"), worker->edj);
   PROGRESS_SEND(_("Creating a specifiec file and folders"));
   worker->project = _project_files_create(worker);
   TODO("Add correct error handling here (if project == NULL). Probably we should add negative TC where directory already exist");
   THREAD_TESTCANCEL;
   worker->project->pro_path = eina_stringshare_printf("%s/%s/%s.pro", worker->path, worker->name, worker->name);
   THREAD_TESTCANCEL;
   PROGRESS_SEND(_("Import processing"));
   _project_edj_file_copy(worker);
   _copy_meta_data_to_pro(worker);
   _project_special_group_add(worker->project);
   _project_open_internal(worker->project);
   THREAD_TESTCANCEL;
   _project_resource_export(worker->project, NULL, worker);
   _project_linked_images_copy(worker);
   edje_file_cache_flush();
   PROGRESS_SEND(_("Import finished. Project '%s' created"), worker->project->name);

   END_SEND(PM_PROJECT_SUCCESS);

   return NULL;
}

Project_Thread *
pm_project_import_edj(const char *name,
                      const char *path,
                      const char *edj,
                      PM_Project_Progress_Cb func_progress,
                      PM_Project_End_Cb func_end ,
                      const void *data)
{
   Project_Thread *worker;

   assert(name != NULL);
   assert(path != NULL);
   assert(edj != NULL);

   WORKER_CREATE(func_progress, func_end, data, NULL,
                 name, path, edj, NULL, NULL);

   if (!eina_thread_create(&worker->thread, EINA_THREAD_URGENT, -1,
                           (void *)_project_import_edj, worker))
     {
        ERR("System error: can't create thread");
        abort();
     }

   return worker;
}

static Eina_Bool
_exe_data(void *data,
          int type __UNUSED__,
          void *event)
{
   int i;
   Ecore_Exe_Event_Data *ev = event;
   Project_Thread *worker = (Project_Thread *)data;

   assert(worker != NULL);

   if (ev->lines)
     {
        for (i = 0; ev->lines[i].line; i++)
          {
             DBG("%s", ev->lines[i].line);
             PROGRESS_SEND("%s", ev->lines[i].line);
          }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static void *
_project_import_edc(void *data,
                    Eina_Thread *thread __UNUSED__)
{
   Project_Thread *worker;
   Eina_Tmpstr *tmp_dirname;
   Ecore_Event_Handler *cb_msg_stdout = NULL,
                       *cb_msg_stderr = NULL;
   Ecore_Exe_Flags flags  = ECORE_EXE_PIPE_READ |
                            ECORE_EXE_PIPE_READ_LINE_BUFFERED |
                            ECORE_EXE_PIPE_ERROR |
                            ECORE_EXE_PIPE_ERROR_LINE_BUFFERED;
   Eina_Stringshare *cmd;
   Ecore_Exe *exe_cmd;
   pid_t exe_pid;
   int edje_cc_res = 0, waitpid_res = 0;

   worker = (Project_Thread *)data;

   assert(worker != NULL);

   PROGRESS_SEND(_("Start import '%s' file as new project"), worker->edj);
   PROGRESS_SEND(_("Creating a specifiec file and folders"));
   if (worker->func_progress)
     {
        cb_msg_stdout = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, _exe_data, worker);
        cb_msg_stderr = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, _exe_data, worker);
     }
   eina_file_mkdtemp("eflete_build_XXXXXX", &tmp_dirname);
   worker->edj = eina_stringshare_printf("%s/out.edj", tmp_dirname);
   cmd = eina_stringshare_printf("edje_cc -v %s %s %s",
                                 worker->build_options,
                                 worker->edc,
                                 worker->edj);
   THREAD_TESTCANCEL;
   exe_cmd = ecore_exe_pipe_run(cmd, flags, NULL);
   exe_pid = ecore_exe_pid_get(exe_cmd);
   THREAD_TESTCANCEL;
   waitpid_res = waitpid(exe_pid, &edje_cc_res, 0);
   if (worker->func_progress)
     {
        ecore_event_handler_del(cb_msg_stdout);
        ecore_event_handler_del(cb_msg_stderr);
     }

   if ((waitpid_res == -1) ||
       (WIFEXITED(edje_cc_res) && (WEXITSTATUS(edje_cc_res) != 0 )))
     {
        END_SEND(PM_PROJECT_ERROR);
        return NULL;
     }

   THREAD_TESTCANCEL;
   worker->project = _project_files_create(worker);
   TODO("Add correct error handling here (if project == NULL). Probably we should add negative TC where directory already exist");
   worker->project->pro_path = eina_stringshare_printf("%s/%s/%s.pro", worker->path, worker->name, worker->name);
   THREAD_TESTCANCEL;
   PROGRESS_SEND("%s", _("Import processing"));
   _project_edj_file_copy(worker);
   ecore_file_recursive_rm(tmp_dirname);
   eina_tmpstr_del(tmp_dirname);
   _copy_meta_data_to_pro(worker);
   _project_special_group_add(worker->project);
   _project_open_internal(worker->project);
   THREAD_TESTCANCEL;
   _project_resource_export(worker->project, NULL, worker);
   _project_linked_images_copy(worker);
   edje_file_cache_flush();
   PROGRESS_SEND(_("Import finished. Project '%s' created"), worker->project->name);

   END_SEND(PM_PROJECT_SUCCESS)

   return NULL;
}

Project_Thread *
pm_project_import_edc(const char *name,
                      const char *path,
                      const char *edc,
                      const char *import_options,
                      PM_Project_Progress_Cb func_progress,
                      PM_Project_End_Cb func_end,
                      const void *data)
{
   Project_Thread *worker;

   assert(name != NULL);
   assert(path != NULL);
   assert(edc != NULL);

   WORKER_CREATE(func_progress, func_end, data, NULL,
                 name, path, NULL, edc, import_options);

   if (!eina_thread_create(&worker->thread, EINA_THREAD_URGENT, -1,
                           (void *)_project_import_edc, worker))
     {
        ERR("System error: can't create thread");
        abort();
     }

   return worker;
}

Eina_Bool
pm_project_thread_cancel(Project_Thread *worker)
{
   int ret;

   assert(worker != NULL);

   ret = pthread_cancel((pthread_t)worker->thread);
   if (ret)
     return false;

   END_SEND(PM_PROJECT_CANCEL);
   DBG("Project Thread %p stoped by user!", worker);
   return true;
}

Eina_Bool
pm_project_thread_free(Project_Thread *worker)
{
   assert(worker != NULL);

   if (worker->result == 0)
     {
        WORKER_FREE();
        return true;
     }
   return false;
}

Project *
pm_project_open(const char *path)
{
   Eet_File *ef;
   Project *project;
   char *tmp;
   int tmp_len;

   assert(path != NULL);

   edje_file_cache_flush();

   _project_descriptor_init();
   ef = eet_open(path, EET_FILE_MODE_READ_WRITE);
   if (!ef) return NULL;

   project = eet_data_read(ef, eed_project, PROJECT_FILE_KEY);
   _pm_project_descriptor_shutdown();
   eet_close(ef);
   if (!project) return NULL;

   project->pro_path = eina_stringshare_add(path);

   /* updating .dev file path */
   tmp = strdup(path);
   tmp_len = strlen(tmp);
   tmp[tmp_len - 3] = 'd';
   tmp[tmp_len - 2] = 'e';
   tmp[tmp_len - 1] = 'v';
   eina_stringshare_replace(&project->dev, tmp);
   free(tmp);
   /* updating .edj file path */
   tmp = strdup(path);
   tmp[tmp_len - 3] = 'e';
   tmp[tmp_len - 2] = 'd';
   tmp[tmp_len - 1] = 'j';
   eina_stringshare_replace(&project->saved_edj, tmp);
   free(tmp);

   /* checking for older project versions and upgrading them version-by-version */
   if (project->version < 2) /* upgrade to version 2 */
     {
        WARN(_("Old project version. Project files were updated."));
        ecore_file_mv(project->dev, project->saved_edj);
        project->version = 2;
     }
   if (project->version < 3) /* upgrade to version 3 */
     {
        WARN(_("Old project version. Project files were updated."));
        _project_special_group_add(project);
        project->version = 3;
     }
   TODO("Add crash recovery prompt here")

   _project_open_internal(project);

   pm_project_meta_data_get(project, &project->name, NULL, NULL, NULL, NULL);
   if (!project->name) project->name = eina_stringshare_add(_("No title"));

   return project;
}

void
pm_save_to_dev(Project *pr, Style *st, Eina_Bool save)
{
   assert(pr != NULL);

   if (st)
     {
        if (save) edje_edit_without_source_save(st->obj, true);
     }
   else
     {
        if (save) edje_edit_without_source_save(pr->global_object, false);
     }
   /* reloading mmaped dev file to update cached groups */
   eina_file_close(pr->mmap_file);
   pr->mmap_file = eina_file_open(pr->dev, false);
   if (st) edje_object_mmap_set(st->obj, pr->mmap_file, st->full_group_name);
   edje_object_mmap_set(pr->global_object, pr->mmap_file, EFLETE_INTERNAL_GROUP_NAME);
}

static void *
_project_save(void *data,
              Eina_Thread *thread __UNUSED__)
{
   Project_Thread *worker;
   Eina_List *list;
   Eina_Stringshare *dest;
   Evas_Object *edje_edit_obj;

   worker = (Project_Thread *)data;

   assert(worker != NULL);

   edje_edit_obj = worker->project->global_object;

   ecore_thread_main_loop_begin();
   PROGRESS_SEND(_("Save project '%s'"), worker->project->name);
   pm_save_to_dev(worker->project, NULL, true);
   eina_file_close(worker->project->mmap_file);
   Uns_List *it;
   Eina_List *add_list = NULL, *l;
   EINA_LIST_FOREACH(worker->project->nsimage_list, l, it)
     {
        if (it->act_type == ACTION_TYPE_ADD)
          add_list = eina_list_append(add_list, eina_stringshare_add(ecore_file_file_get(it->data)));
     }

   /* saving */
   dest = eina_stringshare_printf("%s/images", worker->project->develop_path);
   _image_resources_export(worker->project, add_list, dest, NULL, worker->project->dev, edje_edit_obj, worker);
   edje_edit_string_list_free(add_list);
   EINA_LIST_FREE(worker->project->nsimage_list, it)
      free(it);
   eina_stringshare_del(dest);

   list = edje_edit_sound_samples_list_get(edje_edit_obj);
   dest = eina_stringshare_printf("%s/sounds", worker->project->develop_path);
   _sound_resources_export(list, dest, NULL, edje_edit_obj, worker);
   edje_edit_string_list_free(list);
   eina_stringshare_del(dest);

   list = edje_edit_fonts_list_get(edje_edit_obj);
   dest = eina_stringshare_printf("%s/fonts", worker->project->develop_path);
   _font_resources_export(list, dest, NULL, worker->project->dev, edje_edit_obj, worker);
   edje_edit_string_list_free(list);
   eina_stringshare_del(dest);

   ecore_file_cp(worker->project->dev, worker->project->saved_edj);

   /* reloading dev*/
   worker->project->mmap_file = eina_file_open(worker->project->dev, false);
   if (worker->project->current_style)
     {
        edje_object_mmap_set(worker->project->current_style->obj,
                             worker->project->mmap_file,
                             worker->project->current_style->full_group_name);
     }
   PROGRESS_SEND("Save done");
   ecore_thread_main_loop_end();

   END_SEND(PM_PROJECT_SUCCESS);
   return NULL;
}

Project_Thread *
pm_project_save(Project *project,
                PM_Project_Progress_Cb func_progress,
                PM_Project_End_Cb func_end,
                const void *data)
{
   Project_Thread *worker;

   assert(project != NULL);

   WORKER_CREATE(func_progress, func_end, data, project,
                 NULL, NULL, NULL, NULL, NULL);

   if (!eina_thread_create(&worker->thread, EINA_THREAD_URGENT, -1,
                           (void *)_project_save, worker))
     {
        ERR("System error: can't create thread");
        abort();
     }

   return worker;
}

Eina_Bool
pm_project_style_save_as(Project *project __UNUSED__, const char *file __UNUSED__)
{
   return false;
}

Eina_Bool
pm_project_build(Project *project __UNUSED__, Build build_profile __UNUSED__)
{
   return false;
}

Eina_Bool
pm_project_close(Project *project)
{
   Eina_Stringshare *backup, *data;

   assert(project != NULL);

   backup = eina_stringshare_printf("%s.backup", project->dev);
   ecore_file_remove(backup);
   eina_stringshare_del(backup);

   wm_widgets_list_free(project->widgets);
   wm_layouts_list_free(project->layouts);

   evas_object_del(project->global_object);
   ecore_evas_free(project->ecore_evas);

   eina_file_close(project->mmap_file);
   ecore_file_unlink(project->dev);

   eina_stringshare_del(project->name);
   eina_stringshare_del(project->dev);
   eina_stringshare_del(project->develop_path);
   eina_stringshare_del(project->pro_path);

   EINA_LIST_FREE(project->res.images, data)
      eina_stringshare_del(data);
   EINA_LIST_FREE(project->res.sounds, data)
      eina_stringshare_del(data);
   EINA_LIST_FREE(project->res.fonts, data)
      eina_stringshare_del(data);

#ifdef HAVE_ENVENTOR
   if (enventor_object_project_unload(project))
     free(project->enventor);
#endif /* HAVE_ENVENTOR */

   free(project);

   return true;
}

void
pm_project_meta_data_get(Project *project,
                         Eina_Stringshare **name,
                         Eina_Stringshare **authors,
                         Eina_Stringshare **version,
                         Eina_Stringshare **license,
                         Eina_Stringshare **comment)
{
   char *tmp;

   assert(project != NULL);

   Eet_File *ef = eet_open(project->pro_path, EET_FILE_MODE_READ);
   if (!ef)
     {
        ERR("Can't open proect file \"%s\" for read", project->pro_path);
        return;
     }
#define DATA_READ(DATA, KEY) \
   if (DATA) \
     { \
        tmp = eet_read(ef, KEY, NULL); \
        *DATA = eina_stringshare_add(tmp); \
        free(tmp); \
     }

   DATA_READ(name,    PROJECT_KEY_NAME);
   DATA_READ(authors, PROJECT_KEY_AUTHORS);
   DATA_READ(version, PROJECT_KEY_FILE_VERSION);
   DATA_READ(license, PROJECT_KEY_LICENSE);
   DATA_READ(comment, PROJECT_KEY_COMMENT);

   eet_close(ef);

#undef DATA_READ
}

Eina_Bool
pm_project_meta_data_set(Project *project,
                         const char *name,
                         const char *authors,
                         const char *version,
                         const char *license,
                         const char *comment)
{
   int bytes, size;
   Eina_Bool res;

   assert(project != NULL);

   res = true;

   Eet_File *ef = eet_open(project->pro_path, EET_FILE_MODE_READ_WRITE);
   if (!ef)
     {
        ERR("Can't open proect file \"%s\" for write", project->pro_path);
        return false;
     }
#define DATA_WRITE(DATA, KEY) \
   if (DATA) \
     { \
        size = (strlen(DATA) + 1) * sizeof(char); \
        bytes = eet_write(ef, KEY, DATA, size, compess_level); \
        if (bytes <= 0 ) res = false; \
     }

   DATA_WRITE(name,    PROJECT_KEY_NAME);
   DATA_WRITE(authors, PROJECT_KEY_AUTHORS);
   DATA_WRITE(version, PROJECT_KEY_FILE_VERSION);
   DATA_WRITE(license, PROJECT_KEY_LICENSE);
   DATA_WRITE(comment, PROJECT_KEY_COMMENT);

   eet_close(ef);

#undef DATA_WRITE
   return res;
}

static Eina_Bool
_image_resources_export(Project *project,
                        Eina_List *images, Eina_Stringshare *destination,
                        Eina_Stringshare *source, Eina_Stringshare *dev,
                        Evas_Object *edje_edit, Project_Thread *worker)
{
  Eina_Stringshare *image_name, *source_file, *dest_file;
  Eina_List *l;
  Evas *e;
  Evas_Object *im;
  int id;
  char *file_dir;
  int im_total, im_proc;

  assert(project != NULL);
  assert(destination != NULL);
  assert(dev != NULL);
  assert(edje_edit != NULL);

  if (!ecore_file_mkpath(destination))
    {
       ERR("Failed create path %s for export images", destination);
       return false;
    }
  e = ecore_evas_get(project->ecore_evas);
  im_total = eina_list_count(images);
  im_proc = 0;
  if (worker) PROGRESS_SEND(_("Start image processing, total %d:"), im_total);
  EINA_LIST_FOREACH(images, l, image_name)
    {
       /* for supporting old themes, which were compilled
        * with edje_cc version less than 1.10 */
       if (!image_name) continue;
       im_proc++;
       if (worker) PROGRESS_SEND(_("image processing (%d/%d): %s"),
                                 im_proc, im_total, image_name);
       source_file = eina_stringshare_printf("%s/%s", source,
                                             ecore_file_file_get(image_name));
       dest_file = eina_stringshare_printf("%s/%s", destination, image_name);
       if (!ecore_file_exists(dest_file))
         {
            file_dir = ecore_file_dir_get(dest_file);
            ecore_file_mkpath(file_dir);
            free(file_dir);
            if ((source) && (ecore_file_exists(source_file)))
              {
                 ecore_file_cp(source_file, dest_file);
              }
            else
              {
                 ecore_thread_main_loop_begin();
                 im = evas_object_image_add(e);
                 id = edje_edit_image_id_get(edje_edit, image_name);
                 if (id < 0)
                   {
                      WARN("Image %s coudn't be exported", image_name);
                      ecore_thread_main_loop_end();
                      continue;
                   }
                 source_file = eina_stringshare_printf("edje/images/%i", id);
                 evas_object_image_file_set(im, dev, source_file);
                 ecore_thread_main_loop_end();
                 evas_object_image_save(im, dest_file, NULL, NULL);
                 ecore_thread_main_loop_begin();
                 evas_object_del(im);
                 ecore_thread_main_loop_end();
              }
         }
       eina_stringshare_del(source_file);
       eina_stringshare_del(dest_file);
    }

  return true;
}

static Eina_Bool
_sound_resources_export(Eina_List *sounds, Eina_Stringshare *destination,
                        Eina_Stringshare *source, Evas_Object *edje_edit,
                        Project_Thread *worker)
{
  Eina_Stringshare *sound_name, *source_file, *dest_file, *sound_file;
  Eina_List *l;
  Eina_Binbuf *sound_bin;
  FILE *f;
  char *file_dir;
  int snd_total, snd_proc;

  assert(destination != NULL);
  assert(edje_edit != NULL);

  if (!ecore_file_mkpath(destination))
    {
       ERR("Failed create path %s for export sounds", destination);
       return false;
    }
  snd_total = eina_list_count(sounds);
  snd_proc = 0;
  if (worker) PROGRESS_SEND(_("Start sound processing: total %d:"), snd_total);
  EINA_LIST_FOREACH(sounds, l, sound_name)
    {
       sound_file = edje_edit_sound_samplesource_get(edje_edit, sound_name);
       source_file = eina_stringshare_printf("%s/%s", source, sound_file);
       dest_file = eina_stringshare_printf("%s/%s", destination, sound_file);
       snd_proc++;
       if (worker) PROGRESS_SEND(_("sound processing (%d/%d): %s"),
                                 snd_proc, snd_total, sound_file);

       if (!ecore_file_exists(dest_file))
         {
            file_dir = ecore_file_dir_get(dest_file);
            ecore_file_mkpath(file_dir);
            free(file_dir);
            if ((source) && (ecore_file_exists(source_file)))
              {
                 ecore_file_cp(source_file, dest_file);
              }
            else
              {
                 sound_bin = edje_edit_sound_samplebuffer_get(edje_edit, sound_name);
                 if (!(f = fopen(dest_file, "wb")))
                   {
                      ERR("Could not open file: %s", dest_file);
                      continue;
                   }
                 if (fwrite(eina_binbuf_string_get(sound_bin),
                            eina_binbuf_length_get(sound_bin), 1, f) != 1)
                   ERR("Could not write font: %s", strerror(errno));
                 if (f) fclose(f);
                 eina_binbuf_free(sound_bin);
              }
         }
       edje_edit_string_free(sound_file);
       eina_stringshare_del(source_file);
       eina_stringshare_del(dest_file);
    }

  return true;
}

static Eina_Bool
_font_resources_export(Eina_List *fonts, Eina_Stringshare *destination,
                       Eina_Stringshare *source, Eina_Stringshare *dev,
                       Evas_Object *edje_edit, Project_Thread *worker)
{
  Eet_File *ef;
  Eina_List *l;
  Eina_Stringshare *font_name, *source_file, *dest_file, *font_file;
  void *font;
  FILE *f;
  int size, fnt_total, fnt_proc;

  assert(destination != NULL);
  assert(dev != NULL);
  assert(edje_edit != NULL);

  if (!ecore_file_mkpath(destination))
    {
       ERR("Failed create path %s for export fonts", destination);
       return false;
    }
  ef = eet_open(dev, EET_FILE_MODE_READ);
  fnt_total = eina_list_count(fonts);
  fnt_proc = 0;
  if (worker) PROGRESS_SEND(_("Start font processing, total %d:"), fnt_total);
  EINA_LIST_FOREACH(fonts, l, font_name)
    {
       font_file = edje_edit_font_path_get(edje_edit, font_name);
       source_file = eina_stringshare_printf("%s/%s", source, font_file);
       dest_file = eina_stringshare_printf("%s/%s", destination, font_file);
       fnt_proc++;
       if (worker) PROGRESS_SEND(_("font processing (%d/%d): %s"),
                                 fnt_proc, fnt_total, font_file);
       if (!ecore_file_exists(dest_file))
         {
            edje_edit_string_free(font_file);
            if ((source) && (ecore_file_exists(source_file)))
              {
                 ecore_file_cp(source_file, dest_file);
              }
            else
              {
                 font_file = eina_stringshare_printf("edje/fonts/%s", font_name);
                 font = eet_read(ef, font_file, &size);
                 if (!font) continue;
                 if (!(f = fopen(dest_file, "wb")))
                   {
                      ERR("Could not open file: %s", dest_file);
                      continue;
                   }
                 if (fwrite(font, size, 1, f) != 1)
                   ERR("Could not write font: %s", strerror(errno));
                 if (f) fclose(f);
                 free(font);
                 eina_stringshare_del(font_file);
              }
         }
       eina_stringshare_del(source_file);
       eina_stringshare_del(dest_file);
    }
  eet_close(ef);
  return true;
}

Eina_Bool
pm_style_resource_export(Project *pro , Style *style, Eina_Stringshare *path)
{
   Eina_List *l, *l_next, *parts, *state_list, *l_states, *tween_list, *l_tween;
   Eina_List *programs;

   Eina_List *images = NULL, *sounds = NULL, *fonts = NULL;

   Eina_Stringshare *dest, *source;
   const char *data_name, *state_name, *data;
   const char *state = NULL; double value = 0; char **state_split;
   Edje_Part_Type part_type = EDJE_PART_TYPE_NONE;
   Edje_Action_Type action_type = EDJE_ACTION_TYPE_NONE;

   assert(pro != NULL);
   assert(style != NULL);
   assert(style->obj != NULL);
   assert(path != NULL);

   /* Collect lists with info needed resources */
   parts = edje_edit_parts_list_get(style->obj);
   EINA_LIST_FOREACH_SAFE(parts, l, l_next, data_name)
     {
        part_type = edje_edit_part_type_get(style->obj, data_name);
        switch (part_type)
          {
           case EDJE_PART_TYPE_IMAGE:
              state_list = edje_edit_part_states_list_get(style->obj, data_name);
              EINA_LIST_FOREACH(state_list, l_states, state_name)
                {
                  state_split = eina_str_split(state_name, " ", 2);
                  state = eina_stringshare_add(state_split[0]);
                  value = atof(state_split[1]);
                  free(state_split[0]);
                  free(state_split);

                  data = edje_edit_state_image_get(style->obj, data_name,
                                                   state, value);
                  if ((data) && (!eina_list_data_find(images, data)))
                    images = eina_list_append(images, eina_stringshare_add(data));
                  edje_edit_string_free(data);

                  tween_list = edje_edit_state_tweens_list_get(style->obj,
                                                               data_name,
                                                               state, value);
                  EINA_LIST_FOREACH(tween_list, l_tween, data)
                    {
                       if (!eina_list_data_find(images, data))
                         images = eina_list_append(images, eina_stringshare_add(data));
                    }
                  edje_edit_string_list_free(tween_list);
                }
              edje_edit_string_list_free(state_list);
           break;
           case EDJE_PART_TYPE_TEXT:
              state_list = edje_edit_part_states_list_get(style->obj, data_name);
              EINA_LIST_FOREACH(state_list, l_states, state_name)
                {
                  state_split = eina_str_split(state_name, " ", 2);
                  state = eina_stringshare_add(state_split[0]);
                  value = atof(state_split[1]);
                  free(state_split[0]);
                  free(state_split);

                  data = edje_edit_state_font_get(style->obj, data_name,
                                                  state, value);
                  if ((data) && (!eina_list_data_find(fonts, data)))
                    fonts = eina_list_append(fonts, eina_stringshare_add(data));
                  edje_edit_string_free(data);
                }
              edje_edit_string_list_free(state_list);
           break;
           case EDJE_PART_TYPE_TEXTBLOCK:
           break;
           default:
           break;
          }
     }
   edje_edit_string_list_free(parts);

   programs = edje_edit_programs_list_get(style->obj);
   EINA_LIST_FOREACH_SAFE(programs, l, l_next, data_name)
     {
        action_type = edje_edit_program_action_get(style->obj, data_name);
        if (action_type == EDJE_ACTION_TYPE_SOUND_SAMPLE)
          {
             data = edje_edit_program_sample_name_get(style->obj, data_name);
             if ((data) && (!eina_list_data_find(sounds, data)))
               sounds = eina_list_append(sounds, data);
          }
     }
   edje_edit_string_list_free(programs);

   dest = eina_stringshare_printf("%s/images", path);
   EINA_LIST_FOREACH(pro->res.images, l, source)
     {
       if (!_image_resources_export(pro, images, dest, source, pro->dev, style->obj, NULL))
         WARN("Failed export images");
     }
   eina_stringshare_del(dest);
   EINA_LIST_FREE(images, data)
     eina_stringshare_del(data);

   dest = eina_stringshare_printf("%s/sounds", path);
   EINA_LIST_FOREACH(pro->res.sounds, l, source)
     {
       if (!_sound_resources_export(sounds, dest, source, style->obj, NULL))
         ERR("Failed export sounds");
     }
   eina_stringshare_del(dest);
   EINA_LIST_FREE(sounds, data)
     eina_stringshare_del(data);

   dest = eina_stringshare_printf("%s/fonts", path);
   EINA_LIST_FOREACH(pro->res.fonts, l, source)
     {
        if (!_font_resources_export(fonts, dest, source, pro->dev, style->obj, NULL))
          WARN("Failed export fonts");
     }
   eina_stringshare_del(dest);
   EINA_LIST_FREE(fonts, data)
     eina_stringshare_del(data);

   return true;
}

Eina_Bool
pm_project_resource_export(Project *pro, const char* dir_path)
{
   return _project_resource_export(pro, dir_path, NULL);
}

Eina_Bool
_project_resource_export(Project *pro, const char* dir_path, Project_Thread *worker)
{
   Eina_List *list;
   Evas_Object *edje_edit_obj;
   Evas *e;
   Eina_Stringshare *dest;
   Eina_Stringshare *path = NULL;

   assert(pro != NULL);

   ecore_thread_main_loop_begin();
   Ecore_Evas *ee = ecore_evas_buffer_new(0, 0);
   e = ecore_evas_get(ee);

   list = edje_file_collection_list(pro->dev);
   edje_edit_obj = edje_edit_object_add(e);
   if (!edje_object_mmap_set(edje_edit_obj, pro->mmap_file, eina_list_data_get(list)))
     {
        evas_object_del(edje_edit_obj);
        ecore_evas_free(ee);
        ecore_thread_main_loop_end();
        return false;
     }
   ecore_thread_main_loop_end();
   edje_edit_string_list_free(list);
   path = (dir_path) ? eina_stringshare_add(dir_path)
          : eina_stringshare_add(pro->develop_path);

   /* export images */
   list = edje_edit_images_list_get(edje_edit_obj);
   dest = eina_stringshare_printf("%s/images", path);
   _image_resources_export(pro, list, dest, NULL, pro->dev, edje_edit_obj, worker);
   edje_edit_string_list_free(list);
   eina_stringshare_del(dest);

   /* export fonts */
   list = edje_edit_fonts_list_get(edje_edit_obj);
   dest = eina_stringshare_printf("%s/fonts", path);
   _font_resources_export(list, dest, NULL, pro->dev, edje_edit_obj, worker);
   edje_edit_string_list_free(list);
   eina_stringshare_del(dest);

   /* export sounds */
   list = edje_edit_sound_samples_list_get(edje_edit_obj);
   dest = eina_stringshare_printf("%s/sounds", path);
   _sound_resources_export(list, dest, NULL, edje_edit_obj, worker);
   edje_edit_string_list_free(list);
   eina_stringshare_del(dest);

   eina_stringshare_del(path);
   ecore_thread_main_loop_begin();
   evas_object_del(edje_edit_obj);
   ecore_evas_free(ee);
   ecore_thread_main_loop_end();

   return true;
}

Eina_Bool
pm_project_style_source_code_export(Project *pro, Style *style, const char *file)
{
   Eina_Stringshare *code = NULL;
   Eina_Stringshare *path = NULL;
   FILE *f;

   assert(pro != NULL);
   assert(style != NULL);
   assert(style->obj != NULL);

   if (file) path = eina_stringshare_add(file);
   else path = eina_stringshare_printf("%s/tmp.edc", pro->develop_path);
   f = fopen(path, "w");
   if (!f)
     {
        ERR("Could't open file '%s'", path);
        eina_stringshare_del(path);
        return false;
     }
   code = edje_edit_source_generate(style->obj);
   fputs(code, f);
   fclose(f);

   eina_stringshare_del(code);
   eina_stringshare_del(path);
   return true;
}

Eina_Bool
pm_project_source_code_export(Project *pro, const char *dir_path)
{
   Eina_Stringshare *code = NULL;
   Eina_Stringshare *group = NULL;
   Eina_Stringshare *top_level_str[TOP_LEVEL_NUMBER];
   Eina_Stringshare *last = NULL;

   Eina_Stringshare *path = NULL;
   Eina_Stringshare *path_edc_dir = NULL;
   Eina_Stringshare *path_edc = NULL;
   Eina_Stringshare *group_edc = NULL;
   Eina_Stringshare *include = NULL;
   Eina_Stringshare *gr_name = NULL;

   assert(pro != NULL);
   assert(dir_path != NULL);

   Eina_Bool next = true, open_flag = true, group_start = false;
   unsigned int i = 0, tokens_count = 0, tok_name = 0, j = 0;
   FILE *f, *fedc = NULL, *f_top[TOP_LEVEL_NUMBER];
   char **tmp, **tmp_name = NULL;
   int value = -1, k = 0;
   const char *pos = NULL;
   char str[100];
   Evas_Object *edje_edit_obj = pro->global_object;

   path = eina_stringshare_printf("%s/%s.edc", dir_path, pro->name);
   f = fopen(path, "w");
   if (!f)
     {
        ERR("Could't open file '%s'", path);
        goto exit;
     }

   path_edc_dir = eina_stringshare_printf("%s/edc", dir_path);
   if (!ecore_file_exists(path_edc_dir))
     ecore_file_mkdir(path_edc_dir);

   for (i = 1; i < TOP_LEVEL_NUMBER - 1; i++)
     {
        path = eina_stringshare_printf("%s/edc/%s.edc", dir_path, top_levels[i]);
        f_top[i] = fopen(path, "w");
        if (!f_top[i]) goto exit;
        top_level_str[i] = eina_stringshare_printf("/* Automatically generated by Eflete */\n"
                                                   "%s {\n", top_levels[i]);

        /* Sounds level should be at collections */
        if (i == 7)
          {
             fputs("collections {\n", f);
             fputs(S3, f);
          }
        include = eina_stringshare_printf("#include \"edc/%s.edc\"\n", top_levels[i]);
        fputs(include, f);
     }

   code = eina_stringshare_add(edje_edit_full_source_generate(edje_edit_obj));
   tmp = eina_str_split_full(code, "{", 0, &tokens_count);
   if (tmp[0])
   for (i = 0; tokens_count > 0; i++, tokens_count--)
     {
        /* Generate group_name.edc */
        if (group_start && open_flag)
          {
             if (!sscanf(tmp[i], " name: \"%s", str)) continue;
             gr_name = eina_stringshare_nprintf(strlen(str) - 1, "%s", str);
             tmp_name = eina_str_split_full(gr_name, "/", 0, &tok_name);
             if (!tmp_name[0]) continue;
             group_edc = eina_stringshare_add(tmp_name[0]);
             for (j = 1; tok_name - 1 > 0; j++, tok_name--)
               group_edc = eina_stringshare_printf("%s_%s", group_edc, tmp_name[j]);
             group_edc = eina_stringshare_printf("%s.edc", group_edc);
             free(tmp_name[0]);
             free(tmp_name);
             path_edc = eina_stringshare_printf("%s/%s", path_edc_dir, group_edc);
             fedc = fopen(path_edc, "w");
             if (!fedc) continue;
             open_flag = false;
             group_start = false;
             eina_stringshare_del(gr_name);
          }
        if ((value == 0) && !(strstr(tmp[i], "sounds")))
          {
             if (!group)
               group = eina_stringshare_add(tmp[i]);
             else if ((strstr(tmp[i], GROUP)) || tokens_count == 1)
               {
                  pos = strrchr(tmp[i], '}');
                  last = eina_stringshare_nprintf(pos - tmp[i] + 2, "%s", tmp[i]);
                  group = eina_stringshare_printf("%s{%s", group, last);
                  eina_stringshare_del(last);
               }
             else
               group = eina_stringshare_printf("%s{%s", group, tmp[i]);
          }
        /* Generate top_level_name.edc and sounds */
        else if ((value > 0) && (value < TOP_LEVEL_NUMBER))
          {
             for (k = 0; top_levels[k] != NULL; k++)
               {
                  if (k == value) continue;
                  if (strstr(tmp[i], top_levels[k]))
                    {
                       pos = strrchr(tmp[i], '}');
                       last = eina_stringshare_nprintf(pos - tmp[i] + 1, "%s", tmp[i]);
                       top_level_str[value] = eina_stringshare_printf("%s{%s",
                                                                      top_level_str[value], last);
                       eina_stringshare_del(last);
                       next = false;
                    }
               }
             if (next)
               top_level_str[value] = eina_stringshare_printf("%s{%s",
                                                              top_level_str[value], tmp[i]);
          }
        for (k = 0; top_levels[k] != NULL; k++)
          {
             if ((value != 0) && strstr(tmp[i], top_levels[k]))
               {
                  if ((value == 7) && (k == 8)) value = 0;
                  else value = k;
                  break;
               }
             else if (strstr(tmp[i], top_levels[7])) value = 7;
          }
        if ((value == 0) && strstr(tmp[i], GROUP)) group_start = true;
        if ((!open_flag && (strstr(tmp[i], GROUP))) || (!open_flag && (tokens_count == 1)))
          {
             if (group)
               {
                  fputs(GROUP"{", fedc);
                  fputs(group, fedc);
               }
             include = eina_stringshare_printf(S3"#include \"edc/%s\"\n", group_edc);
             fputs(include, f);
             eina_stringshare_del(path_edc);
             eina_stringshare_del(group_edc);
             eina_stringshare_del(group);
             eina_stringshare_del(include);
             group = NULL;
             open_flag = true;
          }
        next = true;
     }
   free(tmp[0]);
   free(tmp);
   for (i = 1; i < TOP_LEVEL_NUMBER - 1; i++)
     {
        if ((f_top[i]) && (top_level_str[i]))
          {
             fputs(top_level_str[i], f_top[i]);
             fputs("}", f_top[i]);
          }
        fclose(f_top[i]);
        eina_stringshare_del(top_level_str[i]);
     }

exit:
   eina_stringshare_del(path);
   eina_stringshare_del(path_edc_dir);
   if (f) fclose(f);
   if (fedc) fclose(fedc);
   return EINA_TRUE;
}
#undef S3
#undef GROUP
#undef TOP_LEVEL_NUMBER

static void *
_develop_export(void *data,
                Eina_Thread *thread __UNUSED__)
{
   Project_Thread *worker;

   worker = (Project_Thread *)data;

   assert(worker != NULL);

   PROGRESS_SEND(_("Export project as develop file"));
   PROGRESS_SEND(_("Export to file '%s'"), worker->edj);
   edje_edit_save_all(worker->project->global_object);
   eina_file_copy(worker->project->dev, worker->edj,
                  EINA_FILE_COPY_PERMISSION | EINA_FILE_COPY_XATTR,
                  NULL, NULL);
   PROGRESS_SEND("Export done");

   END_SEND(PM_PROJECT_SUCCESS);
   return NULL;
}

Project_Thread *
pm_project_develop_export(Project *project,
                          const char *path,
                          PM_Project_Progress_Cb func_progress,
                          PM_Project_End_Cb func_end,
                          const void *data)
{
   Project_Thread *worker;

   assert(project != NULL);
   assert(path != NULL);

   WORKER_CREATE(func_progress, func_end, data, project,
                 NULL, NULL, path, NULL, data);

   if (!eina_thread_create(&worker->thread, EINA_THREAD_URGENT, -1,
                           (void *)_develop_export, worker))
     {
        ERR("System error: can't create thread");
        abort();
     }

   return worker;
}

#ifdef HAVE_ENVENTOR
static void *
_enventor_save(void *data,
               Eina_Thread *thread __UNUSED__)
{
   Project_Thread *worker;
   Ecore_Event_Handler *cb_msg_stdout = NULL,
                       *cb_msg_stderr = NULL;
   Ecore_Exe_Flags flags  = ECORE_EXE_PIPE_READ |
                            ECORE_EXE_PIPE_READ_LINE_BUFFERED |
                            ECORE_EXE_PIPE_ERROR |
                            ECORE_EXE_PIPE_ERROR_LINE_BUFFERED;
   Eina_Stringshare *cmd, *edj, *dir;
   Ecore_Exe *exe_cmd;
   pid_t exe_pid;
   Eina_List *l;
   Eina_Strbuf *buf = NULL;
   int edje_cc_res = 0, edje_pick_res = 0, waitpid_res = 0;

   worker = (Project_Thread *)data;

   assert(worker != NULL);

   if (worker->func_progress)
     {
        cb_msg_stdout = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, _exe_data, worker);
        cb_msg_stderr = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, _exe_data, worker);
     }
   edj = eina_stringshare_printf("%s/build.edj", worker->project->enventor->path);
   buf = eina_strbuf_new();

   EINA_LIST_FOREACH(worker->project->res.images, l, dir)
     {
        eina_strbuf_append_printf(buf, " -id %s", dir);
     }
   EINA_LIST_FOREACH(worker->project->res.fonts, l, dir)
     {
        eina_strbuf_append_printf(buf, " -fd %s", dir);
     }
   EINA_LIST_FOREACH(worker->project->res.sounds, l, dir)
     {
        eina_strbuf_append_printf(buf, " -sd %s", dir);
     }
   cmd = eina_stringshare_printf("edje_cc -v %s %s %s", eina_strbuf_string_get(buf),
                                 worker->project->enventor->file, edj);
   eina_strbuf_free(buf);
   THREAD_TESTCANCEL;
   DBG("Run command for compile: %s", cmd);
   exe_cmd = ecore_exe_pipe_run(cmd, flags, NULL);
   exe_pid = ecore_exe_pid_get(exe_cmd);
   THREAD_TESTCANCEL;
   waitpid_res = waitpid(exe_pid, &edje_cc_res, 0);

   if (worker->func_progress)
     {
        ecore_event_handler_del(cb_msg_stdout);
        ecore_event_handler_del(cb_msg_stderr);
     }
   eina_stringshare_del(cmd);

   if ((waitpid_res == -1) ||
       (WIFEXITED(edje_cc_res) && (WEXITSTATUS(edje_cc_res) != 0 )))
     {
        END_SEND(PM_PROJECT_ERROR);
        return NULL;
     }
   THREAD_TESTCANCEL;

   if (worker->func_progress)
     {
        cb_msg_stdout = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, _exe_data, worker);
        cb_msg_stderr = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, _exe_data, worker);
     }
   worker->edj = eina_stringshare_printf("%s/enbuild.edj",
                                         worker->project->enventor->path);
   cmd = eina_stringshare_printf("edje_pick -o %s -a %s -i %s -g %s",
                                 worker->edj,
                                 worker->project->dev, edj,
                                 worker->project->current_style->full_group_name);

   DBG("Run command for compile: %s", cmd);
   exe_cmd = ecore_exe_pipe_run(cmd, flags, NULL);
   exe_pid = ecore_exe_pid_get(exe_cmd);
   THREAD_TESTCANCEL;
   waitpid_res = waitpid(exe_pid, &edje_pick_res, 0);

   if (worker->func_progress)
     {
        ecore_event_handler_del(cb_msg_stdout);
        ecore_event_handler_del(cb_msg_stderr);
     }

   if ((waitpid_res == -1) ||
       (WIFEXITED(edje_pick_res) && (WEXITSTATUS(edje_pick_res) != 0 )))
     {
        END_SEND(PM_PROJECT_ERROR);
        return NULL;
     }

   eina_file_copy(worker->edj, worker->project->dev,
                  EINA_FILE_COPY_PERMISSION | EINA_FILE_COPY_XATTR,
                  NULL, NULL);

   eina_stringshare_del(edj);
   END_SEND(PM_PROJECT_SUCCESS)

   return NULL;
}

Project_Thread *
pm_project_enventor_save(Project *project,
                         PM_Project_Progress_Cb func_progress,
                         PM_Project_End_Cb func_end,
                         const void *data)
{
   Project_Thread *worker;

   assert(project != NULL);

   WORKER_CREATE(func_progress, func_end, data, project,
                 NULL, NULL, NULL, NULL, NULL);

   if (!eina_thread_create(&worker->thread, EINA_THREAD_URGENT, -1,
                           (void *)_enventor_save, worker))
     {
        ERR("System error: can't create thread");
        abort();
     }

   return worker;
}
#endif /* HAVE_ENVENTOR */
