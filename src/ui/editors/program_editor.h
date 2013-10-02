/* Edje Theme Editor
* Copyright (C) 2013 Samsung Electronics.
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
* along with this program; If not, see .
*/

#ifndef PROGRAM_EDITOR_H
#define PROGRAM_EDITOR_H

#define TRANSITIONS_COUNT 14

/**
 * @defgroup ProgramEditor Program Editor
 *
 * Program editor is subwindow that contain information about
 * all the programs used by the project.
 */

#include "efl_ete.h"
#include "modal_window.h"

/**
 * Add new program editor inwin object.
 *
 * @param parent The parent object. It must be main window object.
 * @param group Current style and group already loaded into groupspace.
 * @return Pointer to inwin object, which contain some program properties and
 * list of programs.
 *
 * @ingroup ProgramEditor
 */
Evas_Object *
program_editor_window_add(Evas_Object *parent, Group *group);

#endif /* PROGRAM_EDITOR_H */
