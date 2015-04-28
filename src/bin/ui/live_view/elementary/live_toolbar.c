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
#include "live_view_prop.h"

static Evas_Object *
_create_toolbar(Evas_Object *obj, const char *class, const char *style)
{
   Evas_Object *layout;
   Evas_Object *tb;
   Elm_Object_Item *tb_it;
   Evas_Object *bt;

   layout = elm_layout_add(obj);
   elm_layout_theme_set(layout, "layout", "live_view", "toolbar_helper");
   evas_object_show(layout);
   tb = elm_toolbar_add(layout);
   elm_layout_content_set(layout, "elm.swallow.content", tb);

   if (strcmp(class, "more") != 0)
     elm_toolbar_shrink_mode_set(tb, ELM_TOOLBAR_SHRINK_SCROLL);
   else
     elm_toolbar_shrink_mode_set(tb, ELM_TOOLBAR_SHRINK_EXPAND);
   evas_object_size_hint_weight_set(tb, 0.0, 0.0);
   evas_object_size_hint_align_set(tb, 0.0, 0.0);

   elm_toolbar_homogeneous_set(tb, false);
   evas_object_show(tb);
   int i;
   elm_toolbar_standard_priority_set(tb, 50);
   for (i = 0; i < 6; i++)
     {
        bt = elm_button_add(tb);
        elm_object_part_text_set(bt, NULL, _("Object"));
        tb_it = elm_toolbar_item_append(tb, NULL, NULL, NULL, NULL);
        elm_toolbar_item_priority_set(tb_it, 20 * i);
        elm_object_item_part_content_set(tb_it, NULL, bt);
        tb_it = elm_toolbar_item_append(tb, "folder-new", _("Enabled"), NULL, NULL);
        elm_toolbar_item_priority_set(tb_it, 20 * i);
        tb_it = elm_toolbar_item_append(tb, "folder-new", _("Disabled"), NULL, NULL);
        elm_toolbar_item_priority_set(tb_it, 20 * i);
        elm_object_item_disabled_set(tb_it, true);
        tb_it = elm_toolbar_item_append(tb, NULL, NULL, NULL, NULL);
        elm_toolbar_item_priority_set(tb_it, 20 * i);
        elm_toolbar_item_separator_set(tb_it, true);
     }
   elm_object_style_set(tb, style);

   return layout;
}

Evas_Object *
widget_toolbar_create(Evas_Object *parent, const Style *style)
{
   Eina_Stringshare *class;
   Eina_Stringshare *style_name;

   standard_widget_name_parse(style->full_group_name, NULL, &class, &style_name);

   Evas_Object *object;
   object = _create_toolbar(parent, class, style_name);
   evas_object_show(object);

   evas_object_data_set(object, SWALLOW_FUNC, on_swallow_check);
   evas_object_data_set(object, TEXT_FUNC, on_text_check);
   evas_object_data_set(object, SIGNAL_FUNC, send_signal);

   eina_stringshare_del(class);
   eina_stringshare_del(style_name);
   return object;
}
