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

#include "test_ui_signal_list.h"
#include "widget_manager.h"

/**
 * @addtogroup ui_signal_list_test
 * @{
 * @addtogroup ui_signal_list_data_set
 * @{
 * Signal List
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ui_signal_list_data_set
 * @{
 * <tr>
 * <td>ui_signal_list_data_set</td>
 * <td>ui_signal_list_data_set_test_p1</td>
 * <td>
 * @precondition
 * @step 1 initialized elm.
 * @step 2 created Window.
 * @step 3 Mmap edj file.
 * @step 4 Style filled with data. (style contain programs and signals)
 * @step 5 created Signal List
 *
 * @procedure
 * @step 1 Call function ui_signal_list_data_set.
 * @step 2 Check returned value.
 * </td>
 * <td>(Evas_Object *) gl_signals, (Style *) style</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ui_signal_list_data_set_test_p1)
{
   Evas_Object *window, *gl_signals;
   Evas *e;
   Style *style;
   const char *edj = "./edj_build/ui_signal_list_data_set.edj";
   const char *style_name = "def";
   const char *full_style_name = "elm/radio/base/def";
   Eina_File *mmap_file = NULL;

   elm_init(0,0);

   window = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   mmap_file = eina_file_open(edj, EINA_FALSE);
   e = evas_object_evas_get(window);
   style = wm_style_add(style_name, full_style_name, STYLE, NULL);
   wm_style_data_load(style, e, mmap_file);
   gl_signals = ui_signal_list_add(window);
   ck_assert_msg(ui_signal_list_data_set(gl_signals, style) == EINA_TRUE,
                 "Data not setted to the Signal List");

   wm_style_free(style);
   evas_object_del(window);
   eina_file_close(mmap_file);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_signal_list_data_set
 * @{
 * <tr>
 * <td>ui_signal_list_data_set</td>
 * <td>ui_signal_list_data_set_test_p2</td>
 * <td>
 * @precondition
 * @step 1 initialized elm.
 * @step 2 created Window.
 * @step 3 Mmap edj file.
 * @step 4 Style filled with data. (style doesn't contain any programs and signals)
 * @step 5 created Signal List
 *
 * @procedure
 * @step 1 Call function ui_signal_list_data_set.
 * @step 2 Check returned value.
 * </td>
 * <td>(Evas_Object *) gl_signals, (Style *) style</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ui_signal_list_data_set_test_p2)
{
   Evas_Object *window, *gl_signals;
   Evas *e;
   Style *style;
   const char *edj = "./edj_build/ui_signal_list_data_set.edj";
   const char *style_name = "def";
   const char *full_style_name = "elm/radio/notbase/test";
   Eina_File *mmap_file = NULL;

   elm_init(0,0);

   window = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   e = evas_object_evas_get(window);
   mmap_file = eina_file_open(edj, EINA_FALSE);
   style = wm_style_add(style_name, full_style_name, STYLE, NULL);
   wm_style_data_load(style, e, mmap_file);
   gl_signals = ui_signal_list_add(window);
   ck_assert_msg(ui_signal_list_data_set(gl_signals, style) == EINA_TRUE,
                 "Data not setted to the Signal List");

   wm_style_free(style);
   evas_object_del(window);
   eina_file_close(mmap_file);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_signal_list_data_set
 * @{
 * <tr>
 * <td>ui_signal_list_data_set</td>
 * <td>ui_signal_list_data_set_test_p3</td>
 * <td>
 * @precondition
 * @step 1 initialized elm.
 * @step 2 created Window.
 * @step 3 Mmap edj file.
 * @step 3 Style filled with data.
 * @step 4 created Signal List.
 * @step 5 Signal List filled with data already.
 *
 * @procedure
 * @step 1 Call function ui_signal_list_data_set with same style.
 * @step 2 Check returned value.
 * </td>
 * <td>(Evas_Object *) gl_signals, (Style *) style</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ui_signal_list_data_set_test_p3)
{
   Evas_Object *window, *gl_signals;
   Evas *e;
   Style *style;
   const char *edj = "./edj_build/ui_signal_list_data_set.edj";
   const char *style_name = "def";
   const char *full_style_name = "elm/radio/base/def";
   Eina_File *mmap_file = NULL;

   elm_init(0,0);

   window = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   mmap_file = eina_file_open(edj, EINA_FALSE);
   e = evas_object_evas_get(window);
   style = wm_style_add(style_name, full_style_name, STYLE, NULL);
   wm_style_data_load(style, e, mmap_file);
   gl_signals = ui_signal_list_add(window);
   ui_signal_list_data_set(gl_signals, style);
   ck_assert_msg(ui_signal_list_data_set(gl_signals, style) == EINA_TRUE,
                 "Data not setted to the Signal List");

   wm_style_free(style);
   evas_object_del(window);
   eina_file_close(mmap_file);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_signal_list_data_set
 * @{
 * <tr>
 * <td>ui_signal_list_data_set</td>
 * <td>ui_signal_list_data_set_test_p4</td>
 * <td>
 * @precondition
 * @step 1 initialized elm.
 * @step 2 created Window.
 * @step 3 Mmap edj file.
 * @step 4 Style filled with data.
 * @step 5 another Style. (style doesn't contain any programs or signals)
 * @step 6 created Signal List.
 * @step 7 Signal List filled with data already.
 *
 * @procedure
 * @step 1 Call function ui_signal_list_data_set with another style
 *         (that means, the style without any programs or signals).
 * @step 2 Check returned value.
 *
 * </td>
 * <td>(Evas_Object *) gl_signals, (Style *) style</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ui_signal_list_data_set_test_p4)
{
   Evas_Object *window, *gl_signals;
   Evas *e;
   Style *style, *another_style;
   const char *edj = "./edj_build/ui_signal_list_data_set.edj";
   const char *style_name = "def";
   const char *full_style_name = "elm/radio/base/def";
   const char *another_style_name = "default";
   const char *another_full_style_name = "elm/radio/base/test";
   Eina_File *mmap_file = NULL;


   elm_init(0,0);

   window = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   mmap_file = eina_file_open(edj, EINA_FALSE);

   e = evas_object_evas_get(window);
   style = wm_style_add(style_name, full_style_name, STYLE, NULL);
   another_style = wm_style_add(another_style_name, another_full_style_name, STYLE, NULL);
   wm_style_data_load(style, e, mmap_file);
   wm_style_data_load(another_style, e, mmap_file);
   gl_signals = ui_signal_list_add(window);
   ui_signal_list_data_set(gl_signals, style);
   ck_assert_msg(ui_signal_list_data_set(gl_signals, another_style) == EINA_TRUE,
                 "Data not resetted to the Signal List");

   wm_style_free(style);
   wm_style_free(another_style);
   evas_object_del(window);
   eina_file_close(mmap_file);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_signal_list_data_set
 * @{
 * </TABLE>
 * @}
 * @}
 */
