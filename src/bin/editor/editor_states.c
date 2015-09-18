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

#include "editor.h"
#include "editor_macro.h"

EDITOR_STATE_DOUBLE(rel1_relative_x, ATTRIBUTE_STATE_REL1_RELATIVE_X)
EDITOR_STATE_DOUBLE(rel1_relative_y, ATTRIBUTE_STATE_REL1_RELATIVE_Y)
EDITOR_STATE_DOUBLE(rel2_relative_x, ATTRIBUTE_STATE_REL2_RELATIVE_X)
EDITOR_STATE_DOUBLE(rel2_relative_y, ATTRIBUTE_STATE_REL2_RELATIVE_Y)

EDITOR_STATE_DOUBLE(rel1_offset_x, ATTRIBUTE_STATE_REL1_OFFSET_X)
EDITOR_STATE_DOUBLE(rel1_offset_y, ATTRIBUTE_STATE_REL1_OFFSET_Y)
EDITOR_STATE_DOUBLE(rel2_offset_x, ATTRIBUTE_STATE_REL2_OFFSET_X)
EDITOR_STATE_DOUBLE(rel2_offset_y, ATTRIBUTE_STATE_REL2_OFFSET_Y)

EDITOR_STATE_DOUBLE(align_x, ATTRIBUTE_STATE_ALIGN_X)
EDITOR_STATE_DOUBLE(align_y, ATTRIBUTE_STATE_ALIGN_Y)

EDITOR_STATE_DOUBLE(aspect_min, ATTRIBUTE_STATE_ASPECT_MIN)
EDITOR_STATE_DOUBLE(aspect_max, ATTRIBUTE_STATE_ASPECT_MAX)

EDITOR_STATE_DOUBLE(fill_origin_relative_x, ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X)
EDITOR_STATE_DOUBLE(fill_origin_relative_y, ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y)
EDITOR_STATE_DOUBLE(fill_origin_offset_x, ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X)
EDITOR_STATE_DOUBLE(fill_origin_offset_y, ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y)

EDITOR_STATE_DOUBLE(fill_size_relative_x, ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X)
EDITOR_STATE_DOUBLE(fill_size_relative_y, ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y)
EDITOR_STATE_DOUBLE(fill_size_offset_x, ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X)
EDITOR_STATE_DOUBLE(fill_size_offset_y, ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y)

#define MAX_SET(VAL, VAL_CAPS) \
Eina_Bool \
editor_state_max_## VAL ##_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, \
                           const char *part_name, const char *state_name, double state_val, int new_value) \
{ \
   int old_value; \
   int min_value; \
   Diff *diff; \
   Attribute attribute = ATTRIBUTE_STATE_MAX_##VAL_CAPS; \
 \
   assert(edit_object != NULL); \
   assert(new_value >= 0); \
 \
   old_value = edje_edit_state_max_## VAL ##_get(edit_object, part_name, state_name, state_val); \
   min_value = edje_edit_state_min_## VAL ##_get(edit_object, part_name, state_name, state_val); \
 \
   if ((min_value > new_value) && (new_value != 0)) \
     new_value = min_value; \
 \
   if (change) \
     { \
        diff = mem_calloc(1, sizeof(Diff)); \
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->redo.function = editor_state_max_## VAL ##_set; \
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->redo.args.type_ssdi.d3 = state_val; \
        diff->redo.args.type_ssdi.i4 = new_value; \
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->undo.function = editor_state_max_## VAL ##_set; \
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->undo.args.type_ssdi.d3 = state_val; \
        diff->undo.args.type_ssdi.i4 = old_value; \
        if (merge) \
          change_diff_merge_add(change, diff); \
        else \
          change_diff_add(change, diff); \
     } \
   if (!edje_edit_state_max_## VAL ##_set(edit_object, part_name, state_name, state_val, new_value)) \
     return false; \
   _editor_project_changed(); \
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute); \
   return true; \
}

MAX_SET(w, W)
MAX_SET(h, H)

#define MIN_SET(VAL, VAL_CAPS) \
Eina_Bool \
editor_state_min_## VAL ##_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, \
                           const char *part_name, const char *state_name, double state_val, int new_value) \
{ \
   int old_value; \
   int max_value; \
   Diff *diff; \
   Attribute attribute = ATTRIBUTE_STATE_MIN_##VAL_CAPS; \
 \
   assert(edit_object != NULL); \
   assert(new_value >= 0); \
 \
   old_value = edje_edit_state_min_## VAL ##_get(edit_object, part_name, state_name, state_val); \
   max_value = edje_edit_state_max_## VAL ##_get(edit_object, part_name, state_name, state_val); \
 \
   if ((max_value < new_value) && (max_value != 0)) \
     new_value = max_value; \
 \
   if (change) \
     { \
        diff = mem_calloc(1, sizeof(Diff)); \
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->redo.function = editor_state_min_## VAL ##_set; \
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->redo.args.type_ssdi.d3 = state_val; \
        diff->redo.args.type_ssdi.i4 = new_value; \
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->undo.function = editor_state_min_## VAL ##_set; \
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->undo.args.type_ssdi.d3 = state_val; \
        diff->undo.args.type_ssdi.i4 = old_value; \
        if (merge) \
          change_diff_merge_add(change, diff); \
        else \
          change_diff_add(change, diff); \
     } \
   if (!edje_edit_state_min_## VAL ##_set(edit_object, part_name, state_name, state_val, new_value)) \
     return false; \
   _editor_project_changed(); \
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute); \
   return true; \
}

MIN_SET(w, W)
MIN_SET(h, H)

EDITOR_STATE_BOOL(fixed_h, ATTRIBUTE_STATE_FIXED_H)
EDITOR_STATE_BOOL(fixed_w, ATTRIBUTE_STATE_FIXED_W)
EDITOR_STATE_BOOL(fill_smooth, ATTRIBUTE_STATE_FILL_SMOOTH)
EDITOR_STATE_BOOL(visible, ATTRIBUTE_STATE_VISIBLE)

EDITOR_STATE_STRING(rel1_to_x, ATTRIBUTE_STATE_REL1_TO_X)
EDITOR_STATE_STRING(rel1_to_y, ATTRIBUTE_STATE_REL1_TO_Y)
EDITOR_STATE_STRING(rel2_to_x, ATTRIBUTE_STATE_REL2_TO_X)
EDITOR_STATE_STRING(rel2_to_y, ATTRIBUTE_STATE_REL2_TO_Y)

EDITOR_STATE_STRING(color_class, ATTRIBUTE_STATE_COLOR_CLASS)

TODO("Replace with image stub")
EDITOR_STATE_STRING_WITH_FALLBACK(image, ATTRIBUTE_STATE_IMAGE, NULL)

EDITOR_STATE_INT_INT_INT_INT(color, ATTRIBUTE_STATE_COLOR)
EDITOR_STATE_INT_INT_INT_INT(color2, ATTRIBUTE_STATE_COLOR2)
EDITOR_STATE_INT_INT_INT_INT(color3, ATTRIBUTE_STATE_COLOR3)

EDITOR_STATE_INT_INT_INT_INT(image_border, ATTRIBUTE_STATE_IMAGE_BORDER)

EDITOR_STATE_UCHAR(image_border_fill, ATTRIBUTE_STATE_IMAGE_BORDER_FILL)
EDITOR_STATE_UCHAR(aspect_pref, ATTRIBUTE_STATE_ASPECT_PREF)
