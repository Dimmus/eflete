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

#include "test_edje_compile.h"

Suite* test_suite (void) {
   Suite *suite = suite_create("edje_compile_test");
   TCase *tcase = tcase_create("TCase");
   tcase_add_test(tcase, compile_callback_test_p);
   tcase_add_test(tcase, compile_callback_test_n);
   tcase_add_test(tcase, compile_test_p);
   tcase_add_test(tcase, compile_test_n1);
   tcase_add_test(tcase, compile_test_n2);
   tcase_add_test(tcase, compile_test_n3);

   tcase_add_test(tcase, decompile_callback_test_p);
   tcase_add_test(tcase, decompile_test_p);
   tcase_add_test(tcase, decompile_test_n1);
   tcase_add_test(tcase, decompile_test_n2);
   suite_add_tcase(suite, tcase);
   return suite;
}

int main(void) {
   int number_failed;
   Suite *suite = test_suite();
   SRunner *runner = srunner_create(suite);
   srunner_set_xml (runner, "test_edje_compile.xml");
   srunner_run_all(runner, CK_VERBOSE);
   number_failed = srunner_ntests_failed(runner);
   srunner_free(runner);
   return number_failed;
}
