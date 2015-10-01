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

static inline Eina_Bool
_editor_save(Evas_Object *edit_object, Eina_Bool current_group)
{
   assert(edit_object != NULL);

   if (!edje_edit_without_source_save(edit_object, current_group))
     return false;

   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_SAVED, NULL);
   return true;
}
Eina_Bool
editor_save(Evas_Object *edit_object)
{
   return _editor_save(edit_object, true);
}

Eina_Bool
editor_save_all(Evas_Object *edit_object)
{
   return _editor_save(edit_object, false);
}

Eina_Bool
editor_internal_group_add(Evas_Object *edit_object)
{
   assert(edit_object != NULL);

   if (edje_edit_group_exist(edit_object, EFLETE_INTERNAL_GROUP_NAME))
     return true;

   if (!edje_edit_group_add(edit_object, EFLETE_INTERNAL_GROUP_NAME))
     return false;
   if (!edje_edit_without_source_save(edit_object, false))
     return false;
   return true;
}
