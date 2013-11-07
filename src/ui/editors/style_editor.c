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

#include "style_editor.h"

#define FONT_SIZE "24"

struct _Style_Editor
{
   Project *pr;
   Evas_Object *mwin;
   Evas_Object *glist;
   Evas_Object *entry_style;
   Evas_Object *entry_tag;
   Evas_Object *entry_prop;
   Evas_Object *entry_prev;
};

typedef struct _Style_Editor Style_Editor;

struct _Style_entries
{
   Evas_Object *style_name;
   Evas_Object *default_tags;
};

typedef struct _Style_entries Style_entries;

struct _Style_tag_entries
{
   Elm_Object_Item *style_name;
   Evas_Object *tag_name;
   Evas_Object *tag_value;
};

typedef struct _Style_tag_entries Style_tag_entries;

static Style_Editor window;
static Style_entries st_entries;
static Style_tag_entries st_tag_entries;
static Elm_Genlist_Item_Class *_itc_style = NULL;
static Elm_Genlist_Item_Class *_itc_tags = NULL;

static void
_on_glit_selected(void *data, Evas_Object *obj, void *event_info);

static void
_on_popup_bt_cancel(void *data,
                    Evas_Object *obj __UNUSED__,
                    void *ei __UNUSED__)
{
   evas_object_del(data);
}

static void
_on_st_add_bt_ok(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *ei __UNUSED__)
{
   Evas_Object *popup = (Evas_Object *)data;
   const char *style_name = elm_entry_entry_get(st_entries.style_name);
   const char *default_tags = elm_entry_entry_get(st_entries.default_tags);
   Elm_Object_Item *glit_style;

   if ((!style_name) || (strcmp(style_name, "") == 0))
     {
        NOTIFY_WARNING("Style name can not be empty!");
        return;
     }
   if (!((isalpha(default_tags[0])) ||
         (default_tags[0] == '+') ||
         (!strcmp(default_tags, ""))))
     {
        NOTIFY_WARNING("The default tag must begin from + or alphabetic symbol");
        return;
     }
   if(!style_edit_style_add(window.pr, style_name))
     {
        NOTIFY_WARNING("Style name must be unique!");
        return;
     }
   style_edit_style_tag_add(window.pr, style_name, "DEFAULT", default_tags);

   glit_style = elm_genlist_item_append(window.glist, _itc_style,
                                        style_name,
                                        NULL, ELM_GENLIST_ITEM_NONE,
                                        _on_glit_selected, style_name);
   elm_genlist_item_append(window.glist, _itc_tags,
                           "DEFAULT",
                           glit_style, ELM_GENLIST_ITEM_NONE,
                           _on_glit_selected, "DEFAULT");
   evas_object_del(popup);
   elm_genlist_item_selected_set(glit_style, EINA_TRUE);
   elm_genlist_item_bring_in(glit_style, ELM_GENLIST_ITEM_SCROLLTO_IN);
   elm_genlist_item_show(glit_style, ELM_GENLIST_ITEM_SCROLLTO_IN);
}

static void
_on_tag_add_bt_ok(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *ei __UNUSED__)
{
   Evas_Object *popup = (Evas_Object *)data;
   Elm_Object_Item *glit_tag;
   const char *style_name = elm_object_item_data_get(st_tag_entries.style_name);
   const char *tag_name = elm_entry_entry_get(st_tag_entries.tag_name);
   const char *tag_value = elm_entry_entry_get(st_tag_entries.tag_value);

   if ((!tag_name) || (strcmp(tag_name, "") == 0))
     {
        NOTIFY_WARNING("Tag name can not be empty!");
        return;
     }
   if ((!tag_value) || (strcmp(tag_value, "") == 0))
     {
        NOTIFY_WARNING("Tag value can not be empty!");
        return;
     }
   if (!((isalpha(tag_value[0])) ||
        (tag_value[0] == '+')))
    {
        NOTIFY_WARNING("Tag value must begin from + or alphabetic symbol");
        return;
    }
   if (!style_edit_style_tag_add(window.pr, style_name, tag_name, tag_value))
     {
        NOTIFY_WARNING("Tag name must be unique!");
        return;
     }
   glit_tag = elm_genlist_item_append(window.glist, _itc_tags,
                                      tag_name,
                                      st_tag_entries.style_name, ELM_GENLIST_ITEM_NONE,
                                      _on_glit_selected, tag_name);
   evas_object_del(popup);
   elm_genlist_item_selected_set(glit_tag, EINA_TRUE);
   elm_genlist_item_show(st_tag_entries.style_name, ELM_GENLIST_ITEM_SCROLLTO_IN);
   elm_genlist_item_bring_in(glit_tag, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
}

static void
_on_bt_style_add(void *data __UNUSED__,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Evas_Object *popup, *box, *bt_ok, *bt_cancel;
   Evas_Object *st_box, *st_label, *st_entry;
   Evas_Object *tag_box, *tag_label, *tag_entry;
   static Elm_Entry_Filter_Accept_Set accept_name;

   popup = elm_popup_add(window.mwin);
   elm_object_style_set(popup, "eflete");
   elm_object_part_text_set(popup, "title,text", "Add textblock style");

   box = elm_box_add(popup);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

   st_box = elm_box_add(box);
   elm_box_horizontal_set(st_box, EINA_TRUE);
   evas_object_size_hint_weight_set(st_box, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(st_box, EVAS_HINT_FILL, 0.0);
   evas_object_show(st_box);

   LABEL_ADD(st_box, st_label, "Style name: ")
   elm_box_pack_end(st_box, st_label);

   ENTRY_ADD(st_box, st_entry, EINA_TRUE, DEFAULT_STYLE);
   accept_name.accepted = NULL;
   accept_name.rejected = ":;'\"!?&^%$#@()[]=+*{} ";
   elm_entry_markup_filter_append(st_entry, elm_entry_filter_accept_set, &accept_name);
   elm_box_pack_end(st_box, st_entry);

   tag_box = elm_box_add(box);
   elm_box_horizontal_set(tag_box, EINA_TRUE);
   evas_object_size_hint_weight_set(tag_box, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(tag_box, EVAS_HINT_FILL, 0.0);

   LABEL_ADD(tag_box, tag_label, "Default tags: ")
   elm_box_pack_end(tag_box, tag_label);

   ENTRY_ADD(tag_box, tag_entry, EINA_TRUE, DEFAULT_STYLE);
   elm_box_pack_end(tag_box, tag_entry);
   evas_object_show(tag_box);

   elm_box_pack_end(box, st_box);
   elm_box_pack_end(box, tag_box);
   elm_object_content_set(popup, box);
   evas_object_show(box);

   BUTTON_ADD(popup, bt_ok, "Ok");
   evas_object_smart_callback_add(bt_ok, "clicked", _on_st_add_bt_ok, popup);
   elm_object_part_content_set(popup, "button1", bt_ok);

   BUTTON_ADD(popup, bt_cancel, "Cancel");
   evas_object_smart_callback_add(bt_cancel, "clicked", _on_popup_bt_cancel, popup);
   elm_object_part_content_set(popup, "button2", bt_cancel);

   st_entries.style_name = st_entry;
   st_entries.default_tags = tag_entry;

   evas_object_show(popup);
}

static void
_on_bt_tag_add(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   Evas_Object *popup, *box, *bt_ok, *bt_cancel, *style_label;
   Evas_Object *tag_box, *tag_label, *tag_entry;
   Evas_Object *value_box, *value_label, *value_entry;
   Elm_Object_Item *glit = elm_genlist_selected_item_get(window.glist);
   Elm_Object_Item *glit_parent = elm_genlist_item_parent_get(glit);
   const char *style_name;

   if (!glit)
     {
         NOTIFY_WARNING("Select a style!");
         return;
     }

   if (!glit_parent)
     {
         style_name = elm_object_item_data_get(glit);
         st_tag_entries.style_name = glit;
     }
   else
     {
         style_name = elm_object_item_data_get(glit_parent);
         st_tag_entries.style_name = glit_parent;
     }

   popup = elm_popup_add(window.mwin);
   elm_object_style_set(popup, "eflete");
   elm_object_part_text_set(popup, "title,text", "Add tag to style");

   box = elm_box_add(popup);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

   LABEL_ADD(box, style_label, style_name)
   elm_box_pack_end(box, style_label);

   tag_box = elm_box_add(box);
   elm_box_horizontal_set(tag_box, EINA_TRUE);
   evas_object_size_hint_weight_set(tag_box, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(tag_box, EVAS_HINT_FILL, 0.0);

   LABEL_ADD(tag_box, tag_label, "Tag name: ")
   elm_box_pack_end(tag_box, tag_label);

   ENTRY_ADD(tag_box, tag_entry, EINA_TRUE, DEFAULT_STYLE);
   elm_box_pack_end(tag_box, tag_entry);
   evas_object_show(tag_box);

   value_box = elm_box_add(box);
   elm_box_horizontal_set(value_box, EINA_TRUE);
   evas_object_size_hint_weight_set(value_box, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(value_box, EVAS_HINT_FILL, 0.0);
   evas_object_show(value_box);

   LABEL_ADD(value_box, value_label, "Tag value: ")
   elm_box_pack_end(value_box, value_label);

   ENTRY_ADD(value_box, value_entry, EINA_TRUE, DEFAULT_STYLE);
   elm_box_pack_end(value_box, value_entry);

   elm_box_pack_end(box, tag_box);
   elm_box_pack_end(box, value_box);
   elm_object_content_set(popup, box);
   evas_object_show(box);

   BUTTON_ADD(popup, bt_ok, "Ok");
   evas_object_smart_callback_add(bt_ok, "clicked", _on_tag_add_bt_ok, popup);
   elm_object_part_content_set(popup, "button1", bt_ok);

   BUTTON_ADD(popup, bt_cancel, "Cancel");
   evas_object_smart_callback_add(bt_cancel, "clicked", _on_popup_bt_cancel, popup);
   elm_object_part_content_set(popup, "button2", bt_cancel);

   st_tag_entries.tag_name = tag_entry;
   st_tag_entries.tag_value = value_entry;

   evas_object_show(popup);
}


static void
_on_bt_del(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info __UNUSED__)
{
   Elm_Object_Item *glit = elm_genlist_selected_item_get(window.glist);
   Elm_Object_Item *glit_parent = elm_genlist_item_parent_get(glit);
   const char *style_name, *tag;

   if (!glit_parent)
     {
        style_name = elm_object_item_part_text_get(glit, "elm.text");
        style_edit_style_del(window.pr, style_name);
     }
   else
     {
        style_name = elm_object_item_part_text_get(glit_parent, "elm.text");
        tag = elm_object_item_part_text_get(glit, "elm.text");
        style_edit_style_tag_del(window.pr, style_name, tag);
     }
   elm_object_item_del(glit);
}

/**
 * For GenList, getting the content for showing. Tag Names.
 */
static char *
_item_tags_label_get(void *data,
                     Evas_Object *obj __UNUSED__,
                     const char *part __UNUSED__)
{
   char *tag_label = (char *)data;
   if (!tag_label)
     {
        ERR("It impossible, but it is occurred, tag's name is missing!");
        return NULL;
     }
   return strdup(tag_label);
}

/**
 * For Style names.
 */
static char *
_item_style_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   char *style_label = (char *)data;
   /* char *formated_style_name; */
   if (!style_label)
     {
        ERR("It impossible, but it is occurred, style's name is missing!");
        return NULL;
     }
   /* 6 - <bold> */
   /*
   formated_style_name = mem_malloc(7 + strlen(style_label));
   sprintf(formated_style_name, "%s%s", "<bold>", style_label);
   return formated_style_name;
   */
   return strdup(style_label);
}

/**
 * For showing tags value and name in appropriate entry.
 */
static void
_on_glit_selected(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   Eina_List *tags, *l;
   const char *style_name;
   const char *tag, *value;
   char style[BUFF_MAX] = "DEFAULT=' font_size="FONT_SIZE;
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;
   Elm_Object_Item *glit_parent = elm_genlist_item_parent_get(glit);

   if (!glit_parent)
     {
        style_name = (char *)(data);
        tags = style_edit_style_tags_list_get(window.pr, style_name);

        EINA_LIST_FOREACH(tags, l, tag)
          {
             value = style_edit_style_tag_value_get(window.pr, style_name, tag);
             eina_strlcat(style, value, BUFF_MAX);
          }
        elm_entry_entry_set(window.entry_tag, "");
        elm_entry_entry_set(window.entry_prop, "");
     }
   else
     {
        style_name = elm_object_item_data_get(glit_parent);
        tag = (char *)(data);
        value = style_edit_style_tag_value_get(window.pr, style_name, tag);
        elm_entry_entry_set(window.entry_tag, tag);
        elm_entry_entry_set(window.entry_prop, value);
        eina_strlcat(style, value, BUFF_MAX);
     }
   eina_strlcat(style, "'", BUFF_MAX);
   evas_object_show(window.entry_style);
   elm_entry_text_style_user_push(window.entry_style, style);
   evas_object_size_hint_max_set(window.entry_style, EVAS_HINT_FILL, EVAS_HINT_FILL);
}

static void
_on_viewer_exit(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   Evas_Object *mwin = (Evas_Object *) data;
   evas_object_del(mwin);
}

/**
 * On hide we need to free everything!!!
 */
static void
_item_style_del(void *data __UNUSED__,
                Evas_Object *obj __UNUSED__)
{
   /*
   Text_Styles *_style = (Text_Styles *) data;
   eina_stringshare_del(_style->style_name);
   */
}

/**
 * On hide we need to free everything!!!
 */
static void
_item_tags_del(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__)
{
   /*
   Tag *_tag = (Tag *) data;
   eina_stringshare_del(_tag->tag_name);
   eina_stringshare_del(_tag->tag_value);
   */
}

/**
 * Creating the view of the mwin!!!
 */
Evas_Object*
_form_left_side(Evas_Object *obj)
{
     Elm_Object_Item *glit_style, *glit_tag;
     Evas_Object *box, *btn;
     Eina_List *styles, *tags, *l_st, *l_tg;
     char *style, *tag;

     if (!_itc_style)
       {
          _itc_style = elm_genlist_item_class_new();
          _itc_style->item_style = DEFAULT_STYLE;
          _itc_style->func.text_get = _item_style_label_get;
          _itc_style->func.content_get = NULL;
          _itc_style->func.state_get = NULL;
          _itc_style->func.del = _item_style_del;
       }
     if (!_itc_tags)
       {
          _itc_tags= elm_genlist_item_class_new();
          _itc_tags->item_style = DEFAULT_STYLE;
          _itc_tags->func.text_get = _item_tags_label_get;
          _itc_tags->func.content_get = NULL;
          _itc_tags->func.state_get = NULL;
          _itc_tags->func.del = _item_tags_del;
       }

     box = elm_box_add(obj);
     evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
     evas_object_show(box);

     window.glist = elm_genlist_add(box);
     elm_object_style_set(window.glist, DEFAULT_STYLE);
     elm_box_pack_end(box, window.glist);
     evas_object_show(window.glist);

     evas_object_size_hint_align_set(window.glist,
                                     EVAS_HINT_FILL,
                                     EVAS_HINT_FILL);
     evas_object_size_hint_weight_set(window.glist,
                                      EVAS_HINT_EXPAND,
                                      EVAS_HINT_EXPAND);

     styles = style_edit_styles_list_get(window.pr);

     EINA_LIST_FOREACH(styles, l_st, style)
       {
          glit_style = elm_genlist_item_append(window.glist, _itc_style,
                                               style,
                                               NULL, ELM_GENLIST_ITEM_NONE,
                                               _on_glit_selected, style);
          elm_object_item_data_set(glit_style, style);
          tags = style_edit_style_tags_list_get(window.pr, style);
          EINA_LIST_FOREACH(tags, l_tg, tag)
            {
               glit_tag = elm_genlist_item_append(window.glist, _itc_tags,
                                       tag,
                                       glit_style, ELM_GENLIST_ITEM_NONE,
                                      _on_glit_selected, tag);
                elm_object_item_data_set(glit_tag, tag);
            }
       }
     eina_list_free(styles);

     BUTTON_ADD(obj, btn, "New style");
     evas_object_size_hint_weight_set(btn, 0.0, 0.0);
     evas_object_smart_callback_add(btn, "clicked", _on_bt_style_add, NULL);
     elm_box_pack_end(box, btn);

     BUTTON_ADD(obj, btn, "New tag");
     evas_object_size_hint_weight_set(btn, 0.0, 0.0);
     evas_object_smart_callback_add(btn, "clicked", _on_bt_tag_add, NULL);
     elm_box_pack_end(box, btn);

     BUTTON_ADD(obj, btn, "Delete");
     evas_object_size_hint_weight_set(btn, 0.0, 0.0);
     evas_object_smart_callback_add(btn, "clicked", _on_bt_del, NULL);
     elm_box_pack_end(box, btn);

     return box;
}
static void
_change_bg_cb(void *data __UNUSED__,
              Evas_Object *obj,
              void *event_info __UNUSED__)
{
   int state = elm_radio_state_value_get(obj);
   Evas_Object *bg = NULL;
   Evas *canvas = evas_object_evas_get(obj);
   switch (state)
     {
      case 0:
         bg = evas_object_image_filled_add(canvas);
         evas_object_image_filled_set(bg, EINA_FALSE);
         evas_object_image_file_set(bg, TET_IMG_PATH"bg_demo.png", NULL);
         evas_object_image_fill_set(bg, 0, 0, 16, 16);
      break;
      case 1:
         bg = evas_object_rectangle_add(canvas);
         evas_object_color_set(bg, 0, 0, 0, 255);
         evas_object_show(bg);
      break;
      case 2:
         bg = evas_object_rectangle_add(canvas);
         evas_object_color_set(bg, 255, 255, 255, 255);
         evas_object_show(bg);
      break;
      default:
      break;
     }
   elm_object_part_content_set(window.entry_prev, "background", bg);
}

Evas_Object*
_form_right_side(Evas_Object *obj)
{
   Evas_Object *layout, *btn;
   Evas_Object *box_bg = NULL;
   Evas_Object *image_bg = NULL;
   Evas_Object *radio_group = NULL;
   Evas_Object *radio = NULL;

   layout = elm_layout_add(obj);
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_layout_file_set(layout, TET_EDJ, "ui/style_viewer_window/property");
   evas_object_show(layout);

   ENTRY_ADD(obj, window.entry_tag, EINA_TRUE, DEFAULT_STYLE);
   elm_object_part_content_set (layout, "swallow/tag_entry", window.entry_tag);

   ENTRY_ADD(obj, window.entry_prop, EINA_TRUE, DEFAULT_STYLE);
   elm_object_part_content_set (layout, "swallow/prop_entry", window.entry_prop);

   BUTTON_ADD(obj, btn, "Close viewer");
   evas_object_smart_callback_add(btn, "clicked", _on_viewer_exit, window.mwin);
   elm_object_part_content_set (layout, "swallow/button_close", btn);
   evas_object_show(btn);

   BOX_ADD(obj, box_bg, EINA_TRUE, EINA_FALSE);
   elm_box_padding_set(box_bg, 10, 0);

#define _RADIO_ADD(radio, value, image) \
   radio = elm_radio_add(obj); \
   elm_object_style_set(radio, "eflete/style_editor"); \
   elm_radio_state_value_set(radio, value); \
   evas_object_show(radio); \
   IMAGE_ADD(box_bg, image_bg, image); \
   elm_image_resizable_set(image_bg, EINA_FALSE, EINA_FALSE); \
   elm_object_part_content_set(radio, "bg", image_bg); \
   evas_object_smart_callback_add(radio, "changed", _change_bg_cb, NULL); \
   elm_box_pack_end(box_bg, radio);

   _RADIO_ADD(radio_group, 0, TET_IMG_PATH"styles-preview-bg-transparent.png");
   _RADIO_ADD(radio, 1, TET_IMG_PATH"styles-preview-bg-black.png");
   elm_radio_group_add(radio, radio_group);
   _RADIO_ADD(radio, 2, TET_IMG_PATH"styles-preview-bg-white.png");
   elm_radio_group_add(radio, radio_group);
#undef _RADIO_ADD
   elm_object_part_content_set(layout, "menu_container", box_bg);
   return layout;
}
static void
__on_style_editor_close(void *data __UNUSED__,
                        Evas *e __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info __UNUSED__)
{
   style_edit_save(window.pr);
   /* clear stringshare */
}

Evas_Object *
style_editor_window_add(Evas_Object *parent, Project *project)
{
   if ((!parent) || (!project))
     {
        ERR("NULL param passed to style_editor_window_add");
        return NULL;
     }
   Evas_Object *panes, *panes_h;
   Evas_Object *layout_left, *layout_right;
   Evas_Object *bg = NULL;
   Evas *canvas = evas_object_evas_get(parent);

   window.pr = project;
   window.mwin = mw_add(parent);
   mw_title_set(window.mwin, "Textblock style editor");
   evas_object_event_callback_add(window.mwin, EVAS_CALLBACK_FREE,
                                  __on_style_editor_close, NULL);
   panes = elm_panes_add(window.mwin);
   elm_object_style_set(panes, DEFAULT_STYLE);
   evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_panes_content_left_size_set(panes, 0.2);
   elm_win_inwin_content_set(window.mwin, panes);
   evas_object_show(panes);

   layout_left = _form_left_side(window.mwin);
   elm_object_part_content_set(panes, "left", layout_left);
   evas_object_show(layout_left);

   panes_h = elm_panes_add(window.mwin);
   elm_object_style_set(panes_h, DEFAULT_STYLE);
   evas_object_size_hint_weight_set(panes_h, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panes_h, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_panes_horizontal_set(panes_h, EINA_TRUE);
   elm_object_part_content_set(panes, "right", panes_h);
   evas_object_show(panes_h);

   Evas_Object *layout_preview = NULL;
   layout_preview = elm_layout_add(window.mwin);
   evas_object_size_hint_weight_set(layout_preview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_layout_file_set(layout_preview, TET_EDJ, "ui/style_viewer_window/preview");
   evas_object_show(layout_preview);
   elm_object_part_content_set(panes_h, "left", layout_preview);
   window.entry_prev = layout_preview;

   bg = evas_object_image_filled_add(canvas);
   evas_object_image_filled_set(bg, EINA_FALSE);
   evas_object_image_file_set(bg, TET_IMG_PATH"bg_demo.png", NULL);
   evas_object_image_fill_set(bg, 0, 0, 16, 16);
   elm_object_part_content_set(layout_preview, "background", bg);
   evas_object_show(bg);

   ENTRY_ADD(window.mwin, window.entry_style, EINA_TRUE, "style_editor");
   elm_entry_editable_set(window.entry_style, EINA_FALSE);
   evas_object_size_hint_weight_set(window.entry_style, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_object_part_content_set(layout_preview, "entry", window.entry_style);
   elm_object_text_set(window.entry_style, "The quick brown fox jumps over the lazy dog");
   elm_entry_text_style_user_push(window.entry_style, "DEFAULT='align=center "
                                  "font_size="FONT_SIZE" font_weight=Bold'");

   evas_object_hide(window.entry_style);
   layout_right = _form_right_side(window.mwin);
   elm_object_part_content_set(panes_h, "right", layout_right);
   evas_object_show(layout_right);

   evas_object_show(window.mwin);

   return window.mwin;
}

#undef FONT_SIZE
