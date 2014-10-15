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

#include "ui_property.h"
#include "ui_property_macro.h"
#include "common_macro.h"
#include "image_editor.h"

#define PROP_DATA "prop_data"

#define PROP_DATA_GET(ret) \
   Prop_Data *pd = evas_object_data_get(property, PROP_DATA); \
   if (!pd) \
     { \
        ERR("Can not show group data, because object[%p] is not a property object", property); \
        return ret; \
     }

struct _Prop_Data
{
   Evas_Object *workspace;
   Style *style;
   Part *part;
   Evas_Object *visual;
   Evas_Object *code;
   color_data *color_data;
   struct {
      Evas_Object *frame;
      Evas_Object *info;
      Evas_Object *shared_check;
      Evas_Object *ctxpopup;
      Evas_Object *min;
      Evas_Object *max;
      Evas_Object *current;
   } prop_group;
   struct {
      Evas_Object *frame;
      Evas_Object *name;
      Evas_Object *type;
      Evas_Object *scale;
      Evas_Object *mouse;
      Evas_Object *repeat;
      Evas_Object *clip_to;
      Evas_Object *ignore_flags;
      Evas_Object *select_mode;
      Evas_Object *entry_mode;
      Evas_Object *pointer_mode;
      Evas_Object *cursor_mode;
      Evas_Object *multiline;
   } prop_part;
   struct {
      Evas_Object *frame;
      Evas_Object *drag_x;
      Evas_Object *drag_y;
      Evas_Object *confine;
      Evas_Object *event;
   } prop_part_drag;
   struct {
      Evas_Object *frame;
      Evas_Object *state;
      Evas_Object *visible;
      Evas_Object *min;
      Evas_Object *max;
      Evas_Object *fixed;
      Evas_Object *align;
      Evas_Object *aspect;
      Evas_Object *aspect_pref;
      Evas_Object *color_class;
      Evas_Object *color;
   } prop_state;
   struct {
      Evas_Object *frame;
      Evas_Object *rel1_relative;
      Evas_Object *rel1_offset;
      Evas_Object *rel1_to;
      Evas_Object *rel2_relative;
      Evas_Object *rel2_offset;
      Evas_Object *rel2_to;
   } prop_state_object_area;
   struct {
      Evas_Object *frame;
      Evas_Object *text;
      Evas_Object *font;
      Evas_Object *size;
      Evas_Object *align;
      Evas_Object *source;
      Evas_Object *ellipsis;
      Evas_Object *min;
      Evas_Object *max;
      Evas_Object *fit;
      Evas_Object *text_source; /* not implemented in yet the edje */
      Evas_Object *color2;
      Evas_Object *color3;
      Evas_Object *effect;
   } prop_state_text;
   struct {
      Evas_Object *frame;
      Evas_Object *text;
      Evas_Object *style; /* not implemented in yet the edje  */
      Evas_Object *min;
      Evas_Object *max;
      Evas_Object *source;
      Evas_Object *source2;
      Evas_Object *source3;
      Evas_Object *source4;
      Evas_Object *source5;
      Evas_Object *source6;
   } prop_state_textblock;
   struct {
      Evas_Object *frame;
      Evas_Object *normal;
      Evas_Object *border;
      Evas_Object *middle;
      Evas_Object *tween;
   } prop_state_image;
   struct {
      Evas_Object *frame;
      Evas_Object *type;
      Evas_Object *smooth;
      Evas_Object *origin_relative;
      Evas_Object *origin_offset;
      Evas_Object *size_relative;
      Evas_Object *size_offset;
   } prop_state_fill;
};
typedef struct _Prop_Data Prop_Data;

enum Property_Combo
{
   CLIP_TO,
   FORWARD_EVENTS,
   SOURCE,
   DRAG_AREA,
   COLOR_CLASS,
   TEXT_STYLE,
   TEXT_SOURCE
};

static const char *edje_aspect_pref[] = { N_("None"),
                                          N_("Vertical"),
                                          N_("Horizontal"),
                                          N_("Both"),
                                          N_("Source"),
                                          NULL};

static const char *edje_select_mode[] = { N_("Default"),
                                          N_("Explicit"),
                                          NULL};

static const char *edje_entry_mode[] = { N_("None"),
                                         N_("Plain"),
                                         N_("Editable"),
                                         N_("Password"),
                                         NULL};

static const char *edje_pointer_mode[] = { N_("Autograb"),
                                           N_("Nograb"),
                                           NULL};

static const char *edje_cursor_mode[] = { N_("Under"),
                                          N_("Before"),
                                          NULL};

static const char *edje_fill_type[] = { N_("Scale"),
                                        N_("Tile"),
                                        NULL};

static const char *edje_ignore_flags[] = { N_("None"),
                                           N_("On hold"),
                                           NULL};

static const char *edje_text_effect_type[] = { N_("None"),
                                               N_("Plain"),
                                               N_("Outline"),
                                               N_("Soft Outline"),
                                               N_("Shadow"),
                                               N_("Soft Shadow"),
                                               N_("Outline Shadow"),
                                               N_("Outline Soft Shadow"),
                                               N_("Far Shadow"),
                                               N_("Far Soft Shadow"),
                                               N_("Glow"),
                                               NULL};

static const char *edje_text_effect_direction[] = { N_("None"),
                                                    N_("Bottom Right"),
                                                    N_("Bottom"),
                                                    N_("Bottom Left"),
                                                    N_("Left"),
                                                    N_("Top Left"),
                                                    N_("Top"),
                                                    N_("Top Right"),
                                                    N_("Right"),
                                                    NULL};

static const char *edje_middle_type[] = { N_("None"),
                                          N_("Default"),
                                          N_("Solid")};

static Eina_Bool
ui_property_state_obj_area_set(Evas_Object *property);

static void
ui_property_state_obj_area_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_text_set(Evas_Object *property);

static void
ui_property_state_text_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_textblock_set(Evas_Object *property);

static void
ui_property_state_textblock_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_image_set(Evas_Object *property);

static void
ui_property_state_image_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_fill_set(Evas_Object *property);

static void
ui_property_state_fill_unset(Evas_Object *property);

static Elm_Genlist_Item_Class *_itc_tween = NULL;

static void
_on_spinner_mouse_wheel(void *data __UNUSED__,
                        Evas *e __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info)
{
   Evas_Event_Mouse_Wheel *mev = event_info;
   mev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void
_del_prop_data(void *data,
               Evas *e __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;
   color_term(pd->color_data);
   free(pd);
}

static Evas_Object *
prop_item_label_add(Evas_Object *parent,
                    const char *lab_text,
                    const char *text)
{
   Evas_Object *item, *label;
   ITEM_ADD(parent, item, lab_text, "eflete/property/item/default")
   LABEL_ADD(parent, label, text)
   evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(item, "elm.swallow.content", label);
   return item;
}

static void
prop_item_label_update(Evas_Object *item,
                       const char *text)
{
   Evas_Object *label;
   label = elm_object_part_content_get(item, "elm.swallow.content");
   elm_object_text_set(label, text);
}

Evas_Object *
ui_property_add(Evas_Object *parent)
{
   Evas_Object *box, *scroller, *_bg, *tabs;
   Ewe_Tabs_Item *it;
   Prop_Data *pd;

   if (!parent) return NULL;
   pd = mem_calloc(1, sizeof(Prop_Data));
   tabs = ewe_tabs_add(parent);

   SCROLLER_ADD(tabs, scroller);
   BOX_ADD(scroller, box, EINA_FALSE, EINA_FALSE);
   elm_box_align_set(box, 0.5, 0.0);
   elm_object_content_set(scroller, box);

   GET_IMAGE(_bg, parent, "section-item-bg");
   elm_object_part_content_set(scroller, "elm.swallow.background", _bg);
   evas_object_show(_bg);
   pd->visual = scroller;
   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   it = ewe_tabs_item_append(tabs, NULL, _("Visual"), NULL);
   ewe_tabs_item_content_set(tabs, it, pd->visual);

   pd->code = elm_entry_add(tabs);
   elm_object_style_set(pd->code, DEFAULT_STYLE);
   elm_entry_single_line_set(pd->code, false);
   elm_scroller_policy_set(pd->code, ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
   evas_object_size_hint_weight_set(pd->code, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pd->code, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_scrollable_set(pd->code, true);
   it = ewe_tabs_item_append(tabs, it, _("Code"), NULL);
   ewe_tabs_item_content_set(tabs, it, pd->code);
   elm_entry_editable_set(pd->code, false);
   pd->color_data = color_init(eina_strbuf_new());

   evas_object_data_set(tabs, PROP_DATA, pd);
   evas_object_event_callback_add(tabs, EVAS_CALLBACK_DEL, _del_prop_data, pd);

   return tabs;
}

#define ITEM_2SPINNER_GROUP_CREATE(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_SPINNER_INT_CALLBACK(SUB, VALUE1) \
   ITEM_SPINNER_INT_CALLBACK(SUB, VALUE2) \
   ITEM_2SPINNER_GROUP_ADD(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_2SPINNER_GROUP_UPDATE(SUB, VALUE1, VALUE2)

/* group property */
ITEM_2SPINNER_GROUP_CREATE(_("min"), group_min, w, h)
ITEM_2SPINNER_GROUP_CREATE(_("max"), group_max, w, h)

#define pd_group pd->prop_group

static void
_on__dismissed(void *data __UNUSED__,
               Evas_Object *obj,
               void *event_info __UNUSED__)
{
   evas_object_hide(obj);
}
static void
_on__clicked(void *data,
             Evas *e,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   int x, y;
   Prop_Data *pd = (Prop_Data *)data;
   evas_object_smart_callback_del_full(pd_group.ctxpopup, "dismissed",
                                       _on__dismissed, pd);
   evas_object_smart_callback_add(pd_group.ctxpopup, "dismissed",
                                  _on__dismissed, pd);
   evas_pointer_canvas_xy_get(e, &x, &y);
   evas_object_move(pd_group.ctxpopup, x, y);
   evas_object_show(pd_group.ctxpopup);
}
static void
_prop_item_alias_update(Prop_Data *pd, Style *style, int aliases_count)
{
   Evas_Object *label, *label_ctx;
   Eina_List *list = NULL, *l;
   const char *text_info = NULL;
   char *list_data;
   Eina_Strbuf *text_ctx;

   label = elm_object_part_content_get(pd_group.info, "elm.swallow.content");
   label_ctx = elm_object_content_get(pd_group.ctxpopup);
   list = edje_edit_group_aliases_get(style->obj, style->full_group_name);
   if (style->isAlias)
     {
        text_info = eina_stringshare_add("This is alias of <a>%s</a>");
        text_info = eina_stringshare_printf(text_info,
                                            style->main_group->full_group_name);
     }
   else
     {
        text_info = eina_stringshare_add("changes in this style will also affect <a>%d elements.</a>");
        text_info = eina_stringshare_printf(text_info, aliases_count);

        int count = 1;
        text_ctx = eina_strbuf_new();

        EINA_LIST_FOREACH(list, l, list_data)
          {
             const char *step = "%d. %s";
             if (eina_strbuf_length_get(text_ctx) > 0)
               eina_strbuf_append(text_ctx, "</br>");
             step = eina_stringshare_printf(step, count++, list_data);
             eina_strbuf_append(text_ctx, step);
          }

        evas_object_event_callback_del_full(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                       _on__clicked, pd);
        evas_object_event_callback_add(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                       _on__clicked, pd);
        elm_object_text_set(label_ctx, eina_strbuf_string_get(text_ctx));
        eina_strbuf_free(text_ctx);
     }

   elm_object_text_set(label, text_info);
   evas_object_show(pd_group.info);
   eina_stringshare_del(text_info);
}

static void
_prop_item_shared_check_update(Evas_Object *item, int count)
{
   Evas_Object *entry;
   entry = elm_object_part_content_get(item, "info");
   Eina_Bool bool = false;
   if (count > 0) bool = true;
   elm_check_state_set(entry, bool);
   evas_object_show(item);
}

Eina_Bool
ui_property_style_set(Evas_Object *property, Style *style, Evas_Object *workspace)
{
   Evas_Object *group_frame, *box, *prop_box, *info_en = NULL;
   Evas_Object *info_image;
   Evas_Object *check, *label_ctx;
   Eina_List *aliases = NULL, *l;
   const char *text_info = NULL;
   int aliases_count = 0;
   char *list_data;
   Eina_Strbuf *text_ctx = NULL;
   char *markup_code;
   const char *colorized_code;

   if ((!property) || (!workspace)) return EINA_FALSE;
   PROP_DATA_GET(EINA_FALSE)

   evas_object_show(property);

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);

   pd->workspace = workspace;
   pd->style = style;
   if (style->isAlias) pd->style = style->main_group;
   if (pd->style != workspace_edit_object_get(workspace))
     {
        ERR("Cann't set the style! Style [%p] not matched"
            " with editable group in workspace", style);
        return false;
     }

   markup_code = elm_entry_utf8_to_markup(edje_edit_source_generate(pd->style->obj));
   colorized_code = color_apply(pd->color_data, markup_code,
                                strlen(markup_code), NULL, NULL);
   if (colorized_code) elm_object_text_set(pd->code, colorized_code);

   prop_box = elm_object_content_get(pd->visual);
   aliases = edje_edit_group_aliases_get(style->obj, style->full_group_name);
   aliases_count = eina_list_count(aliases);

   if (!pd_group.info)
     {
        LABEL_ADD(property, info_en, text_info)

        GET_IMAGE(info_image, property, "icon-notification");
        evas_object_show(info_image);

        pd_group.info = elm_layout_add(property);
        evas_object_size_hint_weight_set(pd_group.info, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(pd_group.info, EVAS_HINT_FILL, 0.0);
        elm_layout_file_set(pd_group.info, EFLETE_EDJ, "eflete/property/item/info");

        pd_group.ctxpopup = elm_ctxpopup_add(main_window_get());
        elm_ctxpopup_direction_priority_set(pd_group.ctxpopup,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN);
        elm_object_style_set(pd_group.ctxpopup, "eflete/info");
        evas_object_hide(pd_group.ctxpopup);

        if (style->isAlias)
          {
             text_info = eina_stringshare_add("This is alias of <a>%s</a>");
             text_info = eina_stringshare_printf(text_info,
                                                 style->main_group->full_group_name);
          }
        else
          {
             text_info = eina_stringshare_add("changes in this style will also affect <a>%d elements.</a>");
             text_info = eina_stringshare_printf(text_info, aliases_count);
             int count = 1;
             text_ctx = eina_strbuf_new();

             EINA_LIST_FOREACH(aliases, l, list_data)
               {
                  const char *step = "%d. %s";
                  if (eina_strbuf_length_get(text_ctx) > 0)
                    eina_strbuf_append(text_ctx, "</br>");
                  step = eina_stringshare_printf(step, count++, list_data);
                  eina_strbuf_append(text_ctx, step);
               }
             evas_object_event_callback_add(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                            _on__clicked, pd);
          }

        LABEL_ADD(property, label_ctx, eina_strbuf_string_get(text_ctx))
        eina_strbuf_free(text_ctx);
        elm_object_style_set(label_ctx, "eflete/info");
        elm_object_content_set(pd_group.ctxpopup, label_ctx);

        elm_object_text_set(info_en, text_info);
        elm_object_style_set(info_en, "eflete/info");
        elm_object_part_content_set(pd_group.info, "elm.swallow.content", info_en);
        elm_object_part_content_set(pd_group.info, "info", info_image);
     }

   if (!pd_group.shared_check)
     {
        CHECK_ADD(property, check, DEFAULT_STYLE)
        elm_object_text_set(check, "Shared style");
        if (aliases_count > 0) elm_check_state_set(check, true);
        elm_object_disabled_set(check, true);

        pd_group.shared_check = elm_layout_add(property);
        evas_object_size_hint_weight_set(pd_group.shared_check, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(pd_group.shared_check, EVAS_HINT_FILL, 0.0);
        elm_layout_file_set(pd_group.shared_check, EFLETE_EDJ, "eflete/property/item/info");
        elm_layout_signal_emit(pd_group.shared_check, "prop_item,content,hide", "eflete");
        elm_object_part_content_set(pd_group.shared_check, "info", check);
        evas_object_show(pd_group.shared_check);
     }

   if (!pd_group.frame)
     {
        FRAME_ADD(property, group_frame, true, _("Layout property"))
        BOX_ADD(group_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(group_frame, box);

        pd_group.min = prop_item_group_min_w_h_add(box, pd,
                          0.0, 9999.0, 1.0,
                          _("Minimum group width in pixels."),
                          _("Minimum group height in pixels."));
        pd_group.max = prop_item_group_max_w_h_add(box, pd,
                          -1.0, 9999.0, 1.0,
                          _("Maximum group width in pixels."),
                          _("Maximum group height in pixels."));
        elm_box_pack_end(box, pd_group.min);
        elm_box_pack_end(box, pd_group.max);

        elm_box_pack_start(prop_box, group_frame);
        pd_group.frame = group_frame;
     }
   else
     {
        if ((aliases_count > 0) || (style->isAlias))
          {
             _prop_item_alias_update(pd, style, aliases_count);
             evas_object_show(pd_group.info);
          }
        _prop_item_shared_check_update(pd_group.shared_check, aliases_count);
        prop_item_group_min_w_h_update(pd_group.min, pd);
        prop_item_group_max_w_h_update(pd_group.max, pd);
        evas_object_show(pd_group.frame);
     }
   if ((aliases_count > 0) || (style->isAlias))
     {
        elm_box_pack_start(prop_box, pd_group.info);
        evas_object_show(pd_group.info);
     }
   elm_box_pack_start(prop_box, pd_group.shared_check);

   return true;
}

void
ui_property_style_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   if (!property) return;
   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   evas_object_hide(pd_group.info);
   elm_box_unpack(prop_box, pd_group.info);
   elm_box_unpack(prop_box, pd_group.shared_check);
   evas_object_event_callback_del_full(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                  _on__clicked, pd);
   evas_object_hide(pd_group.frame);
   evas_object_hide(pd_group.shared_check);
   ui_property_part_unset(property);
   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   evas_object_hide(property);
}
#undef pd_group

static void
_on_part_name_unfocus(void *data,
                      Evas_Object *obj,
                      void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;

   const char *value = elm_entry_entry_get(obj);

   if (strcmp(value, pd->part->name))
     elm_entry_entry_set(obj, pd->part->name);
}

static void
_on_part_name_change(void *data,
                     Evas_Object *obj,
                     void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;
   int pos;
   const char *value;

   if (elm_entry_is_empty(obj)) return;

   value = elm_entry_entry_get(obj);
   if (!edje_edit_part_name_set(pd->style->obj, pd->part->name, value))
     {
        NOTIFY_INFO(5, "Wrong input value for name field");
        return;
     }

   pm_project_changed(app_data_get()->project);
   pd->part->name = value;
   workspace_edit_object_recalc(pd->workspace);
   pd->style->isModify = true;
   pos = elm_entry_cursor_pos_get(obj);
   evas_object_smart_callback_call(pd->workspace, "part,name,changed", pd->part);
   elm_object_focus_set(obj, true);
   elm_entry_cursor_pos_set(obj, pos);
}

static Evas_Object *
prop_item_part_name_add(Evas_Object *parent,
                        Prop_Data *pd,
                        const char *tooltip)
{
   Evas_Object *item, *entry;

   ITEM_ADD(parent, item, _("name"), "eflete/property/item/default");
   EWE_ENTRY_ADD(parent, entry, true, DEFAULT_STYLE);
   elm_entry_markup_filter_append(entry, elm_entry_filter_accept_set, &accept_prop);
   ewe_entry_entry_set(entry, pd->part->name);
   elm_object_tooltip_text_set(entry, tooltip);
   evas_object_smart_callback_add(entry, "changed,user", _on_part_name_change, pd);
   evas_object_smart_callback_add(entry, "unfocused", _on_part_name_unfocus, pd);
   elm_object_part_content_set(item, "elm.swallow.content", entry);
   evas_object_data_set(item, ITEM1, entry);
   return item;
}

#define ITEM_1CHECK_PART_CREATE(TEXT, SUB, VALUE) \
   ITEM_CHECK_PART_CALLBACK(SUB, VALUE) \
   ITEM_1CHEACK_PART_ADD(TEXT, SUB, VALUE) \
   ITEM_1CHEACK_PART_UPDATE(SUB, VALUE)

#define ITEM_1COMBOBOX_PART_CREATE(TYPE, TEXT, SUB, VALUE) \
   ITEM_1COMBOBOX_PART_CALLBACK(SUB, VALUE) \
   ITEM_1COMBOBOX_PART_ADD(TYPE, TEXT, SUB, VALUE) \
   ITEM_1COMBOBOX_PART_UPDATE(TYPE, TEXT, SUB, VALUE)

#define ITEM_1COMBOBOX_PART_TEXTBLOCK_CREATE(TEXT, SUB, VALUE, TYPE) \
   ITEM_1COMBOBOX_PART_TEXTBLOCK_CALLBACK(SUB, VALUE, TYPE) \
   ITEM_1COMBOBOX_PART_TEXTBLOCK_ADD(TEXT, SUB, VALUE, TYPE) \
   ITEM_1COMBOBOX_PART_TEXTBLOCK_UPDATE(TEXT, SUB, VALUE, TYPE)


#define ITEM_DRAG_PART_CREATE(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_CHECK_PART_CALLBACK(SUB, VALUE1) \
   ITEM_INT_PART_CALLBACK(SUB, VALUE2) \
   ITEM_DRAG_PART_ADD(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_DRAG_PART_UPDATE(SUB, VALUE1, VALUE2)

#define ITEM_1COMBOBOX_PART_PROPERTY_CREATE ITEM_1COMBOBOX_PART_TEXTBLOCK_CREATE

/* part property */
ITEM_1CHECK_PART_CREATE(_("scalable"), part, scale)
ITEM_1CHECK_PART_CREATE(_("mouse events"), part, mouse_events)
ITEM_1CHECK_PART_CREATE(_("event propagation"), part, repeat_events)
ITEM_1COMBOBOX_PART_CREATE(CLIP_TO, _("clip to"), part, clip_to)
ITEM_1COMBOBOX_PART_PROPERTY_CREATE(_("ignore flags"), part, ignore_flags, Evas_Event_Flags)
ITEM_1COMBOBOX_PART_TEXTBLOCK_CREATE(_("select mode"), part, select_mode, Edje_Edit_Select_Mode)
ITEM_1COMBOBOX_PART_TEXTBLOCK_CREATE(_("entry mode"), part, entry_mode, Edje_Edit_Entry_Mode)
ITEM_1COMBOBOX_PART_TEXTBLOCK_CREATE(_("pointer mode"), part, pointer_mode, Evas_Object_Pointer_Mode)
ITEM_1COMBOBOX_PART_TEXTBLOCK_CREATE(_("cursor mode"), part, cursor_mode, unsigned int)
ITEM_1CHECK_PART_CREATE(_("multiline"), part, multiline)

/* part drag property */
ITEM_DRAG_PART_CREATE(_("x"), part_drag, x, step_x)
ITEM_DRAG_PART_CREATE(_("y"), part_drag, y, step_y)
ITEM_1COMBOBOX_PART_CREATE(DRAG_AREA, _("drag area"), part_drag, confine)
ITEM_1COMBOBOX_PART_CREATE(FORWARD_EVENTS, _("forward events"), part_drag, event)

#define pd_part pd->prop_part
#define pd_part_drag pd->prop_part_drag
Eina_Bool
ui_property_part_set(Evas_Object *property, Part *part)
{
   Evas_Object *part_frame, *part_drag_frame, *box, *prop_box;
   Edje_Part_Type type;

   if ((!property) || (!part)) return EINA_FALSE;
   PROP_DATA_GET(EINA_FALSE)

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);

   pd->part = part;

   type = edje_edit_part_type_get(pd->style->obj, part->name);
   prop_box = elm_object_content_get(pd->visual);

   elm_box_unpack(prop_box, pd->prop_part_drag.frame);

   if (!pd_part.frame)
     {
        FRAME_ADD(pd->visual, part_frame, true, _("Part property"))
        BOX_ADD(part_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(part_frame, box);

        pd_part.name = prop_item_part_name_add(box, pd, _("Selected part name"));
        pd_part.type = prop_item_label_add(box, _("type"), wm_part_type_get(type));
        pd_part.scale = prop_item_part_scale_add(box, pd,
                           _("Specifies whether the part will scale "
                           "its size with an edje scaling factor."));
        pd_part.mouse = prop_item_part_mouse_events_add(box, pd,
                           _("Enable mouse events in this part."));
        pd_part.repeat = prop_item_part_repeat_events_add(box, pd,
                            _("Enable repeat mouse events to the parts below."));
        pd_part.clip_to = prop_item_part_clip_to_add(box, pd,
                             _("Show only the area of part that coincides with "
                             "another part's container"));
        pd_part.ignore_flags = prop_item_part_ignore_flags_add(box, pd,
                                  _("Specifies whether events with the given "
                                  " flags should be ignored"), edje_ignore_flags);
        elm_box_pack_end(box, pd_part.name);
        elm_box_pack_end(box, pd_part.type);
        elm_box_pack_end(box, pd_part.scale);
        elm_box_pack_end(box, pd_part.mouse);
        elm_box_pack_end(box, pd_part.repeat);
        elm_box_pack_end(box, pd_part.clip_to);
        elm_box_pack_end(box, pd_part.ignore_flags);

        if (part->type == EDJE_PART_TYPE_TEXTBLOCK)
          {
             pd_part.select_mode = prop_item_part_select_mode_add(box, pd,
                             _("Sets the selection mode for a textblock part"),
                             edje_select_mode);
             pd_part.entry_mode = prop_item_part_entry_mode_add(box, pd,
                             _("Sets the edit mode for a textblock part"),
                             edje_entry_mode);
             pd_part.pointer_mode = prop_item_part_pointer_mode_add(box, pd,
                             _("Sets the mouse pointer behavior for a given part."),
                             edje_pointer_mode);
             pd_part.cursor_mode = prop_item_part_cursor_mode_add(box, pd,
                             _("Sets the cursor mode for a textblock part"),
                             edje_cursor_mode);
             pd_part.multiline = prop_item_part_multiline_add(box, pd,
                           _("It causes a textblock that is editable to allow multiple lines for editing."));
             elm_box_pack_end(box, pd_part.select_mode);
             elm_box_pack_end(box, pd_part.entry_mode);
             elm_box_pack_end(box, pd_part.pointer_mode);
             elm_box_pack_end(box, pd_part.cursor_mode);
             elm_box_pack_end(box, pd_part.multiline);
          }

        elm_box_pack_after(prop_box, part_frame, pd->prop_group.frame);
        pd_part.frame = part_frame;
     }
   else
     {
         prop_item_label_update(pd_part.name, part->name);
         prop_item_label_update(pd_part.type, wm_part_type_get(type));
         prop_item_part_scale_update(pd_part.scale, pd);
         prop_item_part_mouse_events_update(pd_part.mouse, pd);
         prop_item_part_repeat_events_update(pd_part.repeat, pd);
         prop_item_part_clip_to_update(pd_part.clip_to, pd);
         prop_item_part_ignore_flags_update(pd_part.ignore_flags, pd);
         if (part->type == EDJE_PART_TYPE_TEXTBLOCK)
           {
             box = elm_object_content_get(pd_part.frame);
              if (!pd_part.select_mode)
                {
                   pd_part.select_mode = prop_item_part_select_mode_add(box, pd,
                             _("Sets the selection mode for a textblock part"),
                             edje_select_mode);
                   elm_box_pack_after(box, pd_part.select_mode, pd_part.clip_to);

                }
              else
                prop_item_part_select_mode_update(pd_part.select_mode, pd);

              if (!pd_part.entry_mode)
                {
                   pd_part.entry_mode = prop_item_part_entry_mode_add(box, pd,
                             _("Sets the edit mode for a textblock part."),
                             edje_entry_mode);
                   elm_box_pack_after(box, pd_part.entry_mode, pd_part.select_mode);

                }
              else
                prop_item_part_entry_mode_update(pd_part.entry_mode, pd);

              if (!pd_part.pointer_mode)
                {
                   pd_part.pointer_mode = prop_item_part_pointer_mode_add(box, pd,
                             _("Sets the mouse pointer behavior for a given part."),
                             edje_pointer_mode);
                   elm_box_pack_after(box, pd_part.pointer_mode, pd_part.entry_mode);

                }
              else
                prop_item_part_pointer_mode_update(pd_part.pointer_mode, pd);

              if (!pd_part.cursor_mode)
                {
                   pd_part.cursor_mode = prop_item_part_cursor_mode_add(box, pd,
                             _(" Sets the cursor mode for a textblock part."),
                             edje_cursor_mode);
                   elm_box_pack_after(box, pd_part.cursor_mode, pd_part.pointer_mode);

                }
              else
                prop_item_part_cursor_mode_update(pd_part.cursor_mode, pd);
              if (!pd_part.multiline)
                {
                  pd_part.multiline = prop_item_part_multiline_add(box, pd,
                           _("It causes a textblock that is editable to allow multiple lines for editing."));
                   elm_box_pack_after(box, pd_part.multiline, pd_part.cursor_mode);
                }
              else
                prop_item_part_multiline_update(pd_part.multiline, pd);
           }
         else
           {
              box = elm_object_content_get(pd_part.frame);
              elm_box_unpack(box, pd_part.select_mode);
              elm_box_unpack(box, pd_part.entry_mode);
              elm_box_unpack(box, pd_part.pointer_mode);
              elm_box_unpack(box, pd_part.cursor_mode);
              elm_box_unpack(box, pd_part.multiline);
              evas_object_del(pd_part.select_mode);
              evas_object_del(pd_part.entry_mode);
              evas_object_del(pd_part.pointer_mode);
              evas_object_del(pd_part.cursor_mode);
              evas_object_del(pd_part.multiline);
              pd_part.select_mode = NULL;
              pd_part.entry_mode = NULL;
              pd_part.pointer_mode = NULL;
              pd_part.cursor_mode = NULL;
              pd_part.multiline = NULL;
           }
         evas_object_show(pd_part.frame);
     }
   if (!pd_part_drag.frame)
     {
        FRAME_ADD(pd->visual, part_drag_frame, true, _("Part dragable property"))
        BOX_ADD(part_drag_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(part_drag_frame, box);

        pd_part_drag.drag_x = prop_item_part_drag_x_step_x_add(box, pd,
                                 0.0, 9999.0, 1.0, _("%.0f"),
                                 _("Enable/Disable draggin along X axis"),
                                 _("Set a drag step value"));
        pd_part_drag.drag_y = prop_item_part_drag_y_step_y_add(box, pd,
                                 0.0, 9999.0, 1.0, _("%.0f"),
                                 _("Enable/Disable draggin along Y axis"),
                                 _("Set a drag step value"));
        pd_part_drag.confine = prop_item_part_drag_confine_add(box, pd,
                                 _("Limits the movement of the dragged part to "
                                  "another part's container'"));
        pd_part_drag.event = prop_item_part_drag_event_add(box, pd,
                                _("It causes the part to forward the drag events "
                                "to another part, thus ignoring them for itself."));

        elm_box_pack_end(box, pd_part_drag.drag_x);
        elm_box_pack_end(box, pd_part_drag.drag_y);
        elm_box_pack_end(box, pd_part_drag.confine);
        elm_box_pack_end(box, pd_part_drag.event);

        elm_box_pack_after(prop_box, part_drag_frame, pd_part.frame);
        pd_part_drag.frame = part_drag_frame;
     }
   else
     {
        prop_item_part_drag_x_step_x_update(pd_part_drag.drag_x, pd);
        prop_item_part_drag_y_step_y_update(pd_part_drag.drag_y, pd);
        prop_item_part_drag_confine_update(pd_part_drag.confine, pd);
        prop_item_part_drag_event_update(pd_part_drag.event, pd);
        elm_box_pack_after(prop_box, pd_part_drag.frame, pd_part.frame);
        evas_object_show(pd_part_drag.frame);
     }

   return true;
}

void
ui_property_part_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   if (!property) return;
   PROP_DATA_GET()

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   prop_box = elm_object_content_get(pd->visual);

   if (pd->prop_part.frame)
     {
        elm_box_unpack(prop_box, pd->prop_part.frame);
        evas_object_del(pd->prop_part.frame);
        pd->prop_part.frame = NULL;
     }
   if (pd->prop_part_drag.frame)
     {
        elm_box_unpack(prop_box, pd->prop_part_drag.frame);
        evas_object_del(pd->prop_part_drag.frame);
        pd->prop_part_drag.frame = NULL;
     }

   if (pd->prop_state.frame)
     {
        elm_box_unpack(prop_box, pd->prop_state.frame);
        evas_object_del(pd->prop_state.frame);
        pd->prop_state.frame = NULL;
     }

   ui_property_state_unset(property);
}
#undef pd_part
#undef pd_part_drag

#define ITEM_2SPINNER_STATE_INT_CREATE(TEXT, SUB, VALUE1, VALUE2, STYLE) \
   ITEM_SPINNER_STATE_INT_CALLBACK(SUB, VALUE1) \
   ITEM_SPINNER_STATE_INT_CALLBACK(SUB, VALUE2) \
   ITEM_2SPINNER_STATE_ADD(TEXT, SUB, VALUE1, VALUE2, STYLE) \
   ITEM_2SPINNER_STATE_UPDATE(SUB, VALUE1, VALUE2)

#define ITEM_2SPINNER_STATE_DOUBLE_CREATE(TEXT, SUB, VALUE1, VALUE2, STYLE) \
   ITEM_SPINNER_STATE_DOUBLE_CALLBACK(SUB, VALUE1) \
   ITEM_SPINNER_STATE_DOUBLE_CALLBACK(SUB, VALUE2) \
   ITEM_2SPINNER_STATE_ADD(TEXT, SUB, VALUE1, VALUE2, STYLE) \
   ITEM_2SPINNER_STATE_UPDATE(SUB, VALUE1, VALUE2)

#define ITEM_1ENTRY_STATE_CREATE(TEXT, SUB, VALUE, FILTER) \
   ITEM_STRING_STATE_CALLBACK(SUB, VALUE) \
   ITEM_1ENTRY_STATE_ADD(TEXT, SUB, VALUE, FILTER) \
   ITEM_1ENTRY_STATE_UPDATE(SUB, VALUE)

#define ITEM_STATE_CCL_CREATE(TYPE, TEXT, SUB, VALUE, LIST) \
   ITEM_1COMBOBOX_STATE_ADD(TYPE, TEXT, SUB, VALUE, LIST) \
   ITEM_1COMBOBOX_STATE_UPDATE(TYPE, TEXT, SUB, VALUE, LIST)

#define ITEM_COLOR_STATE_CREATE(TEXT, SUB, VALUE) \
   ITEM_COLOR_STATE_CALLBACK(SUB, VALUE) \
   ITEM_COLOR_STATE_ADD(TEXT, SUB, VALUE) \
   ITEM_COLOR_STATE_UPDATE(SUB, VALUE)

#define ITEM_1CHECK_STATE_CREATE(TEXT, SUB, VALUE) \
   ITEM_CHECK_STATE_CALLBACK(SUB, VALUE) \
   ITEM_1CHEACK_STATE_ADD(TEXT, SUB, VALUE) \
   ITEM_1CHEACK_STATE_UPDATE(SUB, VALUE)

#define ITEM_2CHECK_STATE_CREATE(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_CHECK_STATE_CALLBACK(SUB, VALUE1) \
   ITEM_CHECK_STATE_CALLBACK(SUB, VALUE2) \
   ITEM_2CHEACK_STATE_ADD(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_2CHEACK_STATE_UPDATE(SUB, VALUE1, VALUE2)

#define ITEM_1COMBOBOX_PART_STATE_CREATE(TEXT, SUB, VALUE, TYPE) \
   ITEM_1COMBOBOX_STATE_PART_CALLBACK(SUB, VALUE, TYPE) \
   ITEM_1COMBOBOX_STATE_PART_ADD(TEXT, SUB, VALUE, TYPE) \
   ITEM_1COMBOBOX_STATE_PART_UPDATE(TEXT, SUB, VALUE, TYPE)

ITEM_1CHECK_STATE_CREATE(_("visible"), state, visible)
ITEM_2SPINNER_STATE_INT_CREATE(_("min"), state_min, w, h, "eflete/property/item/default")
ITEM_2SPINNER_STATE_INT_CREATE(_("max"), state_max, w, h, "eflete/property/item/default")
ITEM_2CHECK_STATE_CREATE(_("fixed"), state_fixed, w, h)
ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("align"), state_align, x, y, "eflete/property/item/default")
ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("aspect ratio"), state_aspect, min, max, "eflete/property/item/default")
ITEM_STATE_CCL_CREATE(COLOR_CLASS, _("color class"), state, color_class, color_classes)
ITEM_COLOR_STATE_CREATE(_("color"), state, color)
ITEM_1COMBOBOX_PART_STATE_CREATE(_("aspect ratio mode"), state, aspect_pref, unsigned char)

Eina_Bool
ui_property_state_set(Evas_Object *property, Part *part)
{
   Evas_Object *state_frame, *box, *prop_box;
   Edje_Part_Type type;
   char state[BUFF_MAX];
   char *markup_code;
   const char *colorized_code;

   if ((!property) || (!part)) return EINA_FALSE;
   PROP_DATA_GET(EINA_FALSE)

   if (pd->part != part) return EINA_FALSE; /* time for panic */
   #define pd_state pd->prop_state

   type = edje_edit_part_type_get(pd->style->obj, part->name);
   sprintf(state, "%s %1.2f", part->curr_state, part->curr_state_value);

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_state.frame);

   if (!pd_state.frame)
     {
        FRAME_ADD(property, state_frame, true, _("State"))
        BOX_ADD(state_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(state_frame, box);

        pd_state.state = prop_item_label_add(box, _("state"), state);
        pd_state.visible = prop_item_state_visible_add(box, pd,
                                                       "");
        pd_state.min = prop_item_state_min_w_h_add(box, pd,
                          0.0, 9999.0, 1.0, "%.0f",
                          "w:", "px", "h:", "px",
                          _("Minimum part width in pixels."),
                          _("Minimum part height in pixels."),
                          false);
        pd_state.max = prop_item_state_max_w_h_add(box, pd,
                          -1.0, 9999.0, 1.0, "%.0f",
                          "w:", "px", "h:", "px",
                          _("Maximum part width in pixels."),
                          _("Maximum part height in pixels."),
                          false);
        pd_state.fixed = prop_item_state_fixed_w_h_add(box, pd,
                           _("This affects the minimum width calculation."),
                           _("This affects the minimum height calculation."));
        pd_state.align = prop_item_state_align_x_y_add(box, pd,
                            0, 100, 1, NULL,
                            "x:", "%", "y:", "%",
                            _("Part horizontal align: 0.0 = left  1.0 = right"),
                            _("Part vertical align: 0.0 = top  1.0 = bottom"),
                            true);
        pd_state.aspect = prop_item_state_aspect_min_max_add(box, pd,
                             0, 100, 1, NULL,
                             "x:", "", "y:", "",
                            _("Normally width and height can be "
                             "resized to any values independently"),
                            _("Normally width and height can be "
                             "resized to any values independently"),
                            false);
        pd_state.aspect_pref = prop_item_state_aspect_pref_add(box, pd,
                                   _("The aspect control hints for this object."),
                                   edje_aspect_pref);
        pd_state.color_class = prop_item_state_color_class_add(box, pd,
                                   _on_state_color_class_change,
                                   _("Current color class"));
        pd_state.color = prop_item_state_color_add(box, pd,
                            _("Part main color."));

        elm_box_pack_end(box, pd_state.state);
        elm_box_pack_end(box, pd_state.visible);
        elm_box_pack_end(box, pd_state.min);
        elm_box_pack_end(box, pd_state.max);
        elm_box_pack_end(box, pd_state.fixed);
        elm_box_pack_end(box, pd_state.align);
        elm_box_pack_end(box, pd_state.aspect_pref);
        elm_box_pack_end(box, pd_state.aspect);
        elm_box_pack_end(box, pd_state.color_class);
        if (type == EDJE_PART_TYPE_SPACER)
          {
             evas_object_hide(pd_state.color);
             elm_box_unpack(box, pd_state.color);
          }
        else elm_box_pack_end(box, pd_state.color);
        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_after(prop_box, state_frame, pd->prop_part_drag.frame);
        pd_state.frame = state_frame;
     }
   else
     {
        box = elm_object_content_get(pd_state.frame);
        /* unpack item for part color, because we don't know whether it is necessary */
        elm_box_unpack(box, pd_state.color);
        prop_item_label_update(pd_state.state, state);
        prop_item_state_visible_update(pd_state.visible, pd);
        prop_item_state_min_w_h_update(pd_state.min, pd, false);
        prop_item_state_max_w_h_update(pd_state.max, pd,false);
        prop_item_state_fixed_w_h_update(pd_state.fixed, pd);
        prop_item_state_align_x_y_update(pd_state.align, pd, true);
        prop_item_state_aspect_min_max_update(pd_state.aspect, pd, false);
        prop_item_state_aspect_pref_update(pd_state.aspect_pref, pd);
        prop_item_state_color_class_update(pd_state.color_class, pd);
        if (type != EDJE_PART_TYPE_SPACER)
          {
             prop_item_state_color_update(pd_state.color, pd);
             evas_object_show(pd_state.color);
             elm_box_pack_end(box, pd_state.color);
          }
        else evas_object_hide(pd_state.color);
        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_end(prop_box, pd_state.frame);
        evas_object_show(pd_state.frame);
     }

   ui_property_state_obj_area_set(property);
   if (type == EDJE_PART_TYPE_TEXT) ui_property_state_text_set(property);
   else ui_property_state_text_unset(property);
   if (type == EDJE_PART_TYPE_TEXTBLOCK) ui_property_state_textblock_set(property);
   else ui_property_state_textblock_unset(property);
   if (type == EDJE_PART_TYPE_IMAGE) ui_property_state_image_set(property);
   else ui_property_state_image_unset(property);
   if ((type == EDJE_PART_TYPE_IMAGE) || (type == EDJE_PART_TYPE_PROXY))
     ui_property_state_fill_set(property);
   else if ((type != EDJE_PART_TYPE_IMAGE) && (type != EDJE_PART_TYPE_PROXY))
     ui_property_state_fill_unset(property);

   markup_code = elm_entry_utf8_to_markup(edje_edit_source_generate(pd->style->obj));
   colorized_code = color_apply(pd->color_data, markup_code,
                                strlen(markup_code), NULL, NULL);
   if (colorized_code) elm_object_text_set(pd->code, colorized_code);
   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

   #undef pd_state
   return true;
}

void
ui_property_state_unset(Evas_Object *property)
{
   if (!property) return;
   PROP_DATA_GET()

   evas_object_hide(pd->prop_state.frame);

   ui_property_state_obj_area_unset(property);
   ui_property_state_text_unset(property);
   ui_property_state_image_unset(property);
   ui_property_state_textblock_unset(property);
   ui_property_state_fill_unset(property);
}

/* FIXME: edje_edit_state_relX_to do not update object properly.
   Setting of any other param completes the object update.
   Here min_w param is setted to its current value. */
#define ITEM_REL_TO_COMBOBOX_STATE_CALLBACK(SUB, VALUE) \
static void \
_on_combobox_##SUB##_##VALUE##_change(void *data, \
                             Evas_Object *obj EINA_UNUSED, \
                             void *ei) \
{ \
   Prop_Data *pd = (Prop_Data *)data; \
   Ewe_Combobox_Item *item = ei; \
   if (strcmp(item->title, _("Layout"))) \
     edje_edit_##SUB##_##VALUE##_set(pd->style->obj, pd->part->name, \
                                     pd->part->curr_state, pd->part->curr_state_value, \
                                     item->title); \
   else edje_edit_##SUB##_##VALUE##_set(pd->style->obj, pd->part->name, \
                                        pd->part->curr_state, pd->part->curr_state_value, \
                                        NULL); \
  int temp = edje_edit_state_min_w_get(pd->style->obj, pd->part->name, \
                                       pd->part->curr_state, pd->part->curr_state_value); \
  edje_edit_state_min_w_set(pd->style->obj, pd->part->name, \
                            pd->part->curr_state, \
                            pd->part->curr_state_value, temp); \
   pm_project_changed(app_data_get()->project); \
   workspace_edit_object_recalc(pd->workspace); \
   pd->style->isModify = true; \
}

#define ITEM_2COMBOBOX_STATE_CREATE(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_REL_TO_COMBOBOX_STATE_CALLBACK(SUB, VALUE1) \
   ITEM_REL_TO_COMBOBOX_STATE_CALLBACK(SUB, VALUE2) \
   ITEM_2COMBOBOX_STATE_ADD(TEXT, SUB, VALUE1, VALUE2) \
   ITEM_2COMBOBOX_STATE_UPDATE(SUB, VALUE1, VALUE2)

ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("align"), state_rel1_relative, x, y, "eflete/property/item/relative")
ITEM_2SPINNER_STATE_INT_CREATE(_("offset"), state_rel1_offset, x, y, "eflete/property/item/relative")
ITEM_2COMBOBOX_STATE_CREATE(_("relative to"), state_rel1_to, x, y)
ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("align"), state_rel2_relative, x, y, "eflete/property/item/relative")
ITEM_2SPINNER_STATE_INT_CREATE(_("offset"), state_rel2_offset, x, y, "eflete/property/item/relative")
ITEM_2COMBOBOX_STATE_CREATE(_("relative to"), state_rel2_to, x, y)

#define pd_obj_area pd->prop_state_object_area
static Eina_Bool
ui_property_state_obj_area_set(Evas_Object *property)
{
   Evas_Object *obj_area_frame, *box, *prop_box;
   Evas_Object *separator, *icon;
   PROP_DATA_GET(EINA_FALSE)

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_obj_area.frame);

   if (!pd_obj_area.frame)
     {
        FRAME_ADD(property, obj_area_frame, true, _("Object area"))
        BOX_ADD(obj_area_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(obj_area_frame, box);

        separator = elm_separator_add(obj_area_frame);
        elm_separator_horizontal_set(separator, true);
        elm_object_style_set(separator, "eflete/property");
        elm_object_part_text_set(separator, "eflete.text", _("Start point"));
        ICON_ADD(separator, icon, false, "icon_start-point")
        elm_object_part_content_set(separator, "eflete.swallow.icon", icon);
        evas_object_show(separator);

        pd_obj_area.rel1_to = prop_item_state_rel1_to_x_y_add(box, pd,
                        _("Causes a corner to be positioned relatively to the X axis of another "
                        "part. Setting to \"\" will un-set this value"),
                        _("Causes a corner to be positioned relatively to the Y axis of another "
                        "part. Setting to \"\" will un-set this value"));
        pd_obj_area.rel1_relative = prop_item_state_rel1_relative_x_y_add(box, pd,
                              -500, 500, 1, NULL,
                              "x:", "%", "y:", "%",
                              _("Define the position of left-up corner of the part's container. "
                              "Moves a corner to a relative position inside the container "
                              "by X axis."),
                              _("Define the position of left-up corner of the part's container. "
                              "Moves a corner to a relative position inside the container "
                              "by Y axis."),
                              true);
        ICON_ADD(pd_obj_area.rel1_relative, icon, false, "icon_align");
        elm_object_part_content_set(pd_obj_area.rel1_relative, "eflete.swallow.icon", icon);
        pd_obj_area.rel1_offset = prop_item_state_rel1_offset_x_y_add(box, pd,
                            -9999.0, 9999.0, 1.0, "%.0f",
                            "x:", "px", "y:", "px",
                            _("Left offset from relative position in pixels"),
                            _("Top offset from relative position in pixels"),
                            false);
        ICON_ADD(pd_obj_area.rel1_offset, icon, false, "icon_offset");
        elm_object_part_content_set(pd_obj_area.rel1_offset, "eflete.swallow.icon", icon);

        elm_box_pack_end(box, separator);
        elm_box_pack_end(box, pd_obj_area.rel1_to);
        elm_box_pack_end(box, pd_obj_area.rel1_relative);
        elm_box_pack_end(box, pd_obj_area.rel1_offset);

        separator = elm_separator_add(obj_area_frame);
        elm_separator_horizontal_set(separator, true);
        elm_object_style_set(separator, "eflete/property");
        elm_object_part_text_set(separator, "eflete.text", _("End point"));
        ICON_ADD(separator, icon, false, "icon_end-point");
        elm_object_part_content_set(separator, "eflete.swallow.icon", icon);
        evas_object_show(separator);

        pd_obj_area.rel2_to = prop_item_state_rel2_to_x_y_add(box, pd,
                        _("Causes a corner to be positioned relatively to the X axis of another "
                        "part. Setting to \"\" will un-set this value"),
                        _("Causes a corner to be positioned relatively to the Y axis of another "
                        "part. Setting to \"\" will un-set this value"));
        pd_obj_area.rel2_relative = prop_item_state_rel2_relative_x_y_add(box, pd,
                              -500, 500, 1, NULL,
                              "x:", "%", "y:", "%",
                              _("Define the position of right-down corner of the part's container. "
                              "Moves a corner to a relative position inside the container "
                              "by X axis."),
                              _("Define the position of right-down corner of the part's container. "
                              "Moves a corner to a relative position inside the container "
                              "by Y axis."),
                              true);
        ICON_ADD(pd_obj_area.rel2_relative, icon, false, "icon_align");
        elm_object_part_content_set(pd_obj_area.rel2_relative, "eflete.swallow.icon", icon);
        pd_obj_area.rel2_offset = prop_item_state_rel2_offset_x_y_add(box, pd,
                            -9999.0, 9999.0, 1.0, "%.0f",
                            "x:", "px", "y:", "px",
                            _("Left offset from relative position in pixels"),
                            _("Top offset from relative position in pixels"),
                            false);
        ICON_ADD(pd_obj_area.rel2_offset, icon, false, "icon_offset");
        elm_object_part_content_set(pd_obj_area.rel2_offset, "eflete.swallow.icon", icon);


        elm_box_pack_end(box, separator);
        elm_box_pack_end(box, pd_obj_area.rel2_to);
        elm_box_pack_end(box, pd_obj_area.rel2_relative);
        elm_box_pack_end(box, pd_obj_area.rel2_offset);


        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_end(prop_box, obj_area_frame);
        pd_obj_area.frame = obj_area_frame;
     }
   else
     {
        prop_item_state_rel1_relative_x_y_update(pd_obj_area.rel1_relative, pd, true);
        prop_item_state_rel1_offset_x_y_update(pd_obj_area.rel1_offset, pd, false);
        prop_item_state_rel1_to_x_y_update(pd_obj_area.rel1_to, pd);

        prop_item_state_rel2_relative_x_y_update(pd_obj_area.rel2_relative, pd, true);
        prop_item_state_rel2_offset_x_y_update(pd_obj_area.rel2_offset, pd, false);
        prop_item_state_rel2_to_x_y_update(pd_obj_area.rel2_to, pd);

        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_end(prop_box, pd_obj_area.frame);
        evas_object_show(pd_obj_area.frame);
     }

   return true;
}

static void
ui_property_state_obj_area_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_obj_area.frame);
   evas_object_hide(pd_obj_area.frame);
}
#undef pd_obj_area



#define ITEM_1SPINNER_STATE_INT_CREATE(TEXT, SUB, VALUE) \
   ITEM_SPINNER_STATE_INT_CALLBACK(SUB, VALUE) \
   ITEM_1SPINNER_STATE_ADD(TEXT, SUB, VALUE) \
   ITEM_1SPINNER_STATE_UPDATE(SUB, VALUE)

#define ITEM_1SPINNER_STATE_DOUBLE_CREATE(TEXT, SUB, VALUE) \
   ITEM_SPINNER_STATE_DOUBLE_CALLBACK(SUB, VALUE) \
   ITEM_1SPINNER_STATE_ADD(TEXT, SUB, VALUE) \
   ITEM_1SPINNER_STATE_UPDATE(SUB, VALUE)

#define ITEM_1COMBOBOX_STATE_CREATE(TYPE, TEXT, SUB, VALUE, LIST) \
   ITEM_COMBOBOX_STATE_CALLBACK(TYPE, TEXT, SUB, VALUE) \
   ITEM_1COMBOBOX_STATE_ADD(TYPE, TEXT, SUB, VALUE, LIST) \
   ITEM_1COMBOBOX_STATE_UPDATE(TYPE, TEXT, SUB, VALUE, LIST)

ITEM_1ENTRY_STATE_CREATE(_("text"), state, text, NULL)
ITEM_1ENTRY_STATE_CREATE(_("font"), state, font, &accept_prop)
ITEM_1SPINNER_STATE_INT_CREATE(_("size"), state_text, size)
ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("align"), state_text_align, x, y, "eflete/property/item/default")
ITEM_2CHECK_STATE_CREATE(_("max"), state_text_max, x, y)
ITEM_2CHECK_STATE_CREATE(_("min"), state_text_min, x, y)
ITEM_2CHECK_STATE_CREATE(_("fit"), state_text_fit, x, y)
ITEM_COLOR_STATE_CREATE(_("shadow color"), state, color3)
ITEM_COLOR_STATE_CREATE(_("outline color"), state, color2)
ITEM_1COMBOBOX_STATE_CREATE(SOURCE, _("source"), state_text, source, styles)
ITEM_1COMBOBOX_STATE_CREATE(TEXT_SOURCE, _("text source"), state_text, text_source, styles)


static void
_text_effect_get(Prop_Data *pd, int *type, int *direction)
{
   Edje_Text_Effect edje_effect = edje_edit_part_effect_get(pd->style->obj,
                                                            pd->part->name);
   if (type)
     *type = edje_effect & EDJE_TEXT_EFFECT_MASK_BASIC;
   if ((direction) && (*type >= EDJE_TEXT_EFFECT_SOFT_OUTLINE) &&
       (*type != EDJE_TEXT_EFFECT_GLOW))
     *direction = (edje_effect & EDJE_TEXT_EFFECT_MASK_SHADOW_DIRECTION) >> 4;
   else *direction = 0;
}

typedef struct {
   Evas_Object *type_combobox;
   Evas_Object *direction_combobox;
   Prop_Data *pd;
} _text_effect_callback_data;

static void
_text_effect_value_update(_text_effect_callback_data *effect_data)
{
   Edje_Text_Effect edje_effect;
   edje_effect = ewe_combobox_select_item_get(effect_data->type_combobox)->index;
   edje_effect |= ewe_combobox_select_item_get(effect_data->direction_combobox)->index << 4;
   edje_edit_part_effect_set(effect_data->pd->style->obj,
                             effect_data->pd->part->name, edje_effect);

   workspace_edit_object_recalc(effect_data->pd->workspace);
   effect_data->pd->style->isModify = true;
}

static void
prop_item_state_effect_update(Evas_Object *item, Prop_Data *pd)
{
   int type, direction;
   Evas_Object *combobox;
   _text_effect_get(pd, &type, &direction);
   combobox = evas_object_data_get(item, ITEM1);
   ewe_combobox_select_item_set(combobox, type);
   combobox = evas_object_data_get(item, ITEM2);
   if ((type <= EDJE_TEXT_EFFECT_SOFT_OUTLINE) ||
       (type == EDJE_TEXT_EFFECT_GLOW))
     {
        elm_object_disabled_set(combobox, true);
        ewe_combobox_select_item_set(combobox, 0);
     }
   else
     {
        ewe_combobox_select_item_set(combobox, direction);
        elm_object_disabled_set(combobox, false);
     }
}

static void
_on_text_effect_type_change(void *data,
                            Evas_Object *obj EINA_UNUSED,
                            void *event_info)
{
   _text_effect_callback_data *effect_data = (_text_effect_callback_data *)data;
   Ewe_Combobox_Item *selected_item = event_info;

   switch (selected_item->index)
     {
      case EDJE_TEXT_EFFECT_NONE:
      case EDJE_TEXT_EFFECT_PLAIN:
      case EDJE_TEXT_EFFECT_OUTLINE:
      case EDJE_TEXT_EFFECT_SOFT_OUTLINE:
      case EDJE_TEXT_EFFECT_GLOW:
        {
           ewe_combobox_select_item_set(effect_data->direction_combobox, 0);
           elm_object_disabled_set(effect_data->direction_combobox, true);
           break;
        }
      default:
        {
           if (elm_object_disabled_get(effect_data->direction_combobox))
             {
                ewe_combobox_select_item_set(effect_data->direction_combobox, 1);
                elm_object_disabled_set(effect_data->direction_combobox, false);
             }
        }
     }
   _text_effect_value_update(effect_data);
   pm_project_changed(app_data_get()->project);
}

static void
_on_text_effect_direction_change(void *data,
                                 Evas_Object *obj EINA_UNUSED,
                                 void *event_info EINA_UNUSED)
{
   _text_effect_value_update(data);
   pm_project_changed(app_data_get()->project);
}

static void
_del_text_effect_callback_data(void *data,
                               Evas *e __UNUSED__,
                               Evas_Object *obj __UNUSED__,
                               void *ei __UNUSED__)
{
   _text_effect_callback_data *data_to_del = (_text_effect_callback_data *)data;
   free(data_to_del);
   pm_project_changed(app_data_get()->project);
}

#define ADD_TEXT_EFFECT_COMBOBOX(_prop_name, _prop_callback, _prop_names_array) \
   i = 0; \
   ITEM_ADD(box, layout, _(#_prop_name":"), "eflete/property/item/small_label") \
   EWE_COMBOBOX_ADD(layout, combobox) \
   while (_prop_names_array[i]) \
     { \
        ewe_combobox_item_add(combobox, _prop_names_array[i]); \
        i++; \
     } \
   evas_object_smart_callback_add(combobox, "selected", \
                                  _prop_callback, callback_data); \
   elm_object_part_content_set(layout, "elm.swallow.content", combobox); \
   elm_box_pack_end(box, layout);

static Evas_Object *
prop_item_state_effect_add(Evas_Object *parent, Prop_Data *pd)
{
   int i = 0;
   Evas_Object *item, *box, *layout;
   Evas_Object *combobox;

   _text_effect_callback_data *callback_data = (_text_effect_callback_data *)
                                   mem_malloc(sizeof(_text_effect_callback_data));
   if (!callback_data)
     return NULL;

   ITEM_ADD(parent, item, _("effect"), "eflete/property/item/multiple_prop")
   evas_object_event_callback_add(item, EVAS_CALLBACK_DEL,
                                  _del_text_effect_callback_data,
                                  callback_data);
   callback_data->pd = pd;

   BOX_ADD(item, box, false, true)
   elm_box_padding_set(box, 0, 6);

   ADD_TEXT_EFFECT_COMBOBOX(type, _on_text_effect_type_change, edje_text_effect_type);
   callback_data->type_combobox = combobox;
   evas_object_data_set(item, ITEM1, combobox);
   elm_object_tooltip_text_set(combobox, _("Causes Edje to draw the selected effect."));

   ADD_TEXT_EFFECT_COMBOBOX(direction, _on_text_effect_direction_change, edje_text_effect_direction);
   callback_data->direction_combobox = combobox;
   evas_object_data_set(item, ITEM2, combobox);
   elm_object_tooltip_text_set(combobox, _("Shadow directions."));

   elm_object_part_content_set(item, "elm.swallow.content", box);
   prop_item_state_effect_update(item, pd);
   return item;
}
#undef ADD_TEXT_EFFECT_COMBOBOX

#define pd_text pd->prop_state_text


static void
_on_state_text_ellipsis_change(void *data,
                               Evas_Object *obj,
                               void *event_info __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;
   double value = elm_spinner_value_get(obj);
   if (!edje_edit_state_text_elipsis_set(pd->style->obj,
                                         pd->part->name,
                                         pd->part->curr_state,
                                         pd->part->curr_state_value,
                                         value))
     return;
   workspace_edit_object_recalc(pd->workspace);
   pd->style->isModify = true;
   pm_project_changed(app_data_get()->project);
   /*TODO: app_data_get()->project AGRRRRRR!!!!! need a define project in the pd */
}

static void
_on_state_text_ellipsis_toggle_change(void *data,
                                      Evas_Object *obj,
                                      void *event_info __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;
   Eina_Bool state = elm_check_state_get(obj);
   Evas_Object *spinner = evas_object_data_get(pd_text.ellipsis, ITEM2);
   double value = 0.0;

   if (state)
     {
        elm_object_disabled_set(spinner, false);
        value = elm_spinner_value_get(spinner);
     }
   else
     {
        elm_object_disabled_set(spinner, true);
        value = -1.0;
     }
   edje_edit_state_text_elipsis_set(pd->style->obj,
                                    pd->part->name,
                                    pd->part->curr_state,
                                    pd->part->curr_state_value,
                                    value);
   workspace_edit_object_recalc(pd->workspace);
   pd->style->isModify = true;
   pm_project_changed(app_data_get()->project);
}

static Evas_Object *
prop_item_state_text_ellipsis_add(Evas_Object *parent,
                                  Prop_Data *pd,
                                  const char *tooltip)
{
   Evas_Object *item, *box, *layout, *check, *spinner;
   double value;

   ITEM_ADD(parent, item, "ellipsis", "eflete/property/item/default")
   BOX_ADD(item, box, true, true)
   elm_box_align_set(box, 0.0, 0.5);
   ITEM_CONTEINER_2LABEL_ADD(box, layout, "turn", NULL);
   elm_object_tooltip_text_set(item, tooltip);

   CHECK_ADD(layout, check, "eflete/toggle")
   elm_object_part_content_set(layout, "eflete.content", check);
   evas_object_smart_callback_add(check, "changed",
                                  _on_state_text_ellipsis_toggle_change, pd);

   SPINNER_ADD(box, spinner, 0.0, 1.0, 0.1, true, DEFAULT_STYLE)
   elm_spinner_label_format_set(spinner, "%1.2f");
   value = edje_edit_state_text_elipsis_get(pd->style->obj,
                                            pd->part->name,
                                            pd->part->curr_state,
                                            pd->part->curr_state_value);
   evas_object_smart_callback_add(spinner, "changed",
                                  _on_state_text_ellipsis_change, pd);

   if (value < 0)
     {
        elm_check_state_set(check, false);
        elm_object_disabled_set(spinner, true);
        elm_spinner_value_set(spinner, 0.0);
     }
   else
     {
        elm_check_state_set(check, true);
        elm_object_disabled_set(spinner, false);
        elm_spinner_value_set(spinner, value);
     }

   elm_box_pack_end(box, layout);
   elm_box_pack_end(box, spinner);
   elm_object_part_content_set(item, "elm.swallow.content", box);
   evas_object_data_set(item, ITEM1, check);
   evas_object_data_set(item, ITEM2, spinner);
   return item;

}

static void
prop_item_state_text_ellipsis_update(Evas_Object *item,
                                     Prop_Data *pd)
{
   Evas_Object *check, *spinner;
   double value;

   check = evas_object_data_get(item, ITEM1);
   evas_object_smart_callback_del_full(check, "changed",
                                       _on_state_text_ellipsis_toggle_change, pd);
   evas_object_smart_callback_add(check, "changed",
                                  _on_state_text_ellipsis_toggle_change, pd);

   spinner = evas_object_data_get(item, ITEM2);
   evas_object_smart_callback_del_full(spinner, "changed",
                                       _on_state_text_ellipsis_change, pd);
   evas_object_smart_callback_add(spinner, "changed",
                                  _on_state_text_ellipsis_change, pd);

   value = edje_edit_state_text_elipsis_get(pd->style->obj,
                                            pd->part->name,
                                            pd->part->curr_state,
                                            pd->part->curr_state_value);
    if (value < 0)
     {
        elm_check_state_set(check, false);
        elm_object_disabled_set(spinner, true);
        elm_spinner_value_set(spinner, 0.0);
     }
   else
     {
        elm_check_state_set(check, true);
        elm_object_disabled_set(spinner, false);
        elm_spinner_value_set(spinner, value);
     }
}

static Eina_Bool
ui_property_state_text_set(Evas_Object *property)
{
   Evas_Object *text_frame, *box, *prop_box;
   PROP_DATA_GET(EINA_FALSE)

   /* if previos selected part is TEXT too, unpack it */
   ui_property_state_text_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_text.frame)
     {
         FRAME_ADD(property, text_frame, true, _("Text"))
         BOX_ADD(text_frame, box, EINA_FALSE, EINA_FALSE)
         elm_box_align_set(box, 0.5, 0.0);
         elm_object_content_set(text_frame, box);

         pd_text.text = prop_item_state_text_add(box, pd, NULL,
                           _("Set the text of part."), NULL);
         pd_text.font = prop_item_state_font_add(box, pd, NULL,
                           _("Change the text's font"), NULL);
         pd_text.size = prop_item_state_text_size_add(box, pd,
                           0.0, 128.0, 1.0, "%.0f pt",
                           _("Change text font's size.'"));
         pd_text.align = prop_item_state_text_align_x_y_add(box, pd,
                            0, 100, 1, NULL,
                            "x:", "%", "y:", "%",
                            _("Text horizontal align. "
                            "0.0 = left  1.0 = right"),
                            _("Text vertical align. "
                            "0.0 = top  1.0 = bottom"),
                            true);
         pd_text.min = prop_item_state_text_min_x_y_add(box, pd,
                           _("When any of the parameters is enabled it forces \t"
                           "the minimum size of the container to be equal to\t"
                           "the minimum size of the text."),
                           _("When any of the parameters is enabled it forces \t"
                           "the minimum size of the container to be equal to\t"
                           "the minimum size of the text."));
         pd_text.max = prop_item_state_text_max_x_y_add(box, pd,
                           _("When any of the parameters is enabled it forces \t"
                           "the maximum size of the container to be equal to\t"
                           "the maximum size of the text."),
                           _("When any of the parameters is enabled it forces \t"
                           "the maximum size of the container to be equal to\t"
                           "the maximum size of the text."));
         pd_text.source = prop_item_state_text_source_add(box, pd, NULL,
                            _("Causes the part to use the text properties\t"
                            "(like font and size) of another part\t"
                            "and update them as they change."));
         pd_text.text_source = prop_item_state_text_text_source_add(box, pd, NULL,
                               _("Causes the part to display the text content of \t"
                               "another part and update them as they change."));
         pd_text.ellipsis = prop_item_state_text_ellipsis_add(box, pd,
                            _("Cut text if biggest then part's area"
                            "0.0 = fix the left side  1.0 = right side"));
         pd_text.fit = prop_item_state_text_fit_x_y_add(box, pd,
                          _("Resize the text for it to fit in it's container by X axis"),
                          _("Resize the text for it to fit in it's container by Y axis"));
         pd_text.color2 = prop_item_state_color2_add(box, pd,
                            _("Text shadow color."));
         pd_text.color3 = prop_item_state_color3_add(box, pd,
                            _("Text outline color."));
         pd_text.effect = prop_item_state_effect_add(box, pd);

         elm_box_pack_end(box, pd_text.text);
         elm_box_pack_end(box, pd_text.font);
         elm_box_pack_end(box, pd_text.size);
         elm_box_pack_end(box, pd_text.align);
         elm_box_pack_end(box, pd_text.min);
         elm_box_pack_end(box, pd_text.max);
         elm_box_pack_end(box, pd_text.source);
         elm_box_pack_end(box, pd_text.text_source);
         elm_box_pack_end(box, pd_text.ellipsis);
         elm_box_pack_end(box, pd_text.fit);
         elm_box_pack_end(box, pd_text.color2);
         elm_box_pack_end(box, pd_text.color3);
         elm_box_pack_end(box, pd_text.effect);

         elm_box_pack_end(prop_box, text_frame);
         pd_text.frame = text_frame;
     }
   else
     {
        prop_item_state_text_update(pd_text.text, pd);
        prop_item_state_font_update(pd_text.font, pd);
        prop_item_state_text_size_update(pd_text.size, pd);
        prop_item_state_text_align_x_y_update(pd_text.align, pd, true);
        prop_item_state_text_min_x_y_update(pd_text.min, pd);
        prop_item_state_text_max_x_y_update(pd_text.max, pd);
        prop_item_state_text_source_update(pd_text.source, pd);
        prop_item_state_text_text_source_update(pd_text.text_source, pd);
        prop_item_state_text_ellipsis_update(pd_text.ellipsis, pd);
        prop_item_state_text_fit_x_y_update(pd_text.fit, pd);
        prop_item_state_color2_update(pd_text.color2, pd);
        prop_item_state_color3_update(pd_text.color3, pd);
        prop_item_state_effect_update(pd_text.effect, pd);
        elm_box_pack_end(prop_box, pd_text.frame);
        evas_object_show(pd_text.frame);
     }

   return true;
}

static void
ui_property_state_text_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_text.frame);
   evas_object_hide(pd_text.frame);
}
#undef pd_text

#define pd_textblock pd->prop_state_textblock
ITEM_1COMBOBOX_PART_CREATE(SOURCE, _("source (under selected text)"), part, source)
ITEM_1COMBOBOX_PART_CREATE(SOURCE, _("source2 (over selected text)"), part, source2)
ITEM_1COMBOBOX_PART_CREATE(SOURCE, _("source3 (under cursor)"), part, source3)
ITEM_1COMBOBOX_PART_CREATE(SOURCE, _("source4 (over cursor)"), part, source4)
ITEM_1COMBOBOX_PART_CREATE(SOURCE, _("source5 (under anchor)"), part, source5)
ITEM_1COMBOBOX_PART_CREATE(SOURCE, _("source6 (over anchor)"), part, source6)

ITEM_1COMBOBOX_STATE_CREATE(TEXT_STYLE, _("text style"), state, text_style, styles)

static Eina_Bool
ui_property_state_textblock_set(Evas_Object *property)
{
   Evas_Object *textblock_frame, *box, *prop_box;
   PROP_DATA_GET(EINA_FALSE)

   /* if previos selected part is TEXTBLOCK too, unpack it */
   ui_property_state_textblock_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_textblock.frame)
     {
         FRAME_ADD(property, textblock_frame, true, _("TextBlock"))
         BOX_ADD(textblock_frame, box, EINA_FALSE, EINA_FALSE)
         elm_box_align_set(box, 0.5, 0.0);
         elm_object_content_set(textblock_frame, box);

         pd_textblock.text = prop_item_state_text_add(box, pd, NULL,
                           _("Set the text of part."), NULL);
         pd_textblock.style = prop_item_state_text_style_add(box, pd,
                           _on_state_text_style_change,
                           _("Set the text style of part."));
         pd_textblock.min = prop_item_state_text_min_x_y_add(box, pd,
                           _("When any of the parameters is enabled it forces \t"
                           "the minimum size of the container to be equal to\t"
                           "the minimum size of the text."),
                           _("When any of the parameters is enabled it forces \t"
                           "the minimum size of the container to be equal to\t"
                           "the minimum size of the text."));
         pd_textblock.max = prop_item_state_text_max_x_y_add(box, pd,
                           _("When any of the parameters is enabled it forces \t"
                           "the maximum size of the container to be equal to\t"
                           "the maximum size of the text."),
                           _("When any of the parameters is enabled it forces \t"
                           "the maximum size of the container to be equal to\t"
                           "the maximum size of the text."));
         pd_textblock.source = prop_item_part_source_add(box, pd,
                               _("Used for the group to be loaded and used for selection \t"
                               "display UNDER the selected text the source \t"
                               "of TEXTBLOCK part."));
         pd_textblock.source2 = prop_item_part_source2_add(box, pd,
                               _("It is used for the group to be loaded and used for \t"
                               "selection display OVER the selected text."));
         pd_textblock.source3 = prop_item_part_source3_add(box, pd,
                               _("It is used for the group to be loaded and used for \t"
                               "cursor display UNDER the cursor position."));
         pd_textblock.source4 = prop_item_part_source4_add(box, pd,
                               _("It is used for the group to be loaded and used \t"
                               "for cursor display OVER the cursor position."));
         pd_textblock.source5 = prop_item_part_source5_add(box, pd,
                               _("It is used for the group to be loaded and used for \t"
                               "anchors display UNDER the anchor position."));
         pd_textblock.source6 = prop_item_part_source6_add(box, pd,
                               _("It is used for the group to be loaded and used for \t"
                               "anchor display OVER the anchor position."));

         elm_box_pack_end(box, pd_textblock.text);
         elm_box_pack_end(box, pd_textblock.style);
         elm_box_pack_end(box, pd_textblock.min);
         elm_box_pack_end(box, pd_textblock.max);
         elm_box_pack_end(box, pd_textblock.source);
         elm_box_pack_end(box, pd_textblock.source2);
         elm_box_pack_end(box, pd_textblock.source3);
         elm_box_pack_end(box, pd_textblock.source4);
         elm_box_pack_end(box, pd_textblock.source5);
         elm_box_pack_end(box, pd_textblock.source6);

         elm_box_pack_end(prop_box, textblock_frame);
         pd_textblock.frame = textblock_frame;
     }
   else
     {
        prop_item_state_text_update(pd_textblock.text, pd);
        prop_item_state_text_style_update(pd_textblock.style, pd);
        prop_item_state_text_min_x_y_update(pd_textblock.min, pd);
        prop_item_state_text_max_x_y_update(pd_textblock.max, pd);
        prop_item_part_source_update(pd_textblock.source, pd);
        prop_item_part_source2_update(pd_textblock.source2, pd);
        prop_item_part_source3_update(pd_textblock.source3, pd);
        prop_item_part_source4_update(pd_textblock.source4, pd);
        prop_item_part_source5_update(pd_textblock.source5, pd);
        prop_item_part_source6_update(pd_textblock.source6, pd);
        elm_box_pack_end(prop_box, pd_textblock.frame);
        evas_object_show(pd_textblock.frame);
     }
   return true;
}

static void
ui_property_state_textblock_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_textblock.frame);
   evas_object_hide(pd_textblock.frame);
}
#undef pd_textblock


#define ITEM_IM_BORDER_STATE_CREATE(TEXT, SUB, VALUE) \
   ITEM_IM_BORDER_STATE_CALLBACK(SUB, VALUE) \
   ITEM_IM_BORDER_STATE_ADD(TEXT, SUB, VALUE) \
   ITEM_IM_BORDER_STATE_UPDATE(SUB, VALUE)

#define pd_image pd->prop_state_image

static void
_on_image_editor_done(void *data,
                      Evas_Object *obj __UNUSED__,
                      void *event_info)
{
   Evas_Object *image_entry, * border_entry;
   Prop_Data *pd = (Prop_Data *)data;
   const char *value;
   const char *selected = (const char *)event_info;

   if (!selected) return;
   image_entry = elm_object_part_content_get(pd_image.normal, "elm.swallow.content");
   border_entry = elm_object_part_content_get(pd_image.border, "elm.swallow.content");
   value = elm_entry_entry_get(image_entry);

   if (strcmp(value, selected) == 0) return;
   ewe_entry_entry_set(image_entry, selected);
   evas_object_smart_callback_call(image_entry, "changed,user", NULL);
   ewe_entry_entry_set(border_entry, NULL);
   evas_object_smart_callback_call(border_entry, "changed,user", NULL);
}

static void
_on_state_image_choose(void *data,
                       Evas_Object *obj __UNUSED__,
                       void *ei __UNUSED__)
{
   Evas_Object *img_edit;
   Prop_Data *pd = (Prop_Data *)data;
   Evas_Object *entry = elm_object_part_content_get(pd_image.normal, "elm.swallow.content");
   const char *selected = elm_entry_entry_get(entry);

   App_Data *ap = app_data_get();

   img_edit = image_editor_window_add(ap->project, SINGLE);
   image_editor_file_choose(img_edit, selected);
   image_editor_callback_add(img_edit, _on_image_editor_done, pd);
}

static void
_del_tween_image(void *data,
                 Evas_Object *obj,
                 void *event_info __UNUSED__)
{
   Evas_Object *tween_list = elm_object_parent_widget_get(obj);
   const char *selected = (const char *)data;
   Elm_Object_Item *it = elm_genlist_selected_item_get(tween_list);
   Prop_Data *pd = evas_object_data_get(tween_list, PROP_DATA);

   if ((!selected) || (!it) || (!pd)) return;
   if (edje_edit_state_tween_del(pd->style->obj, pd->part->name,
                                 pd->part->curr_state, pd->part->curr_state_value,
                                 selected))
     {
        elm_object_item_del(it);
        pd->style->isModify = true;
        pm_project_changed(app_data_get()->project);
     }
}

static void
_on_image_editor_tween_done(void *data,
                            Evas_Object *obj __UNUSED__,
                            void *ei)
{
   Evas_Object *tween_list = (Evas_Object *)data;
   Eina_List *selected = (Eina_List *)ei;
   Eina_List *l = NULL;
   const char *name = NULL;
   Prop_Data *pd = evas_object_data_get(tween_list, PROP_DATA);

   if ((!selected) || (!pd)) return;

   EINA_LIST_FOREACH(selected, l, name)
     {
        if (edje_edit_state_tween_add(pd->style->obj, pd->part->name,
                                      pd->part->curr_state,
                                      pd->part->curr_state_value, name))
          {
             elm_genlist_item_append(tween_list, _itc_tween, name, NULL,
                                     ELM_GENLIST_ITEM_NONE, NULL, NULL);
             pd->style->isModify = true;
             pm_project_changed(app_data_get()->project);
          }
     }
   elm_frame_collapse_go(pd->prop_state_image.tween, false);
   edje_edit_string_list_free(selected);
}

static void
_add_tween_image(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Evas_Object *img_edit;
   Evas_Object *tween_list = (Evas_Object *)data;

   App_Data *ap = app_data_get();

   img_edit = image_editor_window_add(ap->project, MULTIPLE);
   image_editor_callback_add(img_edit, _on_image_editor_tween_done, tween_list);

   return;
}

static char *
_item_label_get(void *data,
                Evas_Object *obj __UNUSED__,
                const char *part __UNUSED__)
{
   return strdup(data);
}

static Evas_Object *
_item_content_get(void *data, Evas_Object *obj, const char *part)
{
   const char* buf = NULL;
   const char *file = NULL, *group = NULL;
   Evas_Load_Error err;
   Evas_Object *button, *image;
   Prop_Data *pd = NULL;

   if (!strcmp(part, "elm.swallow.icon"))
    {
       pd = evas_object_data_get(obj, PROP_DATA);
       if (!pd) return NULL;
       edje_object_file_get((const Eo*)pd->style->obj, &file, &group);
       image = evas_object_image_add(evas_object_evas_get(obj));
       buf = eina_stringshare_printf("edje/images/%i",
                        edje_edit_image_id_get(pd->style->obj, (const char*)data));
       evas_object_image_file_set(image, file, buf);
       err = evas_object_image_load_error_get(image);
       if (err != EVAS_LOAD_ERROR_NONE)
         {
            WARN("Image [%s] from file [%s] loaded with errors", buf, file);
            return NULL;
         }
       evas_object_image_filled_set(image, true);
       return image;
    }
   if (!strcmp(part, "elm.swallow.end"))
    {
       BUTTON_ADD(obj, button, NULL)
       ICON_ADD(button, image, true, "icon-remove");
       elm_object_part_content_set(button, NULL, image);
       evas_object_smart_callback_add(button, "clicked", _del_tween_image,
                                      (const char*)data);
       elm_object_style_set(button, "eflete/simple");
       return button;
    }
    return NULL;

}

static void
_tween_image_moved(Evas_Object *data,
                   Evas_Object *obj,
                   Elm_Object_Item *item EINA_UNUSED)
{
   Prop_Data *pd = (Prop_Data *)data;
   Eina_List *images_list, *l;
   Elm_Object_Item *next;
   const char *image_name;

   images_list = edje_edit_state_tweens_list_get(pd->style->obj,
                                                 pd->part->name,
                                                 pd->part->curr_state,
                                                 pd->part->curr_state_value);

   EINA_LIST_FOREACH(images_list, l, image_name)
     {
        edje_edit_state_tween_del(pd->style->obj, pd->part->name,
                                  pd->part->curr_state, pd->part->curr_state_value,
                                  image_name);
     }

   next = elm_genlist_first_item_get(obj);
   while (next)
     {
        image_name  = elm_object_item_data_get(next);
        edje_edit_state_tween_add(pd->style->obj, pd->part->name,
                                  pd->part->curr_state, pd->part->curr_state_value,
                                  image_name);
        next = elm_genlist_item_next_get(next);
     }
   pm_project_changed(app_data_get()->project);
}

Evas_Object *
prop_item_state_image_tween_add(Evas_Object *box, Prop_Data *pd)
{
   Evas_Object *tween_frame, *tween_list;
   Evas_Object *button, *icon;
   Eina_List *images_list, *l;
   char *image_name;

   FRAME_ADD(box, tween_frame, true, _("Tweens"))
   elm_object_style_set(tween_frame, "eflete/tween");

   tween_list = elm_genlist_add(tween_frame);
   elm_genlist_longpress_timeout_set(tween_list, 0.2);
   elm_genlist_reorder_mode_set(tween_list, true);
   evas_object_data_set(tween_list, PROP_DATA, pd);
   elm_object_style_set(tween_list, "eflete/default");

   if (!_itc_tween)
     {
        _itc_tween = elm_genlist_item_class_new();
        _itc_tween->item_style = "eflete/tween";
        _itc_tween->func.text_get = _item_label_get;
        _itc_tween->func.content_get = _item_content_get;
        _itc_tween->func.state_get = NULL;
        _itc_tween->func.del = NULL;
     }

   images_list = edje_edit_state_tweens_list_get(pd->style->obj,
                                                 pd->part->name,
                                                 pd->part->curr_state,
                                                 pd->part->curr_state_value);
   if (!images_list) elm_frame_collapse_go(tween_frame, true);
   EINA_LIST_FOREACH(images_list, l, image_name)
     {
       elm_genlist_item_append(tween_list, _itc_tween,
                               eina_stringshare_add(image_name), NULL,
                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
     }
   edje_edit_string_list_free(images_list);
   elm_object_content_set(tween_frame, tween_list);

   BUTTON_ADD(tween_frame, button, NULL)
   ICON_ADD(button, icon, true, "icon-add");
   elm_object_part_content_set(button, NULL, icon);
   evas_object_smart_callback_add(button, "clicked", _add_tween_image,
                                  tween_list);
   elm_object_style_set(button, "eflete/simple");
   elm_object_part_content_set(tween_frame, "elm.swallow.add", button);
   evas_object_smart_callback_add(tween_list, "moved",
                                  (Evas_Smart_Cb)_tween_image_moved, pd);

   evas_object_show(tween_list);

   return tween_frame;
}

void
prop_item_state_image_tween_update(Evas_Object *tween, Prop_Data *pd)
{
   Evas_Object *tween_list;
   Eina_List *images_list, *l;
   const char *image_name = NULL;

   tween_list = elm_object_content_get(tween);
   elm_genlist_clear(tween_list);
   images_list = edje_edit_state_tweens_list_get(pd->style->obj,
                                                 pd->part->name,
                                                 pd->part->curr_state,
                                                 pd->part->curr_state_value);
   if (!images_list) elm_frame_collapse_go(tween, true);
   else elm_frame_collapse_go(tween, false);

   EINA_LIST_FOREACH(images_list, l, image_name)
     {
       elm_genlist_item_append(tween_list, _itc_tween, image_name, NULL,
                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
     }
}

ITEM_1ENTRY_STATE_CREATE(_("image"), state, image, &accept_prop)
ITEM_IM_BORDER_STATE_CREATE(_("border"), state_image, border)
ITEM_1COMBOBOX_PART_STATE_CREATE(_("middle"), state_image, border_fill, unsigned char)

static Eina_Bool
ui_property_state_image_set(Evas_Object *property)
{
   Evas_Object *image_frame, *box, *prop_box;
   PROP_DATA_GET(EINA_FALSE)

   /* if previos selected part is IMAGE too, unpack it */
   ui_property_state_image_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_image.frame)
     {
        FRAME_ADD(property, image_frame, true, _("Image"))
        BOX_ADD(image_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(image_frame, box);

        pd_image.normal = prop_item_state_image_add(box, pd, _on_state_image_choose,
                             _("Current image name"),
                             _("Change image"));
        Evas_Object *entry = elm_object_part_content_get(pd_image.normal, "elm.swallow.content");
        ewe_entry_regex_set(entry, IMAGE_NAME_REGEX, EWE_REG_ICASE | EWE_REG_EXTENDED);
        ewe_entry_regex_autocheck_set(entry, true);
        ewe_entry_regex_glow_set(entry, true);
        pd_image.border = prop_item_state_image_border_add(box, pd,
                             _("Image's border value"));
        entry = elm_object_part_content_get(pd_image.border, "elm.swallow.content");
        ewe_entry_regex_set(entry, IMAGE_BORDER_REGEX, EWE_REG_EXTENDED);
        ewe_entry_regex_autocheck_set(entry, true);
        ewe_entry_regex_glow_set(entry, true);
        pd_image.middle = prop_item_state_image_border_fill_add(box, pd,
                             _("Image's middle value"), edje_middle_type);

        pd_image.tween = prop_item_state_image_tween_add(box, pd);

        elm_box_pack_end(box, pd_image.normal);
        elm_box_pack_end(box, pd_image.border);
        elm_box_pack_end(box, pd_image.middle);
        elm_box_pack_end(box, pd_image.tween);

        elm_box_pack_end(prop_box, image_frame);
        pd_image.frame = image_frame;
     }
   else
     {
        prop_item_state_image_update(pd_image.normal, pd);
        prop_item_state_image_border_update(pd_image.border, pd);
        prop_item_state_image_border_fill_update(pd_image.middle, pd);
        prop_item_state_image_tween_update(pd_image.tween, pd);
        elm_box_pack_end(prop_box, pd_image.frame);
        evas_object_show(pd_image.frame);
     }

   return true;
}

static void
ui_property_state_image_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_image.frame);
   evas_object_hide(pd_image.frame);
}
#undef pd_image

#define pd_fill pd->prop_state_fill
ITEM_1COMBOBOX_PART_STATE_CREATE(_("type"), state_fill, type, unsigned char)
ITEM_1CHECK_STATE_CREATE(_("smooth"), state_fill, smooth)
ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("align"), state_fill_origin_relative, x, y, "eflete/property/item/relative")
ITEM_2SPINNER_STATE_INT_CREATE(_("offset"), state_fill_origin_offset, x, y, "eflete/property/item/relative")
ITEM_2SPINNER_STATE_DOUBLE_CREATE(_("align"), state_fill_size_relative, x, y, "eflete/property/item/relative")
ITEM_2SPINNER_STATE_INT_CREATE(_("offset"), state_fill_size_offset, x, y, "eflete/property/item/relative")

static Eina_Bool
ui_property_state_fill_set(Evas_Object *property)
{
   Evas_Object *fill_frame, *box, *prop_box;
   Evas_Object *icon, *separator;
   PROP_DATA_GET(EINA_FALSE)

   /* if previos selected part is IMAGE or PROXY too, unpack it */
   ui_property_state_fill_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_fill.frame)
     {
        FRAME_ADD(property, fill_frame, true, _("Fill"))
        BOX_ADD(fill_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(fill_frame, box);

        pd_fill.type = prop_item_state_fill_type_add(box, pd,
                             _("Sets the image fill type."),
                             edje_fill_type);

        pd_fill.smooth = prop_item_state_fill_smooth_add(box, pd,
                             _("On/off image smooth on scaling"));

        elm_box_pack_end(box, pd_fill.type);
        elm_box_pack_end(box, pd_fill.smooth);

       /* origin subblock of fill block */
        separator = elm_separator_add(fill_frame);
        elm_separator_horizontal_set(separator, true);
        elm_object_style_set(separator, "eflete/property");
        elm_object_part_text_set(separator, "eflete.text", _("Origin"));
        ICON_ADD(separator, icon, false, "icon_start-point")
        elm_object_part_content_set(separator, "eflete.swallow.icon", icon);
        evas_object_show(separator);

        pd_fill.origin_relative = prop_item_state_fill_origin_relative_x_y_add(box,
                                     pd, -500, 500, 1, NULL,
                                     "x:", "%", "y:", "%",
                                     _("Sets the starting point X coordinate "
                                       "relatively to displayed element's content."),
                                     _("Sets the starting point Y coordinate "
                                       "relatively to displayed element's content."),
                                     true);
        ICON_ADD(pd_fill.origin_relative, icon, false, "icon_align");
        elm_object_part_content_set(pd_fill.origin_relative, "eflete.swallow.icon", icon);
        pd_fill.origin_offset = prop_item_state_fill_origin_offset_x_y_add(box,
                                     pd, -9999.0, 9999.0, 1.0, "%.0f",
                                     "x:", "px", "y:", "px",
                                     _("Affects the starting point a fixed "
                                       "number of pixels along X axis"),
                                     _("Affects the starting point a fixed "
                                       "number of pixels along Y axis"),
                                     false);
        ICON_ADD(pd_fill.origin_offset, icon, false, "icon_offset");
        elm_object_part_content_set(pd_fill.origin_offset, "eflete.swallow.icon", icon);

        elm_box_pack_end(box, separator);
        elm_box_pack_end(box, pd_fill.origin_relative);
        elm_box_pack_end(box, pd_fill.origin_offset);

        /* size subblock of fill block */
        separator = elm_separator_add(fill_frame);
        elm_separator_horizontal_set(separator, true);
        elm_object_style_set(separator, "eflete/property");
        elm_object_part_text_set(separator, "eflete.text", _("Size"));
        ICON_ADD(separator, icon, false, "icon_end-point")
        elm_object_part_content_set(separator, "eflete.swallow.icon", icon);
        evas_object_show(separator);

        pd_fill.size_relative = prop_item_state_fill_size_relative_x_y_add(box,
                                     pd, -500, 500, 1, NULL,
                                     "x:", "%", "y:", "%",
                                     _("Value that represent the percentual value "
                                       "of the original size of the element by X axis."),
                                     _("Value that represent the percentual value "
                                       "of the original size of the element by Y axis."),
                                      true);
        ICON_ADD(pd_fill.size_relative, icon, false, "icon_align");
        elm_object_part_content_set(pd_fill.size_relative, "eflete.swallow.icon", icon);
        pd_fill.size_offset = prop_item_state_fill_size_offset_x_y_add(box,
                                     pd, -9999.0, 9999.0, 1.0, "%.0f",
                                     "x:", "px", "y:", "px",
                                     _("Affects the size of the tile a fixed "
                                       "number of pixels along X axis."),
                                     _("Affects the size of the tile a fixed "
                                       "number of pixels along Y axis."),
                                     false);
        ICON_ADD(pd_fill.size_offset, icon, false, "icon_offset");
        elm_object_part_content_set(pd_fill.size_offset, "eflete.swallow.icon", icon);

        elm_box_pack_end(box, separator);
        elm_box_pack_end(box, pd_fill.size_relative);
        elm_box_pack_end(box, pd_fill.size_offset);

        pd_fill.frame = fill_frame;
        elm_box_pack_end(prop_box, pd_fill.frame);
     }
   else
     {
        prop_item_state_fill_type_update(pd_fill.type, pd);
        prop_item_state_fill_smooth_update(pd_fill.smooth, pd);
        prop_item_state_fill_origin_relative_x_y_update(pd_fill.origin_relative, pd, true);
        prop_item_state_fill_origin_offset_x_y_update(pd_fill.origin_offset, pd, false);
        prop_item_state_fill_size_relative_x_y_update(pd_fill.size_relative, pd, true);
        prop_item_state_fill_size_offset_x_y_update(pd_fill.size_offset, pd, false);

        elm_box_pack_end(prop_box, pd_fill.frame);
     }
   evas_object_show(pd_fill.frame);
   return true;
}

static void
ui_property_state_fill_unset(Evas_Object *property)
{
   Evas_Object *prop_box;
   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_fill.frame);
   evas_object_hide(pd_fill.frame);
}
#undef pd_fill

static void
_on_state_color_class_change(void *data,
                             Evas_Object *obj EINA_UNUSED,
                             void *event_info)
{
   Prop_Data *pd = (Prop_Data *)data;
   int r, g, b, a, r1, g1, b1, a1, r2, g2, b2, a2;
   r = g = b = a = r1 = g1 = b1 = a1 = r2 = g2 = b2 = a2 = 0;

   Ewe_Combobox_Item *item = event_info;
   if (strcmp(item->title, "None"))
     {
        edje_edit_state_color_class_set(pd->style->obj, pd->part->name,
                                     pd->part->curr_state,
                                     pd->part->curr_state_value,
                                     item->title);
        edje_edit_color_class_colors_get(pd->style->obj, item->title, &r, &g, &b, &a,
                                         &r1, &g1, &b1, &a1, &r2, &g2, &b2, &a2);
        edje_edit_state_color_set(pd->style->obj, pd->part->name,
                             pd->part->curr_state, pd->part->curr_state_value,
                             r, g, b, a);
        edje_edit_state_color2_set(pd->style->obj, pd->part->name,
                             pd->part->curr_state, pd->part->curr_state_value,
                             r1, g1, b1, a1);
        edje_edit_state_color3_set(pd->style->obj, pd->part->name,
                             pd->part->curr_state, pd->part->curr_state_value,
                             r2, g2, b2, a2);

        prop_item_state_color_update(pd->prop_state.color, pd);
        prop_item_state_color2_update(pd->prop_state_text.color2, pd);
        prop_item_state_color3_update(pd->prop_state_text.color3, pd);
     }
   else edje_edit_state_color_class_set(pd->style->obj, pd->part->name,
                                        pd->part->curr_state,
                                        pd->part->curr_state_value,
                                        NULL);

   workspace_edit_object_recalc(pd->workspace);
   pd->style->isModify = true;
}


#undef PROP_DATA
#undef PROP_DATA_GET
