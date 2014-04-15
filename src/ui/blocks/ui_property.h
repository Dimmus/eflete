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

#ifndef UI_PROPERTY_1_H
#define UI_PROPERTY_1_H

/**
 * @defgroup Property Property
 * @ingroup Block
 *
 * This widget designed for show group and part details.
 */

#include "widget_manager.h"
#include "ui_workspace.h"

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
 * Set to current 'property' widget a style view which style details.
 * If current property has a style view, then old view will be removed.
 *
 * @param property The 'property' widget.
 * @param style The Style data which will be displayed.
 * @param workspace The workspace object with editable object.
 *
 * @note If the parameter style and editable style in the workspace not matched
 * propety do not set, and function will return EINA_FALSE.
 *
 * @ingroup Property
 */
Eina_Bool
ui_property_style_set(Evas_Object *property, Style *style, Evas_Object *workspace);

/**
 * Unset the style view content on a given property.
 * This method cascade unset all prop view:
 * style -> part -> state
 *
 * @param property the 'property' widget.
 *
 * @ingroup property
 */
void
ui_property_style_unset(Evas_Object *property);

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
