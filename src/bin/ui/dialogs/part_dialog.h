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

#ifndef PART_DIALOG_H
#define PART_DIALOG_H

/**
 * @defgroup New_Part_Dialog New_Part_Dialog
 * @ingroup Dialogs
 *
 * Show dialog window for choise part type.
 */

#include "eflete.h"
#include "main_window.h"
#include "widget_manager.h"
#include "modal_window.h"
#include "widget_macro.h"
#include "image_editor.h"

/**
 * Add new dialog for choise part type add inwin object.
 *
 * @param parent The parent object. Its must be layout of main window
 * object.
 * @param groupspace The groupspace layout pointer.
 * @return Pointer to inwin object.
 *
 * @ingroup New_Part_Dialog
 */
Evas_Object *
part_dialog_add(void);

#endif /* PART_DIALOG_H */
