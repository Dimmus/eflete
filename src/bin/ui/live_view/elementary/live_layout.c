#include "live_elementary_widgets.h"

Evas_Object *
widget_layout_create(Evas_Object *parent, const Group2 *group)
{
   assert(parent != NULL);
   assert(group != NULL);
   assert(group->style != NULL);
   assert(group->class != NULL);

   Evas_Object *object = elm_layout_add(parent);
   elm_layout_theme_set(object, "layout", group->class, group->style);

   evas_object_smart_callback_add(object, SIGNAL_DEMO_SWALLOW_SET, on_swallow_check, NULL);
   evas_object_smart_callback_add(object, SIGNAL_DEMO_TEXT_SET, on_text_check, NULL);
   evas_object_smart_callback_add(object, SIGNAL_DEMO_SIGNAL_SEND, send_signal, NULL);
   evas_object_clip_set(object, evas_object_clip_get(parent));

   return object;
}
