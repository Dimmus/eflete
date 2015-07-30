/**
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

#include "test_ui_workspace.h"

/**
 * @addtogroup ui_workspace_test
 * @{
 * @addtogroup workspace_edit_object_part_restack
 * @{
 * ui_workspace
 * <TABLE>
 * @}
 */

/**
 * @addtogroup workspace_edit_object_part_restack
 * @{
 * <tr>
 * <td>workspace_edit_object_part_restack</td>
 * <td>workspace_edit_object_part_restack_test_p1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 Mmap edj file.
 * @step 5 create workspace object
 * @step 6 create style object
 * @step 7 load style data from edje file
 * @step 8 set edit object into workspace
 *
 * @procedure
 * @step 1 Restack exist part below another exist part.
 * </td>
 * <td>(Evas_Object *)workspace, (const char *)"elm.swallow.content", (const char*)"elm.text", (Eina_Bool) EINA_FALSE</td>
 * <td>EINA_TRUE returned</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_edit_object_part_restack_test_p1)
{
   elm_init(0, 0);
   app_init();
   Evas_Object *parent, *workspace;
   Eina_Bool ret = EINA_FALSE;
   Style *style = NULL;
   Evas *e = NULL;
   Eina_File *mmap_file = NULL;

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   mmap_file = eina_file_open("./edj_build/workspace_edit_object_part_restack.edj", EINA_FALSE);
   workspace = workspace_add(parent);
   e = evas_object_evas_get(parent);
   style = wm_style_add("test", "elm/radio/base/def", STYLE, NULL);
   wm_style_data_load(style, e, mmap_file);
   workspace_edit_object_set(workspace, style, "./edj_build/workspace_edit_object_part_restack.edj");

   ret = workspace_edit_object_part_restack(workspace, "elm.swallow.content", "elm.text", EINA_FALSE);
   ck_assert_msg(ret == EINA_TRUE, "Failed below part \"elm.swallow.content\" relative to \"elm.text\"");

   wm_style_free(style);
   eina_file_close(mmap_file);
   workspace_edit_object_unset(workspace);
   evas_object_del(workspace);
   evas_object_del(parent);
   app_shutdown();
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_edit_object_part_restack
 * @{
 * <tr>
 * <td>workspace_edit_object_part_restack</td>
 * <td>workspace_edit_object_part_restack_test_p2</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 Mmap edj file.
 * @step 5 create workspace object
 * @step 6 create style object
 * @step 7 load style data from edje file
 * @step 8 set edit object into workspace
 *
 * @procedure
 * @step 1 Restack exist part below another exist part.
 * </td>
 * <td>(Evas_Object *)workspace, (const char *)"elm.swallow.content", (const char*)"bg", (Eina_Bool) EINA_TRUE</td>
 * <td>EINA_TRUE returned</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_edit_object_part_restack_test_p2)
{
   elm_init(0, 0);
   app_init();
   Evas_Object *parent, *workspace;
   Eina_Bool ret = EINA_FALSE;
   Style *style = NULL;
   Evas *e = NULL;
   Eina_File *mmap_file = NULL;

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   mmap_file = eina_file_open("./edj_build/workspace_edit_object_part_restack.edj", EINA_FALSE);
   workspace = workspace_add(parent);
   e = evas_object_evas_get(parent);
   style = wm_style_add("test", "elm/radio/base/def", STYLE, NULL);
   wm_style_data_load(style, e, mmap_file);
   workspace_edit_object_set(workspace, style, "./edj_build/workspace_edit_object_part_restack.edj");

   ret = workspace_edit_object_part_restack(workspace, "elm.swallow.content", "bg", EINA_TRUE);
   ck_assert_msg(ret == EINA_TRUE, "Failed below part \"elm.swallow.content\" relative to \"bg\"");

   wm_style_free(style);
   eina_file_close(mmap_file);
   workspace_edit_object_unset(workspace);
   evas_object_del(workspace);
   evas_object_del(parent);
   app_shutdown();
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_edit_object_part_restack
 * @{
 * </TABLE>
 * @}
 * @}
 */
