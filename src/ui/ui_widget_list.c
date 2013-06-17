#include "ui_widget_list.h"

static Elm_Genlist_Item_Class *_itc_widget = NULL;
static Elm_Genlist_Item_Class *_itc_style = NULL;
static Elm_Genlist_Item_Class *_itc_group = NULL;
static Elm_Genlist_Item_Class *_itc_part = NULL;

static inline Elm_Object_Item *
_widget_list_get(Evas_Object *naviframe)
{
   Eina_List *items;
   Elm_Object_Item *item_gl_widgets;

   items = elm_naviframe_items_get(naviframe);
   item_gl_widgets = eina_list_data_get(eina_list_last(items));

   eina_list_free(items);
   return item_gl_widgets;
}


static char *
_item_part_label_get(void *data,
                     Evas_Object *obj __UNUSED__,
                     const char *part __UNUSED__)
{
   Part *p = (Part *)data;
   if (!p->name)
     {
        ERR("It impossible, but it is occurred, part name is missing!");
        return NULL;
     }
   return strdup(p->name);
}

static Evas_Object *
_item_part_content_get(void *data __UNUSED__,
                       Evas_Object *obj,
                       const char *part)
{
   Evas_Object *icon = elm_icon_add(obj);
   if (!strcmp(part, "elm.swallow.icon"))
     elm_image_file_set(icon, TET_IMG_PATH"eye_open.png", NULL);

   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   return icon;
}

static char *
_item_group_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   Group *g = (Group *)data;
   if (!g->group_name)
     {
        ERR("It impossible, but it is occurred, group name is missing!");
        return NULL;
     }
   return strdup(g->group_name);
}

static char *
_item_widget_label_get(void *data,
                       Evas_Object *obj __UNUSED__,
                       const char *part __UNUSED__)
{
   Widget *w = (Widget *)data;
   if (!w->widget_name)
     {
        ERR("It impossible, but it is occurred, widget name is missing!");
        return NULL;
     }
   return strdup(w->widget_name);
}

static char *
_item_style_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   Style *st = (Style *)data;
   if (!st->style_name)
     {
        ERR("It impossible, but it is occurred, style name is missing!");
        return NULL;
     }
   return strdup(st->style_name);
}

static void
_navi_gl_styles_pop(void *data,
                    Evas_Object *obj __UNUSED__,
                    void *event_info __UNUSED__)
{
   Evas_Object *nf = (Evas_Object *)data;
   elm_naviframe_item_pop(nf);
}

static void
_navi_gl_parts_pop(void *data,
                   Evas_Object *obj __UNUSED__,
                   void *event_info __UNUSED__)
{
   Evas_Object *nf = (Evas_Object *)data;
   elm_naviframe_item_pop(nf);

   evas_object_smart_callback_call (nf, "part,back", NULL);
}

static void
_on_part_select(void *data __UNUSED__,
                Evas_Object *obj,
                void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;
   Evas_Object *nf = elm_object_parent_widget_get(obj);
   evas_object_smart_callback_call (nf, "part,select", glit);
   elm_object_signal_emit(nf, "part,select", "part");
}

static void
_on_group_clicked_double(void *data,
                         Evas_Object *obj,
                         void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;
   Elm_Object_Item *eoi;
   Evas_Object *nf, *gl_parts, *bt, *ic;
   Project *project = (Project *)data;
   Eina_Inlist *parts;
   Group *_group;
   Part *_part;

   nf = elm_object_parent_widget_get(obj);
   _group = elm_object_item_data_get(glit);

   if (_group->__type != GROUP) return;
   if (!_group->obj)
     wm_group_data_load(_group, evas_object_evas_get(obj), project->swapfile);
   parts = _group->parts;

   evas_object_smart_callback_call (nf, "group,select", _group);

   if (!_itc_part)
     {
        _itc_part = elm_genlist_item_class_new();
        _itc_part->item_style = "default";
        _itc_part->func.text_get = _item_part_label_get;
        _itc_part->func.content_get = _item_part_content_get;
        _itc_part->func.state_get = NULL;
        _itc_part->func.del = NULL;
     }

   gl_parts = elm_genlist_add(nf);
   evas_object_size_hint_align_set(gl_parts,
                                   EVAS_HINT_FILL,
                                   EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(gl_parts,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);

   EINA_INLIST_FOREACH(parts, _part)
     {
        eoi = elm_genlist_item_append(gl_parts, _itc_part,
                                      _part,
                                      NULL, ELM_GENLIST_ITEM_NONE,
                                      NULL, NULL);
        elm_object_item_data_set(eoi, _part);
     }
   evas_object_smart_callback_add(gl_parts, "selected",
                                  _on_part_select, NULL);

   ic = elm_icon_add(nf);
   elm_icon_standard_set(ic, "arrow_left");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   bt = elm_button_add(nf);
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_content_set(bt, "icon", ic);
   evas_object_smart_callback_add(bt, "clicked", _navi_gl_parts_pop, nf);

   elm_naviframe_item_push(nf, _group->full_group_name, bt, NULL, gl_parts, NULL);

}

static void
_on_widget_clicked_double(void *data,
                          Evas_Object *obj,
                          void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;
   Elm_Object_Item *glit_style, *glit_group;
   Evas_Object *nf, *gl_styles, *bt, *ic;
   Project *project = (Project *)data;
   Eina_Inlist *styles, *groups;
   Widget *_widget;
   Style *_style;
   Group *_group;

   nf = elm_object_parent_widget_get(obj);
   _widget = elm_object_item_data_get(glit);
   styles = _widget->styles;

   if(!_itc_style)
     {
        _itc_style = elm_genlist_item_class_new();
        _itc_style->item_style = "default";
        _itc_style->func.text_get = _item_style_label_get;
        _itc_style->func.content_get = NULL;
        _itc_style->func.state_get = NULL;
        _itc_style->func.del = NULL;
     }

   if (!_itc_group)
     {
        _itc_group = elm_genlist_item_class_new();
        _itc_group->item_style = "default";
        _itc_group->func.text_get = _item_group_label_get;
        _itc_group->func.content_get = NULL;
        _itc_group->func.state_get = NULL;
        _itc_group->func.del = NULL;
     }

   gl_styles = elm_genlist_add(nf);
   evas_object_size_hint_align_set(gl_styles,
                                   EVAS_HINT_FILL,
                                   EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(gl_styles,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);

   EINA_INLIST_FOREACH(styles, _style)
     {
        glit_style = elm_genlist_item_append(gl_styles, _itc_style,
                                             _style,
                                             NULL, ELM_GENLIST_ITEM_NONE,
                                             NULL, NULL);
        groups = _style->groups;

        EINA_INLIST_FOREACH(groups, _group)
          {
             glit_group = elm_genlist_item_append(gl_styles, _itc_group,
                             _group, glit_style, ELM_GENLIST_ITEM_NONE,
                             NULL, NULL);
             elm_object_item_data_set(glit_group, _group);
          }
     }


   ic = elm_icon_add(nf);
   elm_icon_standard_set(ic, "arrow_left");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   bt = elm_button_add(nf);
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_content_set(bt, "icon", ic);
   elm_naviframe_item_push(nf, _widget->widget_name, bt, NULL, gl_styles, NULL);
   evas_object_smart_callback_add(bt, "clicked", _navi_gl_styles_pop, nf);
   evas_object_smart_callback_add(gl_styles, "clicked,double",
                                  _on_group_clicked_double, project);

}

Evas_Object *
ui_widget_list_add(Evas_Object *parent)
{
   Evas_Object *nf;
   Evas_Object *gl_widgets = NULL;
   Elm_Object_Item *it;

   if (!_itc_widget)
     {
        _itc_widget = elm_genlist_item_class_new();
        _itc_widget->item_style = "default";
        _itc_widget->func.text_get = _item_widget_label_get;
        _itc_widget->func.content_get = NULL;
        _itc_widget->func.state_get = NULL;
        _itc_widget->func.del = NULL;
     }

   nf = elm_naviframe_add(parent);
   evas_object_size_hint_align_set(nf, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(nf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   gl_widgets = elm_genlist_add(nf);
   evas_object_size_hint_align_set(gl_widgets,
                                   EVAS_HINT_FILL,
                                   EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(gl_widgets,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);

   it = elm_naviframe_item_push(nf, NULL, NULL, NULL, gl_widgets, NULL);
   elm_object_item_part_text_set(it, "subtitle", "Widget list");

   return nf;
}

void
ui_widget_list_title_set(Evas_Object *object, const char *title)
{
   Elm_Object_Item *item_gl_widgets;

   if (!object) return;

   item_gl_widgets = _widget_list_get(object);
   elm_object_item_part_text_set(item_gl_widgets, "elm.text.title", title);
}

Eina_Bool
ui_widget_list_data_set(Evas_Object *object, Project *project)
{
   Widget *_widget;
   Elm_Object_Item *eoi;
   Evas_Object *gl_widgets;
   Eina_Inlist *widget_list = project->widgets;

   if ((!object) || (!widget_list)) return EINA_FALSE;

   gl_widgets = elm_object_item_part_content_get(_widget_list_get(object),
                                                 "elm.swallow.content");

   EINA_INLIST_FOREACH(widget_list, _widget)
     {
        eoi = elm_genlist_item_append(gl_widgets, _itc_widget,
                                      _widget,
                                      NULL, ELM_GENLIST_ITEM_NONE,
                                      NULL, NULL);
        /* maybe add a ref to _widget */
        elm_object_item_data_set(eoi, _widget);
     }

   evas_object_smart_callback_add(gl_widgets, "clicked,double",
                                  _on_widget_clicked_double, project);

   return EINA_TRUE;
}

void
ui_widget_list_callback_add(Evas_Object *object,
                            const char *event,
                            Edje_Signal_Cb func,
                            void *data)
{
   if (!object) return;
   elm_object_signal_callback_add(object, event, "", func, data);
}
