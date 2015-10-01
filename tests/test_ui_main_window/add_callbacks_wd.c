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

#include "test_ui_main_window.h"

/**
 * @addtogroup ui_main_window_test
 * @{
 * @addtogroup add_callbacks_wd
 * @{
 * ui_main_window
 * <TABLE>
 * @}
 */

/**
 * @addtogroup add_callbacks_wd
 * @{
 * <tr>
 * <td>add_callbacks_wd</td>
 * <td>add_callbacks_wd_test_p1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 initialize application with app_init() function
 * @step 3 create application data
 * @step 4 create main window
 * @step 5 create widget list.
 *
 * @procedure
 * @step 1 Call add_callbacks_wd(widget_list, app_data)
 * @step 2 Check returned value
 * </td>
 * <td>Evas_Object *widget_list, App_Data *app_data</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST (add_callbacks_wd_test_p1)
{
   elm_init(0, 0);
   Evas_Object *widget_list;
   Eina_Bool result = EINA_FALSE;

   app_init();
   ui_main_window_add();
   widget_list = ui_widget_list_add(ap.win);

   result = add_callbacks_wd(widget_list);
   ck_assert_msg(result == EINA_TRUE, "Could not register callbacks!");

   app_shutdown();
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup add_callbacks_wd
 * @{
 * </TABLE>
 * @}
 * @}
 */
