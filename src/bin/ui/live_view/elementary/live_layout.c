/*
 * Edje Theme Editor
 * Copyright (C) 2013-2015 Samsung Electronics.
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

#include "live_elementary_widgets.h"

Evas_Object *
widget_layout_create(Evas_Object *parent, const Style *style)
{
   assert(parent != NULL);
   assert(style != NULL);

   Eina_Stringshare *class;
   Eina_Stringshare *style_name;
   standard_widget_name_parse(style->full_group_name, NULL, &class, &style_name);

   Evas_Object *object = elm_layout_add(parent);
   elm_layout_theme_set(object, "layout", class, style_name);

   evas_object_data_set(object, SWALLOW_FUNC, on_swallow_check);
   evas_object_data_set(object, TEXT_FUNC, on_text_check);
   evas_object_data_set(object, SIGNAL_FUNC, send_signal);

   eina_stringshare_del(class);
   eina_stringshare_del(style_name);
   return object;
}
