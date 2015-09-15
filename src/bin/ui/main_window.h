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

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

/**
 * @defgroup Window Window
 * @ingroup EFLETE
 *
 * Main window header provide API to create and manipulate main window object.
 * Create panes markup for differents block's, like workspace, widget list,
 * state list, etc...
 */

#include "eflete.h"
#include "enventor_module.h"
#include "config.h"
#include "common_macro.h"
#include "ui_widget_list.h"
#include "ui_signal_list.h"
#include "ui_block.h"
#include "notify.h"
#include "string_common.h"

#include "part_dialog.h"
#include "style_dialog.h"
#include "item_dialog.h"
#include "colorsel.h"
#include "colorclass_manager.h"

enum Menu_Item
{
   /* Needed as parent for top level menu items and as end mark in items lists */
   MENU_NULL,

   MENU_FILE,
      MENU_FILE_NEW_PROJECT,
      MENU_FILE_OPEN_PROJECT,
      MENU_FILE_IMPORT_EDJ,
      MENU_FILE_IMPORT_EDC,
      MENU_FILE_SAVE,
      MENU_FILE_EXPORT_EDC,
         MENU_FILE_EXPORT_EDC_PROJECT,
         MENU_FILE_EXPORT_EDC_GROUP,
      MENU_FILE_EXPORT,
         MENU_FILE_EXPORT_DEVELOP,
         MENU_FILE_EXPORT_RELEASE,
      MENU_FILE_CLOSE_PROJECT,
      MENU_FILE_EXIT,
   MENU_EDIT,
      MENU_EDIT_PREFERENCE,
   MENU_VIEW,
      MENU_VIEW_WORKSPACE,
         MENU_VIEW_WORKSPACE_ZOOM_IN,
         MENU_VIEW_WORKSPACE_ZOOM_OUT,
         MENU_VIEW_WORKSPACE_SEPARATE,
         MENU_VIEW_WORKSPACE_OBJECT_AREA,
      MENU_VIEW_RULERS,
         MENU_VIEW_RULERS_SHOW,
         MENU_VIEW_RULERS_ABS,
         MENU_VIEW_RULERS_REL,
         MENU_VIEW_RULERS_BOTH,
   MENU_EDITORS,
      MENU_EDITORS_ANIMATOR,
      MENU_EDITORS_IMAGE,
      MENU_EDITORS_SOUND,
      MENU_EDITORS_COLORCLASS,
      MENU_EDITORS_TEXT_STYLE,
   MENU_HELP,
      MENU_HELP_ABOUT,

   /* Needed for menu array init */
   MENU_ITEMS_COUNT
};

extern int MENU_ITEMS_LIST_BASE[];
extern int MENU_ITEMS_LIST_STYLE_ONLY[];
extern int MENU_ITEMS_LIST_MAIN[];
extern int MENU_ITEMS_LIST_TEMPORARY[];

#define POPUP_BUTTON_MASK 0xf
typedef enum _Popup_Button
{
   BTN_NONE       = 0,
   BTN_OK         = (1 << 0),
   BTN_CANCEL     = (1 << 1),
   BTN_SAVE       = (1 << 2),
   BTN_DONT_SAVE  = (1 << 3),
   BTN_REPLACE    = (1 << 4)
} Popup_Button;

/**
 * Adds toolbar with menu and buttons to the given Elementary layout.
 *
 * @return menu Menu object if successful, or NULL elthewhere.
 *
 * @ingroup Window
 */
Menu *
ui_menu_add(void);

/**
 * Disable or enable menu item
 *
 * If flag is EINA_TRUE - an item will be disabled, othervise - enabled, so it
 * can be used, clicked or anything like that.
 *
 * @param menu object
 * @param mid Menu item's ID.
 * @param flag for disabling - EINA_TRUE, for enabling - EINA_FALSE.
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 */
Eina_Bool
ui_menu_disable_set(Menu *menu, int mid, Eina_Bool flag);

/**
 * Disable or enable list of menu items
 *
 * If flag is EINA_TRUE - an item will be disabled, othervise - enabled, so it
 * can be used, clicked or anything like that.
 *
 * @param menu object
 * @param list Array of menu items ID. Last item should always be MENU_NULL.
 * see default lists (MENU_ITEMS_LIST_BASE, MENU_ITEMS_LIST_STYLE_ONLY,
 * MENU_ITEMS_LIST_MAIN)
 * @param flag for disabling - EINA_TRUE, for enabling - EINA_FALSE.
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 */
Eina_Bool
ui_menu_items_list_disable_set(Menu *menu, int *list, Eina_Bool flag);

enum _Splash_Status
{
   SPLASH_NONE,
   SPLASH_SUCCESS,
   SPLASH_CANCEL,
   SPLASH_LAST
};
typedef enum _Splash_Status Splash_Status;

typedef Eina_Bool
(* Splash_Cb)(void *data, Splash_Status status);

/**
 * Adds main window object for Edje tool development.
 *
 * @return EINA_TRUE if window and subblocks created correct, EINA_FALSE if fail.
 *
 * @ingroup Window
 */
Eina_Bool
ui_main_window_add(void);

/**
 * Delete main window object for Edje tool development.
 *
 * @return EINA_TRUE if successful, or EINA_FALSE otherwise.
 *
 * @ingroup Window
 */
Eina_Bool
ui_main_window_del(void);

/**
 * Update displayed information about current part state. Update part object
 * on workspace with new state parameters.
 * Moved to own method for the separation of the interaction between the blocks.
 *
 * @param obj The Evas_Object pointer, wich pointed on states genlist.
 * @param state Name of the state to be set.
 *
 * @ingroup Window
 */
void
ui_state_select(Evas_Object *obj, Eina_Stringshare *state);

/**
 * Show information about properties of part. Highlight part object
 * on workspace.
 * Moved to own method for the separation of the interaction between the blocks.
 *
 * @param part The Part pointer.
 *
 * @return Evas_object pointer. States gen list object.
 * @ingroup Window
 */
Evas_Object *
ui_part_select(Part* part);

/**
 * Load project data to App_Data structure. Turn to work state for application.
 * Moved to own method for the separation of the interaction between the blocks.
 *
 * @param selected_file String with opened project file name.
 *
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 *
 * @ingroup Window
 */
Eina_Bool
ui_edj_load(const char *selected_file);

/**
 * Delete selected style/class/layout from current widget
 *
 * @param group_type type of group to be deleted.
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 *
 * @ingroup Window
 */
Eina_Bool
ui_group_delete(Type group_type);

/**
 * Open new theme from template file.
 *
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 *
 * @ingroup Window
 */
Eina_Bool
new_theme_create(void);

/**
 * Ask user if he wants to close project
 *
 * @param msg The explanation text that will be shown to user. NULL for default message.
 * @return EINA_TRUE if roject is saved or user wants to discard changes, EINA_FALSE otherwise.
 *
 * @ingroup Window
 */
Eina_Bool
ui_close_project_request(const char *msg);

/**
 * Get data of widget user currently works with.
 *
 * @return Widget data structure.
 */
Widget *
ui_widget_from_ap_get(void);

/**
 * Get data of class user currently works with.
 *
 * @return Class data structure.
 */
Class *
ui_class_from_ap_get(void);

/* FIXME: Add comments */
Eina_Bool
register_callbacks(void);

/**
 * Add callbacks to widget list. Callbacks are using next signals:
 * "wl,group,select"
 * "wl,part,select",
 * "wl,part,back",
 * "wl,group,back",
 *
 * @param wd_list A pointer to widget list object.
 * @return EINA_TRUE if succeed, EINA_FALSE otherwise.
 */
Eina_Bool
add_callbacks_wd(Evas_Object *wd_list);

/**
 * Switch code editing mode ON or OFF.
 * The Code Editing mode means that Workspace, States, History and Signals
 * blocks are hidden and only Widget List, Life Wiew and Code tab are available
 * for user.
 *
 * @param is_on value to toggle Code Editing mode ON/OFF.
 * @return EINA_TRUE if succeed, EINA_FALSE otherwise.
 *
 * @ingroup Window
 */
Eina_Bool
code_edit_mode_switch(Eina_Bool is_on);

/**
 * The splash window with animation and info line.
 *
 * @param parent The parent widget, MUST be a window;
 * @param setup Callback will be be called on splash window show;
 * @param teardown Callback will be called on splash delete event;
 * @param cancel Callback will be called on "Cancel" button click;
 * @param data User data.
 *
 * @return The splash window object.
 *
 * @ingroup Window
 */
Evas_Object *
splash_add(Evas_Object *parent,
           Splash_Cb setup,
           Splash_Cb teardown,
           Splash_Cb cancel,
           void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

/**
 * Delete the splash window. Before delete will be played the close animation.
 *
 * @param obj The splash object.
 *
 * @ingroup Window
 */
void
splash_del(Evas_Object *obj);

/**
 * Show the main layout blocks.
 *
 * @return EINA_TRUE on success, otherwise EINA_FALSE.
 *
 * @ingroup Window
 */
Eina_Bool
blocks_show(void);

/**
 * Open existing project.
 *
 * @ingroup Window
 */
void
project_open();

/**
 * Save opened project.
 *
 * @ingroup Window
 */
void
project_save();

/**
 * Requesting to change project (need to close it, to hide blocks, unset data,
 * etc).
 * This function will ask user what to do with opened project
 * (if it is changed).
 *
 * @return EINA_TRUE on success, otherwise EINA_FALSE.
 *
 * @ingroup Window
 */
Eina_Bool
project_close(void);

/**
 * Create and show popup with given title and content. Only one - Evas_Object or
 * text will be setted as content. And you can set define the buttons what you
 * want to use in the popup. Func have a blocked behavior, while the popup button
 * not clicked, popup blocked the code runing, used ecore main loop iteration.
 *
 * @param title The Popup title;
 * @param msg The Popup message, formated text;
 * @param content The user Evas_Object seted as content to popup;
 * @param p_btns The flags for set the popup buttons
 *
 * @ingroup Window
 */
Popup_Button
popup_want_action(const char *title,
                  const char *msg,
                  Evas_Object *content,
                  Popup_Button p_btns);

void
popup_fileselector_folder_helper(Evas_Object *entry, const char *path);

void
popup_fileselector_edj_helper(Evas_Object *entry, const char *path);

void
popup_fileselector_edc_helper(Evas_Object *entry, const char *path);

void
popup_log_message_helper(const char *msg);


/**
 * Export project as develop edj file.
 *
 * @ingroup Window
 */
void
project_export_develop(void);

/**
 * Update Statusbar field that contains time of last save of current
 * project file.
 *
 * @param is_autosave flag to inform if the function is called by
 *                    'autosave' functionality.
 * @ingroup Window
 */
void
save_time_info_update(Eina_Bool is_autosave);

/**
 * Call dialog window for request
 *
 * @param parent Parent object
 * @param msg Request message.
 * @return EINA_FALSE if user has rejected request or EINA_TRUE if user has accepted request
 *
 * @ingroup Window
 */
Eina_Bool
export_replace_request(Evas_Object *parent, const char *msg);

/**
 * Export project as edc.
 *
 * @ingroup Window
 */
void
project_export_edc_project(void);


/**
 * Export group as edc.
 *
 * @ingroup Window
 */
void
project_export_edc_group(void);


#endif /* UI_MAIN_WINDOW_H */
