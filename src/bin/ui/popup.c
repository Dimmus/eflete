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

#include "main_window.h"

static Popup_Button btn_pressed;
static Evas_Object *popup;
static Evas_Object *fs;
static Evas_Smart_Cb dismiss_func;
static void* func_data;

static const Popup_Button _btn_ok         = BTN_OK;
static const Popup_Button _btn_save       = BTN_SAVE;
static const Popup_Button _btn_replace    = BTN_REPLACE;
static const Popup_Button _btn_dont_save  = BTN_DONT_SAVE;
static const Popup_Button _btn_cancel     = BTN_CANCEL;
static Popup_Validator_Func validator     = NULL;
static void *user_data                    = NULL;

static void
_btn_cb(void *data,
        Evas_Object *obj __UNUSED__,
        void *ei __UNUSED__)
{
   btn_pressed = *((Popup_Button *)data);
   if ((BTN_OK == btn_pressed) || (BTN_SAVE == btn_pressed) || (BTN_REPLACE == btn_pressed))
     if (validator && (!validator(user_data))) return;
   eflete_main_loop_quit();
}

#define BTN_ADD(TEXT, PLACE, DATA) \
{ \
   BUTTON_ADD(popup, btn, TEXT); \
   evas_object_smart_callback_add(btn, "clicked", _btn_cb, DATA); \
   elm_object_part_content_set(popup, PLACE, btn); \
}

Popup_Button
popup_want_action(const char *title,
                  const char *msg,
                  Evas_Object *content,
                  Evas_Object *to_focus,
                  Popup_Button popup_btns,
                  Popup_Validator_Func func,
                  void *data)

{
   Evas_Object *btn;

   /* only one content will be setted to popup: or message, or used content */
   assert((msg != NULL) != (content != NULL));
   validator = func;
   user_data = data;

   ui_menu_items_list_disable_set(ap.menu, MENU_ITEMS_LIST_MAIN, true);

   popup = elm_popup_add(ap.win);
   elm_object_part_text_set(popup, "title,text", title);
   elm_popup_content_text_wrap_type_set(popup, ELM_WRAP_WORD);
   if (msg) elm_object_text_set(popup, msg);
   if (content) elm_object_content_set(popup, content);

   if (popup_btns & BTN_OK)
     BTN_ADD(_("Ok"), "button1", &_btn_ok)

   if (popup_btns & BTN_SAVE)
     BTN_ADD(_("Save"), "button1", &_btn_save)

   if (popup_btns & BTN_REPLACE)
     BTN_ADD(_("Replace"), "button1", &_btn_replace)

   if (popup_btns & BTN_DONT_SAVE)
     BTN_ADD(_("Don't save"), "button2", &_btn_dont_save)

   if ((popup_btns & BTN_CANCEL) && (popup_btns & BTN_DONT_SAVE))
     BTN_ADD(_("Cancel"), "button3", &_btn_cancel)
   else
     BTN_ADD(_("Cancel"), "button2", &_btn_cancel)

   evas_object_show(popup);
   if (to_focus) elm_object_focus_set(to_focus, true);
   eflete_main_loop_begin();

   /* clear up before return the presed button */
   elm_object_content_unset(popup);
   evas_object_del(popup);
   popup = NULL;
   ui_menu_items_list_disable_set(ap.menu, MENU_ITEMS_LIST_MAIN, false);

   validator = NULL;
   user_data = NULL;

   return btn_pressed;
}
#undef BTN_ADD

void
popup_buttons_disabled_set(Popup_Button popup_btns, Eina_Bool disabled)
{
   if ((popup_btns & BTN_OK) || (popup_btns & BTN_SAVE) || (popup_btns & BTN_REPLACE))
     elm_object_disabled_set(elm_object_part_content_get(popup, "button1"), disabled);
   if (popup_btns & BTN_DONT_SAVE)
     elm_object_disabled_set(elm_object_part_content_get(popup, "button2"), disabled);
   if ((popup_btns & BTN_CANCEL) && (popup_btns & BTN_DONT_SAVE))
     elm_object_disabled_set(elm_object_part_content_get(popup, "button3"), disabled);
   if (popup_btns & BTN_CANCEL)
     elm_object_disabled_set(elm_object_part_content_get(popup, "button2"), disabled);
}

static void
_popup_dismiss(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               const char *signal __UNUSED__,
               const char *source __UNUSED__)
{
   evas_object_del(popup);
}

static void
_done(void *data __UNUSED__,
      Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   Eina_List *selected_paths = NULL;
   Eina_Stringshare *selected;

   if (dismiss_func)
     {
        if (elm_fileselector_multi_select_get(fs))
          selected_paths = (Eina_List *)elm_fileselector_selected_paths_get(fs);
        else
          {
             selected = elm_fileselector_selected_get(fs);
             selected_paths = eina_list_append(selected_paths, selected);
          }
        ((Evas_Smart_Cb)dismiss_func)(func_data, NULL, selected_paths);
     }
   eina_list_free(selected_paths);
   dismiss_func = NULL;
   func_data = NULL;
   _popup_dismiss(NULL, NULL, data, NULL);
}

#define FS_W 430
#define FS_H 460

static void
_popup_obj_follow(void *data __UNUSED__,
                  Evas *e __UNUSED__,
                  Evas_Object *obj,
                  void *event_info __UNUSED__)
{
   int x, y, w, h, nx, ny;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   nx = x - (FS_W + 12 - w);
   ny = y + h;
   evas_object_move(popup, nx, ny);
}

static void
_popup_win_follow(void *data __UNUSED__,
                  Evas *e __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   int w, h, nx, ny;

   evas_object_geometry_get(ap.win, NULL, NULL, &w, &h);
   fprintf(stdout, "%d %d\n", w, h);
   nx = (w / 2) - ((FS_W + 12) / 2);
   ny = (h / 2) - ((FS_H + 12) / 2);
   evas_object_move(popup, nx, ny);
}

static void
_fileselector_helper(const char *title,
                     Evas_Object *follow_up,
                     const char *path,
                     Evas_Smart_Cb func,
                     void *data,
                     Elm_Fileselector_Filter_Func filter_cb)
{
   dismiss_func = func;
   func_data = data;

   popup = elm_layout_add(ap.win);
   elm_layout_theme_set(popup, "layout", "popup", title ? "hint_title" : "hint");
   elm_layout_signal_callback_add(popup, "hint,dismiss", "eflete", _popup_dismiss, NULL);

   fs = elm_fileselector_add(ap.win);
   elm_fileselector_expandable_set(fs, false);
   if (filter_cb)
     {
        elm_fileselector_custom_filter_append(fs, filter_cb, NULL, "edj");
        evas_object_hide(elm_layout_content_unset(fs, "elm.swallow.filters"));
     }
   else elm_fileselector_folder_only_set(fs, true);

   elm_fileselector_path_set(fs, path ? path : getenv("HOME"));
   evas_object_smart_callback_add(fs, "done", _done, NULL);
   evas_object_smart_callback_add(fs, "activated", _done, NULL);
   /* small hack, hide not necessary button */
   evas_object_hide(elm_layout_content_unset(fs, "elm.swallow.cancel"));
   /* one more hack, set text our text to button 'ok' */
   elm_object_text_set(elm_layout_content_get(fs, "elm.swallow.ok"), _("Open"));
   evas_object_size_hint_min_set(fs, FS_W, FS_H);
   evas_object_resize(fs, FS_W, FS_H);

   if (title) elm_object_text_set(popup, title);
   elm_layout_content_set(popup, "elm.swallow.content", fs);
   if (follow_up)
     {
        _popup_obj_follow(NULL, NULL, follow_up, NULL);
        evas_object_event_callback_add(follow_up, EVAS_CALLBACK_RESIZE, _popup_obj_follow, NULL);
        evas_object_event_callback_add(follow_up, EVAS_CALLBACK_MOVE, _popup_obj_follow, NULL);
     }
   else
     {
        _popup_win_follow(NULL, NULL, NULL, NULL);
        evas_object_event_callback_add(ap.win, EVAS_CALLBACK_RESIZE, _popup_win_follow, NULL);
     }
   evas_object_show(popup);
}

void
popup_fileselector_folder_helper(Evas_Object *follow_up, const char *path,
                                 Evas_Smart_Cb func, void *data)
{
   _fileselector_helper(NULL, follow_up, path, func, data, NULL);
}

static Eina_Bool
_edj_filter(const char *path,
            Eina_Bool dir,
            void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, ".edj"))
     return true;
   return false;
}

void
popup_fileselector_edj_helper(const char *title, Evas_Object *follow_up, const char *path,
                              Evas_Smart_Cb func, void *data)
{
   _fileselector_helper(title, follow_up, path, func, data, _edj_filter);
}

static Eina_Bool
_edc_filter(const char *path,
            Eina_Bool dir,
            void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, ".edc"))
     return true;
   return false;
}

void
popup_fileselector_edc_helper(Evas_Object *follow_up, const char *path,
                              Evas_Smart_Cb func, void *data)
{
   _fileselector_helper(NULL, follow_up, path, func, data, _edc_filter);
}

void
popup_log_message_helper(const char *msg)
{
   Evas_Object *box, *en, *lab;

   popup = elm_layout_add(ap.win);
   elm_layout_theme_set(popup, "layout", "popup", "hint");
   elm_layout_signal_callback_add(popup, "hint,dismiss", "eflete", _popup_dismiss, NULL);

   BOX_ADD(popup, box, false, false)
   elm_box_padding_set(box, 0, 6);
   LABEL_ADD(popup, lab, _("<font_size=14>Import edc-file error"))
   evas_object_show(lab);
   ENTRY_ADD(box, en, false)
   elm_entry_editable_set(en, false);
   elm_box_pack_end(box, lab);
   elm_box_pack_end(box, en);
   elm_layout_content_set(popup, NULL, box);

   elm_entry_entry_set(en, msg);

   _popup_win_follow(NULL, NULL, NULL, NULL);
   evas_object_event_callback_add(ap.win, EVAS_CALLBACK_RESIZE, _popup_win_follow, NULL);
   evas_object_show(popup);
}

#undef FS_W
#undef FS_H
