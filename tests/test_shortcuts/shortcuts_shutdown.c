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

#include "test_shortcuts.h"

/**
 * @addtogroup shortcuts_test
 * @{
 * @addtogroup shortcuts_shutdown
 * @{
 * Shortcuts
 * <TABLE>
 * @}
 */

/**
 * @addtogroup shortcuts_shutdown
 * @{
 * <tr>
 * <td>shortcuts_shutdown</td>
 * <td>shortcuts_shutdown_test_p1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 create App_Data
 *
 * @procedure
 * @step 1 call shortcuts_init
 * @step 2 call shortcuts_shutdown
 * </td>
 * <td>App_Data *ap</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST(shortcuts_shutdown_test_p1)
{
   elm_init(0,0);
   app_init();

   app_shutdown();
   ck_assert_msg(ap.shortcuts == NULL, "Can't shutdown shortcuts.");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup shortcuts_shutdown
 * @{
 * </TABLE>
 * @}
 * @}
 */
