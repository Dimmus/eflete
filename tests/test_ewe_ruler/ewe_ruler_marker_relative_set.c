
#include "test_ewe_ruler.h"

/**
 * @addtogroup ewe_ruler_test
 * @{
 * @addtogroup ewe_ruler_marker_relative_set
 * @{
 * ewe_ruler
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ewe_ruler_marker_relative_set
 * @{
 * <tr>
 * <td>ewe_ruler_marker_relative_set</td>
 * <td>ewe_ruler_marker_relative_set_test_p</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 create window
 * @step 3 add ewe_ruler
 * @step 4 add marker to ruler
 *
 * @procedure
 * @step 1 call ewe_ruler_marker_relative_set(ruler, marker, NULL, 125)
 * @step 2 check returned value
 * @step 3 check setted value
 * @step 4 call ewe_ruler_marker_relative_set(ruler, marker, NULL, -125)
 * @step 5 check returned value
 * @step 6 check setted value
 * </td>
 * <td>Evas_Object *ruler, Ewe_Ruler_Marker* marker</td>
 * <td>All checks passes</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFL_START_TEST(ewe_ruler_marker_relative_set_test_p)
{
   
   Evas_Object *win = elm_win_util_standard_add("test", "test");
   Evas_Object *ruler = ewe_ruler_add(win);
   Ewe_Ruler_Marker *marker = ewe_ruler_marker_add(ruler, NULL);

   ck_assert_msg(ewe_ruler_marker_relative_set(ruler, marker, NULL, 125.25) == EINA_TRUE, "Can't set rel position");
   ck_assert_msg(EINA_DBL_EQ(ewe_ruler_marker_relative_get(ruler, marker, NULL), 125.25), "Wrong value setted");
   ck_assert_msg(ewe_ruler_marker_relative_set(ruler, marker, NULL, -125.25) == EINA_TRUE, "Can't set rel position");
   ck_assert_msg(EINA_DBL_EQ(ewe_ruler_marker_relative_get(ruler, marker, NULL), -125.25), "Wrong value setted");

   evas_object_del(win);
}
EFL_END_TEST

/**
 * @addtogroup ewe_ruler_marker_relative_set
 * @{
 * <tr>
 * <td>ewe_ruler_marker_relative_set</td>
 * <td>ewe_ruler_marker_relative_set_test_n</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 create window
 * @step 3 add ewe_ruler
 * @step 4 add marker to ruler
 *
 * @procedure
 * @step 1 call ewe_ruler_marker_relative_set(NULL, marker, NULL, 125.25)
 * @step 2 check returned value
 * @step 3 call ewe_ruler_marker_relative_set(ruler, NULL, NULL, 125.25)
 * @step 4 check returned value
 * @step 5 call ewe_ruler_marker_relative_set(NULL, NULL, NULL, 125.25)
 * @step 6 check returned value
 * </td>
 * <td>NULL, NULL, 125.25</td>
 * <td>All check passed</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFL_START_TEST(ewe_ruler_marker_relative_set_test_n)
{
   
   Evas_Object *win = elm_win_util_standard_add("test", "test");
   Evas_Object *ruler = ewe_ruler_add(win);
   Ewe_Ruler_Marker *marker = ewe_ruler_marker_add(ruler, NULL);

   ck_assert_msg(ewe_ruler_marker_relative_set(NULL, marker, NULL, 125.25) == EINA_FALSE, "Setted rel position for NULL ruler object");
   ck_assert_msg(ewe_ruler_marker_relative_set(ruler, NULL, NULL, 125.25) == EINA_FALSE, "Setted rel position for NULL marker");
   ck_assert_msg(ewe_ruler_marker_relative_set(NULL, NULL, NULL, 125.25) == EINA_FALSE, "Setted rel position for NULL ruler object for NULL marker");

}
EFL_END_TEST

/**
 * @addtogroup ewe_ruler_marker_relative_set
 * @{
 * </TABLE>
 * @}
 * @}
 */

void ewe_ruler_marker_relative_set_test(TCase *tc)
{
   tcase_add_test(tc, ewe_ruler_marker_relative_set_test_p);
   tcase_add_test(tc, ewe_ruler_marker_relative_set_test_n);
}