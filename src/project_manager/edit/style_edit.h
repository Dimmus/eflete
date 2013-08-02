#ifndef STYLE_EDIT_H
#define STYLE_EDIT_H

#include "efl_tet.h"

/**
 *
 */
#define GET_OBJ(project, edje_object) \
   Eina_Inlist *__styles, *__groups, *__widgets = NULL; \
   Widget *__widget; \
   Style *__style; \
   Group *__group; \
   __widgets = project->widgets; \
   if (!__widgets) edje_object = NULL; \
   else\
     { \
         __widget = EINA_INLIST_CONTAINER_GET(__widgets, Widget); \
         __styles = __widget->styles; \
         if (!__styles) edje_object = NULL; \
         else \
           { \
               __style = EINA_INLIST_CONTAINER_GET(__styles, Style); \
               __groups = __style->groups; \
               if (!__groups) edje_object = NULL; \
               else\
                 { \
                     __group = EINA_INLIST_CONTAINER_GET(__groups, Group); \
                     edje_object = __group->obj; \
                 } \
           } \
     }

/**
 * Get the list of all the text styles in the given project.
 *
 * @param project Project being editing.
 * @return List of strings, each being the name for a text style.
 *
 * @ingroup TextStyle
 */
Eina_List *
style_edit_styles_list_get(Project *project);

/**
 * Create a new text style object in the given project.
 * If another style with the same name exists nothing is created and EINA_FALSE
 * is returned.
 *
 * @param project Project being editing.
 * @param style Name for the new style.
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 *
 * @ingroup TextStyle
 */
Eina_Bool
style_edit_style_add(Project *project, const char *style);

/**
 * Delete the given text style and all the child tags.
 *
 * @param project Project being editing.
 * @param style Style to delete.
 *
 * @ingroup TextStyle
 */
void
style_edit_style_del(Project *project, const char *style);

/**
 * Get the list of all the tags name in the given text style.
 *
 * @param project Project being editing.
 * @param style Style to get the tags for.
 * @return List of strings, each being one tag in the given style.
 *
 * @ingroup TextStyle
 */
Eina_List *
style_edit_style_tags_list_get(Project *project, const char *style);

/**
 * Add a new tag to the given text style.
 * If another tag with the same name exists nothing is created and EINA_FALSE
 * is returned.
 *
 * @param project Project being editing.
 * @param style Style where to add the new tag.
 * @param tag_name Name for the new tag.
 * @param tag_value Value for the tag.
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 *
 * @ingroup TextStyle
 */
Eina_Bool
style_edit_style_tag_add(Project *project,
                         const char *style,
                         const char *tag_name,
                         const char *tag_value);

/**
 * Delete the given tag.
 *
 * @param project Project being editing.
 * @param style Style from where to remove the tag.
 * @param tag_name Tag to delete.
 *
 * @ingroup TextStyle
 */
void
style_edit_style_tag_del(Project *project,
                         const char *style,
                         const char *tag_name);

/**
 * Get the value of the given tag.
 *
 * @param project Project being editing.
 * @param style Style containing the tag being.
 * @param tag_name Tag to get the value for.
 * @return Value of the given tag.
 *
 * @ingroup TextStyle
 */
const char*
style_edit_style_tag_value_get(Project *project,
                               const char *style,
                               const char *tag_name);

/**
 * Set the value of the given tag.
 *
 * @param project Project being editing.
 * @param style Style containing the tag to change.
 * @param tag_name Name of the tag to set the value for.
 * @param new_value Value for the tag.
 *
 * @ingroup TextStyle
 */
void
style_edit_style_tag_value_set(Project *project,
                               const char* style,
                               const char* tag_name,
                               const char* new_value);

/**
 * Save the modified styles back to his file.
 *
 * @param project Project being saving.
 * @return EINA_TRUE if successful, EINA_FALSE otherwise.
 *
 * @ingroup TextStyle
 */
Eina_Bool
style_edit_save(Project *project);

#endif /* STYLE_EDIT_H */
