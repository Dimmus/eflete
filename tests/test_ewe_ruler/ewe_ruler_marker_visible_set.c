
#include "test_ewe_ruler.h"

/**
 * @addtogroup ewe_ruler_test
 * @{
 * @addtogroup ewe_ruler_marker_visible_set
 * @{
 * ewe_ruler
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ewe_ruler_marker_visible_set
 * @{
 * <tr>
 * <td>ewe_ruler_marker_visible_set</td>
 * <td>ewe_ruler_marker_visible_set_test_p</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 create window
 * @step 3 add ewe_ruler
 * @step 4 add marker to ruler
 *
 * @procedure
 * @step 1 call ewe_ruler_marker_visible_set(ruler, marker, EINA_FALSE)
 * @step 2 check that returned value is EINA_TRUE
 * @step 3 check that value is setted
 * @step 4 call ewe_ruler_marker_visible_set(ruler, marker, EINA_TRUE)
 * @step 5 check that returned value is EINA_TRUE
 * @step 6 check that value is setted
 * </td>
 * <td>Evas_Object *ruler, Ewe_Ruler_Marker* marker</td>
 * <td>EINA_TRUE returned</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFL_START_TEST(ewe_ruler_marker_visible_set_test_p)
{
   logger_init();
   elm_init(0, 0);
   app_init();
   Evas_Object *win = elm_win_util_standard_add("test", "test");
   Evas_Object *ruler = ewe_ruler_add(win);
   Ewe_Ruler_Marker *marker = ewe_ruler_marker_add(ruler, NULL);

   ck_assert_msg(ewe_ruler_marker_visible_set(ruler, marker, EINA_FALSE) == EINA_TRUE, "EINA_FALSE returned");
   ck_assert_msg(ewe_ruler_marker_visible_get(ruler, marker) == EINA_FALSE, "Value not setted");
   ck_assert_msg(ewe_ruler_marker_visible_set(ruler, marker, EINA_TRUE) == EINA_TRUE, "EINA_FALSE returned");
   ck_assert_msg(ewe_ruler_marker_visible_get(ruler, marker) == EINA_TRUE, "Value not setted");

   evas_object_del(win);
   app_shutdown();
   elm_shutdown();
}
EFL_END_TEST

/**
 * @addtogroup ewe_ruler_marker_visible_set
 * @{
 * <tr>
 * <td>ewe_ruler_marker_visible_set</td>
 * <td>ewe_ruler_marker_visible_set_test_n</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 don't create ruler object
 *
 * @procedure
 * @step 1 call ewe_ruler_marker_visible_set
 * @step 2 check returned value
 * </td>
 * <td>NULL, NULL</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFL_START_TEST(ewe_ruler_marker_visible_set_test_n)
{
   logger_init();
   elm_init(0, 0);
   app_init();

   ck_assert_msg(ewe_ruler_marker_visible_set(NULL, NULL, EINA_FALSE) == EINA_FALSE, "Setted visibility from NULL ruler object");

   app_shutdown();
   elm_shutdown();
}
EFL_END_TEST

/**
 * @addtogroup ewe_ruler_marker_visible_set
 * @{
 * </TABLE>
 * @}
 * @}
 */
