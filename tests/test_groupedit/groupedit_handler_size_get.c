/**
 * Edje Theme Editor
 * Copyright (C) 2013-2014 Samsung Electronics.
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

#include "test_groupedit.h"

/**
 * @addtogroup groupedit_test
 * @{
 * @addtogroup groupedit_handler_size_get
 * @{
 * Groupedit
 * <TABLE>
 * @}
 */

/**
 * @addtogroup groupedit_handler_size_get
 * @{
 * <tr>
 * <td>groupedit_handler_size_get</td>
 * <td>groupedit_handler_size_get_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 create parent window
 * @step 3 create a groupedit
 *
 * @procedure
 * @step 1 get the new size for groupedit handlers
 * @step 2 check size of groupedit handlers
 * </td>
 * <td>(Evas_Object *) groupedit</td>
 * <td>5, 5, 5, 5</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(groupedit_handler_size_get_test_p)
{
   Evas_Object *parent, *groupedit;
   int lt_w, lt_h, br_w, br_h;

   elm_init(0, 0);
   lt_w = lt_h = br_w = br_h = 0;

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);

   groupedit = groupedit_add(parent);
   groupedit_handler_size_get(groupedit, &lt_w, &lt_h, &br_w, &br_h);
   ck_assert_msg(((lt_w == 5) || (lt_h == 5) || (br_w == 5) || (br_h == 5)),
                 "Failed to set the groupedit handlers size.");

   evas_object_del(parent);

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup groupedit_handler_size_get
 * @{
 * <tr>
 * <td>groupedit_handler_size_get</td>
 * <td>groupedit_handler_size_get_test_n</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 create parent window
 *
 * @procedure
 * @step 1 get the new size for groupedit handlers
 * @step 2 check size of groupedit handlers
 * </td>
 * <td>NULL</td>
 * <td>0, 0, 0, 0</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(groupedit_handler_size_get_test_n)
{
   int lt_w, lt_h, br_w, br_h;

   elm_init(0, 0);
   lt_w = lt_h = br_w = br_h = 0;

   groupedit_handler_size_get(NULL, &lt_w, &lt_h, &br_w, &br_h);
   ck_assert_msg(((lt_w == 0) || (lt_h == 0) || (br_w == 0) || (br_h == 0)),
                 "Failed to set the groupedit handlers size.");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup groupedit_handler_size_get
 * @{
 * </TABLE>
 * @}
 * @}
 */
