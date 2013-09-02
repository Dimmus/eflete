#ifndef UI_PROPERTY_1_H
#define UI_PROPERTY_1_H

/**
 * @defgroup Property Property
 * @ingroup Block
 *
 * This widget designed for show group and part details.
 */

#include "efl_tet.h"
#include "widget_manager.h"
#include "widget_define.h"

/**
 * Add a new Property widget.
 *
 * @param parent The parent widget.
 * @return A new 'property' widget handler or NULL, on errors
 *
 * @ingroup Property
 */
Evas_Object *
ui_property_add(Evas_Object *parent);

/**
 * Set to current 'property' widget a group view which group details.
 * If current property has a group view, then old view will be removed.
 *
 * @param property The 'property' widget.
 * @param group The Group data which will be displayed.
 *
 * @ingroup Property
 */
Eina_Bool
ui_property_group_set(Evas_Object *property, Group *group);

/**
 * Unset the group view content on a given property.
 * This method cascade unset all prop view:
 * group -> part -> state
 *
 * @param property the 'property' widget.
 *
 * @ingroup property
 */
void
ui_property_group_unset(Evas_Object *property);

/**
 * Set to current 'property' widget a part view which part details.
 * If current property has a part view, then old view will be removed.
 *
 * @param property The 'property' widget.
 * @param part The Part data which will be displayed.
 *
 * @ingroup Property
 */

Eina_Bool
ui_property_part_set(Evas_Object *property, Part *part);

/**
 * Unset the part view content on a given property.
 * This method cascade unset all prop view:
 * part -> state
 *
 * @param property the 'property' widget.
 *
 * @ingroup property
 */
void
ui_property_part_unset(Evas_Object *property);

/**
 * Set to current 'property' widget a part state view which state details.
 * If current property has a part view, then old view will be removed.
 *
 * @param property The 'property' widget.
 * @param part The Part data which will be displayed.
 *
 * @ingroup Property
 */
Eina_Bool
ui_property_state_set(Evas_Object *property, Part *part);

/**
 * Unset the part state view content on a given property.
 *
 * @param property the 'property' widget.
 *
 * @ingroup property
 */
void
ui_property_state_unset(Evas_Object *property);

#endif /* UI_PROPERTY_1_H */
