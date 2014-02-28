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
* along with this program; If not, see www.gnu.org/licenses/gpl-2.0.html.
*/

#include "test_ui_workspace.h"

/**
 * @addtogroup ui_workspace_test
 * @{
 * @addtogroup workspace_edit_object_unset
 * @{
 * ui_workspace
 * <TABLE>
 * @}
 */

/**
 * @addtogroup workspace_edit_object_unset
 * @{
 * <tr>
 * <td>workspace_edit_object_unset</td>
 * <td>workspace_edit_object_unset_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 create workspace object
 * @step 5 create style object
 * @step 6 load style data from edje file
 * @step 7 set edit object into workspace
 *
 * @procedure
 * @step 1 call workspace_edit_object_unset
 * @step 2 check returned value
 * </td>
 * <td>Evas_Object *workspace</td>
 * <td>Returned EINA_TRUE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_edit_object_unset_test_p)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Eina_Bool res = EINA_FALSE;
   Evas_Object *parent, *workspace;
   Style *style = NULL;
   Evas *e = NULL;

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   workspace = workspace_add(parent);
   e = evas_object_evas_get(parent);
   style = wm_style_add("test", "elm/radio/base/test");
   wm_style_data_load(style, e, "./edj_build/radio.edj");
   workspace_edit_object_set(workspace, style, "./edj_build/radio.edj");
   res = workspace_edit_object_unset(workspace);
   ck_assert_msg(res == EINA_TRUE, "Fail unset edit object from workspace");
   wm_style_free(style);
   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_edit_object_unset
 * @{
 * <tr>
 * <td>workspace_edit_object_unset</td>
 * <td>workspace_edit_object_unset_test_n</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 create workspace object
 *
 * @procedure
 * @step 1 call workspace_edit_object_unset
 * @step 2 check returned value
 * </td>
 * <td>Evas_Object *workspace</td>
 * <td>Returned EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_edit_object_unset_test_n)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Eina_Bool res = EINA_TRUE;
   Evas_Object *parent, *workspace;

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   workspace = workspace_add(parent);
   res = workspace_edit_object_unset(workspace);
   ck_assert_msg(res == EINA_FALSE, "Unset edit object from workspace without load");
   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_edit_object_unset
 * @{
 * <tr>
 * <td>workspace_edit_object_unset</td>
 * <td>workspace_edit_object_unset_test_n1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 call workspace_edit_object_unset with NULL param.
 * @step 2 check returned value
 * </td>
 * <td>NULL</td>
 * <td>Returned EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_edit_object_unset_test_n1)
{
   elm_init(0, 0);
   Eina_Bool res = EINA_TRUE;

   res = workspace_edit_object_unset(NULL);
   ck_assert_msg(res == EINA_FALSE, "Unset edit object from NULL workspace object");
   elm_shutdown();
}
END_TEST


/**
 * @addtogroup workspace_edit_object_unset
 * @{
 * </TABLE>
 * @}
 * @}
 */
