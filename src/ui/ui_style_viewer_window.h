#ifndef UI_STYLE_VIEWER_WINDOW_HEADER_H
#define UI_STYLE_VIEWER_WINDOW_HEADER_H

/**
 * @defgroup TextStyle TextStyle
 *
 * TextStyle is a Inwin that contain information
 * of the widgets text styles.
 */


#include <Eina.h>
#include "efl_tet.h"

enum _type_style
{
	TAG,
	TEXTSTYLE
};

typedef enum _type_style type_style;

struct _Tag
{
	EINA_INLIST;
	char *tag_name;
	char *tag_value;
	type_style __type;
};

typedef struct _Tag Tag;

struct _TextStyles
{
	EINA_INLIST;
	char *style_name;
	Eina_Inlist *tags;
	type_style __type;
};

typedef struct _TextStyles TextStyles;

struct _Style_Window
{
	Evas_Object *entry_style;
	Evas_Object *entry_tag;
	Evas_Object *entry_prop;
};

typedef struct _Style_Window Style_Window;

/**
 * Show the whole inwin window by using some data
 * about loaded project (edj file)
 *
 * @param ap the application data, that contain some information
 * about loaded project
 *
 * @ingroup TextStyle
 */
void
style_viewer_init (App_Data *ap);

#endif	/* UI_STYLE_VIEWER_WINDOW_HEADER_H */

