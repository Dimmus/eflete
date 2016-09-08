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

#define ALLOW_DIRECT_EDJE_EDIT_CALLS
#include "editor.h"
#include "editor_macro.h"
#include "change.h"
#include "diff.h"

extern int _editor_signals_blocked;

Eina_Bool
editor_image_add(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_image_add(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_IMAGE_ADDED, (void *)name);
   return true;
}

Eina_Bool
editor_image_del(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_image_del(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_IMAGE_DELETED, (void *)name);
   return true;
}

Eina_Bool
editor_color_class_add(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   if (!edje_edit_color_class_add(obj, name))
     return false;

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_COLORCLASS_ADDED, (void *)name);
   return true;
}

Eina_Bool
editor_color_class_del(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_color_class_del(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_COLORCLASS_DELETED, (void *)name);
   return true;
}

Eina_Bool
editor_color_class_description_set(Evas_Object *obj, const char *name, const char *description)
{
   assert(obj != NULL);
   assert(name != NULL);

   Attribute attribute = ATTRIBUTE_RESOURCES_COLORCLASS_DESCRIPTION;

   CRIT_ON_FAIL(edje_edit_color_class_description_set(obj, name, description));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (!_editor_signals_blocked)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_RESOURCE_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

Eina_Bool
editor_color_class_colors_set(Evas_Object *obj, const char *name,
                              int r, int g, int b, int a,
                              int r2, int g2, int b2, int a2,
                              int r3, int g3, int b3, int a3)
{
   assert(obj != NULL);
   assert(name != NULL);

   Attribute attribute = ATTRIBUTE_RESOURCES_COLORCLASS_COLORS;

   CRIT_ON_FAIL(edje_edit_color_class_colors_set(obj, name,
                                                 r, g, b, a,
                                                 r2, g2, b2, a2,
                                                 r3, g3, b3, a3));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (!_editor_signals_blocked)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_RESOURCE_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

Eina_Bool
editor_sound_sample_add(Evas_Object *obj, const char *name, const char *source, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   if (!edje_edit_sound_sample_add(obj, name, source))
     return false;

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_SOUND_ADDED, (void *)name);
   return true;
}

Eina_Bool
editor_sound_sample_del(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_sound_sample_del(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_SOUND_DELETED, (void *)name);
   return true;
}

Eina_Bool
editor_sound_tone_add(Evas_Object *obj, const char *name, int frq, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_sound_tone_add(obj, name, frq));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_TONE_ADDED, (void *)name);
   return true;
}

Eina_Bool
editor_sound_tone_del(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_sound_tone_del(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_TONE_DELETED, (void *)name);
   return true;
}

Eina_Bool
editor_style_add(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_style_add(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_STYLE_ADDED, (void *)name);
   return true;
}

Eina_Bool
editor_style_del(Evas_Object *obj, const char *name, Eina_Bool notify)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_style_del(obj, name));

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (notify)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_STYLE_DELETED, (void *)name);
   return true;
}

Eina_Bool
editor_style_tag_add(Evas_Object *obj, const char *name, const char *tag)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_style_tag_add(obj, name, tag));
   Attribute attribute = ATTRIBUTE_RESOURCES_STYLE_TAG_ADDED;

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (!_editor_signals_blocked)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_RESOURCE_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

Eina_Bool
editor_style_tag_del(Evas_Object *obj, const char *name, const char *tag)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_style_tag_del(obj, name, tag));
   Attribute attribute = ATTRIBUTE_RESOURCES_STYLE_TAG_DELETED;

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (!_editor_signals_blocked)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_RESOURCE_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

Eina_Bool
editor_style_tag_value_set(Evas_Object *obj, const char *name, const char *tag, const char *value)
{
   assert(obj != NULL);
   assert(name != NULL);

   CRIT_ON_FAIL(edje_edit_style_tag_value_set(obj, name, tag, value));
   Attribute attribute = ATTRIBUTE_RESOURCES_STYLE_TAG_DELETED;

   if (!editor_save(obj))
     return false; /* i hope it will never happen */
   _editor_project_changed();
   if (!_editor_signals_blocked)
     evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_RESOURCE_ATTRIBUTE_CHANGED, &attribute);
   return true;
}