/* Echart - Chart rendering library
 * Copyright (C) 2017 Vincent Torri
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef EFL_BETA_API_SUPPORT
#define EFL_BETA_API_SUPPORT 1
#endif

#include <Eina.h>
#include <Evas.h>

#include "Echart.h"
#include "echart_private.h"
#include "echart_data.h"
#include "echart_chart.h"
#include "echart_line.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

#define ECHART_LINE_SMART_OBJ_GET(smart, o, type) \
{ \
    char *_echart_line_smart_str; \
    if (!o) return; \
    smart = evas_object_smart_data_get(o); \
    if (!smart) return; \
    _echart_line_smart_str = (char *)evas_object_type_get(o); \
    if (!_echart_line_smart_str) return; \
    if (strcmp(_echart_line_smart_str, type)) return; \
}

#define ECHART_LINE_SMART_OBJ_GET_ERROR(smart, o, type) \
{ \
    char *_echart_line_smart_str; \
    if (!o) goto _err; \
    smart = evas_object_smart_data_get(o); \
    if (!smart) goto _err; \
    _echart_line_smart_str = (char *)evas_object_type_get(o); \
    if (!_echart_line_smart_str) goto _err; \
    if (strcmp(_echart_line_smart_str, type)) goto _err; \
}

#define ECHART_LINE_OBJ_NAME "echart_line_object"
#define COL_TO_A(col_) (((col_) >> 24) & 0xff)
#define COL_TO_R(col_) (((col_) >> 16) & 0xff)
#define COL_TO_G(col_) (((col_) >> 8 ) & 0xff)
#define COL_TO_B(col_) (((col_)      ) & 0xff)

typedef struct
{
    EINA_REFCOUNT;

    const Echart_Chart *chart;
    Evas_Object *bg;
    Evas_Object *title;
    Evas_Object *vg;
} Echart_Smart_Data;

static Evas_Smart *_echart_line_smart = NULL;

static Evas_Object *
echart_text_object_add(Evas *evas,
                       const char *text,
                       const Echart_Font_Style *fs)
{
    char buf[256];
    char buf2[32];
    Evas_Object *o;

    if (!text)
        return NULL;

    *buf2 = '\0';
    if (fs->bold || fs->italic)
    {
        memcpy(buf2, ":style=", 7);
        if (!fs->italic)
            memcpy(buf2 + 7, "Bold", 5);
        else if (!fs->bold)
            memcpy(buf2 + 7, "Italic", 7);
        else
            memcpy(buf2 + 7, "Bold Italic", 12);
    }
    snprintf(buf, sizeof(buf), "%s%s",
             fs->font_name ? fs->font_name : "Sans", buf2);
    buf[255] = '\0';
    o = evas_object_text_add(evas);
    evas_object_text_style_set(o, EVAS_TEXT_STYLE_PLAIN);
    evas_object_color_set(o, COL_TO_R(fs->font_color), COL_TO_G(fs->font_color), COL_TO_B(fs->font_color), COL_TO_A(fs->font_color));
    evas_object_text_font_set(o, buf, (fs->font_size <= 0) ? 13 : fs->font_size);
    evas_object_text_text_set(o, text);

    return o;
}

static void
_echart_line_smart_add(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    sd = calloc(1, sizeof(Echart_Smart_Data));
    EINA_SAFETY_ON_NULL_RETURN(sd);

    EINA_REFCOUNT_INIT(sd);

    evas_object_smart_data_set(obj, sd);
}

static void
_echart_line_smart_del(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    EINA_REFCOUNT_UNREF(sd)
    {
        evas_object_del(sd->bg);
        evas_object_del(sd->title);
        evas_object_del(sd->vg);
        free(sd);
    }
}

static void
_echart_line_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);
    evas_object_move(sd->bg, x, y);
    evas_object_move(sd->title, x, y);
    evas_object_move(sd->vg, x, y);
}

static void
_echart_line_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    /* FIXME: should do something here ? */

    fprintf(stderr, " %s 2 : %dx%d\n", __FUNCTION__, w, h);
}

static void
_echart_line_smart_show(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);
    evas_object_show(sd->bg);
    evas_object_show(sd->title);
    evas_object_show(sd->vg);

}

static void
_echart_line_smart_hide(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);
    evas_object_hide(sd->bg);
    evas_object_hide(sd->title);
    evas_object_hide(sd->vg);
}

static void
_echart_line_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);
    evas_object_clip_set(sd->bg, clip);
    evas_object_clip_set(sd->title, clip);
    evas_object_clip_set(sd->vg, clip);
}

static void
_echart_line_smart_clip_unset(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);
    evas_object_clip_unset(sd->bg);
    evas_object_clip_unset(sd->title);
    evas_object_clip_unset(sd->vg);
}


static void
_echart_line_smart_calculate(Evas_Object *obj)
{
    Echart_Font_Style fs;
    Echart_Smart_Data *sd;
    Evas *evas;
    Evas_Object *o;
    Efl_VG *root;
    Efl_VG *line;
    unsigned int col;
    int w;
    int h;
    Evas_Coord w_title = 0;
    Evas_Coord h_title = 0;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas = evas_object_evas_get(obj);

    echart_chart_size_get(sd->chart, &w, &h);
    col = echart_chart_background_color_get(sd->chart);

    o = evas_object_rectangle_add(evas);
    evas_object_color_set(o, COL_TO_R(col), COL_TO_G(col), COL_TO_B(col), COL_TO_A(col));
    evas_object_move(o, 0, 0);
    evas_object_resize(o, w, h);
    sd->bg = o;

    echart_chart_title_style_get(sd->chart, &fs);
    o = echart_text_object_add(evas,
                               echart_chart_title_get(sd->chart), &fs);
    evas_object_geometry_get(o, NULL, NULL, &w_title, &h_title);
    evas_object_move(o, (w - w_title) / 2, 0);
    sd->title = o;

    o = evas_object_vg_add(evas);
    evas_object_resize(o, w, h);
    evas_object_move(o, 0, 0);
    evas_object_show(o);

    root = evas_object_vg_root_node_get(o);

    line = evas_vg_shape_add(root);
    evas_vg_shape_append_move_to(line, 10, h - 10);
    evas_vg_shape_append_line_to(line, w - 10, h - 10);
    evas_vg_shape_stroke_width_set(line, 1);
    evas_vg_shape_stroke_color_set(line, 0, 0, 0, 255);
    sd->vg = o;
}

static void
_echart_line_smart_init(void)
{
    static Evas_Smart_Class sc = EVAS_SMART_CLASS_INIT_NAME_VERSION(ECHART_LINE_OBJ_NAME);

    if (_echart_line_smart) return;

    if (!sc.add)
    {
        sc.add = _echart_line_smart_add;
        sc.del = _echart_line_smart_del;
        sc.move = _echart_line_smart_move;
        sc.resize = _echart_line_smart_resize;
        sc.show = _echart_line_smart_show;
        sc.hide = _echart_line_smart_hide;
        sc.clip_set = _echart_line_smart_clip_set;
        sc.clip_unset = _echart_line_smart_clip_unset;
/*         sc.callbacks = _echart_line_smart_callbacks; */
        sc.calculate = _echart_line_smart_calculate;
    }
    _echart_line_smart = evas_smart_class_new(&sc);
}

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Evas_Object *
echart_line_object_add(Evas *evas)
{
    Evas_Object *obj;

    _echart_line_smart_init();
    obj = evas_object_smart_add(evas, _echart_line_smart);

    return obj;
}

EAPI void
echart_line_object_chart_set(Evas_Object *obj, const Echart_Chart *chart)
{
    Echart_Smart_Data *sd;

    ECHART_LINE_SMART_OBJ_GET(sd, obj, ECHART_LINE_OBJ_NAME);
    INF("chart set");

    sd->chart = chart;
}
