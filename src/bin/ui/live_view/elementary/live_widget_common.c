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

#include "live_view_prop.h"

Eina_Bool
standard_widget_name_parse(const char *full_group_name,
                           Eina_Stringshare **widget,
                           Eina_Stringshare **class,
                           Eina_Stringshare **style_name)
{
   char **c;
   unsigned int count;
   if (!full_group_name) return false;

   c = eina_str_split_full(full_group_name, "/", 4, &count);

   if (count < 4)
     {
        if (widget) *widget = NULL;
        if (class) *class = NULL;
        if (style_name) *style_name = NULL;

        free(c[0]);
        free(c);
        return false;
     }

   if (widget) *widget = eina_stringshare_add(c[1]);
   if (class) *class = eina_stringshare_add(c[2]);
   if (style_name) *style_name = eina_stringshare_add(c[3]);

   free(c[0]);
   free(c);
   return true;
}

void
on_swallow_check(void *data,
                 Evas_Object *obj,
                 void *ei __UNUSED__)
{
   Evas_Object *rect = NULL, *check = NULL, *ch;
   Eina_List *item_list = NULL, *it;
   Eina_Bool all_checks = true;

   Prop_Data *pd = (Prop_Data *)data;
   Evas_Object *object = pd->live_object;
   const char *part_name = elm_object_part_text_get(obj, NULL);
   check = elm_object_part_content_get(pd->prop_swallow.frame, "elm.swallow.check");

   if (elm_check_state_get(obj))
     {
        rect = evas_object_rectangle_add(object);
        evas_object_color_set(rect, RECT_COLOR);
        elm_object_part_content_set(object, part_name, rect);
        item_list = elm_box_children_get(pd->prop_swallow.swallows);

        EINA_LIST_FOREACH(item_list, it, ch)
          {
             if (elm_check_state_get(ch) == false)
               all_checks = false;
          }
        if (all_checks)
          elm_check_state_set(check, true);
        eina_list_free(item_list);
     }
   else
     {
        rect = elm_object_part_content_unset(object, part_name);
        evas_object_del(rect);
        if (elm_check_state_get(check)) elm_check_state_set(check, false);
     }
}

void
on_text_check(void *data,
              Evas_Object *obj,
              void *ei __UNUSED__)
{
   Evas_Object *check = NULL, *ch;
   Eina_List *item_list = NULL, *it;
   Eina_Bool all_checks = true;
   const char *default_text;

   Prop_Data *pd = (Prop_Data *)data;
   Evas_Object *object = pd->live_object;
   const char *part_name = elm_object_part_text_get(obj, NULL);
   check = elm_object_part_content_get(pd->prop_text.frame, "elm.swallow.check");

   if (elm_check_state_get(obj))
     {
        default_text = elm_object_part_text_get(object, part_name);
        if (default_text)
          eina_hash_add(pd->prop_text.default_text, part_name, eina_stringshare_add(default_text));
        elm_object_part_text_set(object, part_name,
                                 _("Look at it! This is absolutely and totally text"));
        item_list = elm_box_children_get(pd->prop_text.texts);

        EINA_LIST_FOREACH(item_list, it, ch)
          {
             if (elm_check_state_get(ch) == false)
               all_checks = false;
          }
        if (all_checks)
          elm_check_state_set(check, true);
        eina_list_free(item_list);
     }
   else
     {
        default_text = eina_hash_find(pd->prop_text.default_text, part_name);
        eina_hash_del(pd->prop_text.default_text, part_name, NULL);
        elm_object_part_text_set(object, part_name, default_text);
        if (elm_check_state_get(check)) elm_check_state_set(check, false);
     }
}

void
send_signal(void *data,
             Evas_Object *obj,
             void *ei __UNUSED__)
{
   Evas_Object *object = (Evas_Object *)data;
   const char *name = evas_object_data_get(obj, SIGNAL_NAME);
   const char *source = evas_object_data_get(obj, SIGNAL_SOURCE);

   elm_layout_signal_emit(object, name, source);
}
