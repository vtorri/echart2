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

static void
echart_text_object_set(Evas_Object *obj,
                       const char *text,
                       const Echart_Font_Style *fs)
{
    char buf[256];
    char buf2[32];

    if (!text)
        return;

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

    evas_object_text_style_set(obj, EVAS_TEXT_STYLE_PLAIN);
    evas_object_color_set(obj,
                          COL_TO_R(fs->font_color),
                          COL_TO_G(fs->font_color),
                          COL_TO_B(fs->font_color),
                          COL_TO_A(fs->font_color));
    evas_object_text_font_set(obj, buf, (fs->font_size <= 0) ? 13 : fs->font_size);
    evas_object_text_text_set(obj, text);
}

static void
_echart_line_smart_add(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = calloc(1, sizeof(Echart_Smart_Data));
    EINA_SAFETY_ON_NULL_RETURN(sd);

    EINA_REFCOUNT_INIT(sd);

    sd->bg = evas_object_rectangle_add(evas_object_evas_get(obj));
    evas_object_move(sd->bg, 0, 0);
    sd->title = evas_object_text_add(evas_object_evas_get(obj));
    sd->vg = evas_object_vg_add(evas_object_evas_get(obj));

    evas_object_smart_data_set(obj, sd);
}

static void
_echart_line_smart_del(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

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

    fprintf(stderr, " ** %s\n", __FUNCTION__);

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

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    /* FIXME: should do something here ? */

    fprintf(stderr, " %s 2 : %dx%d\n", __FUNCTION__, w, h);
}

static void
_echart_line_smart_show(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

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

    fprintf(stderr, " ** %s\n", __FUNCTION__);

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

    fprintf(stderr, " ** %s\n", __FUNCTION__);

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

    fprintf(stderr, " ** %s\n", __FUNCTION__);

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
    const Echart_Data *data;
    const Echart_Serie *absciss;
    const Echart_Serie *serie;
    const Eina_List *series;
    const Eina_Inarray *x_values;
    const Eina_List *l;
    Echart_Smart_Data *sd;
    unsigned int col;
    Efl_VG *root;
    Efl_VG *line;
    double *xv;
    double ymin;
    double ymax;
    int gxn;
    int gyn;
    int w;
    int h;
    int n;
    int i;
    Evas_Coord w_title = 0;
    Evas_Coord h_title = 0;
    Evas_Coord w_vg = 0;
    Evas_Coord h_vg = 0;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    /* background */
    echart_chart_size_get(sd->chart, &w, &h);
    col = echart_chart_background_color_get(sd->chart);

    evas_object_color_set(sd->bg, COL_TO_R(col), COL_TO_G(col), COL_TO_B(col), COL_TO_A(col));
    evas_object_resize(sd->bg, w, h);

    /* title */
    if (echart_chart_title_get(sd->chart))
    {
        echart_chart_title_style_get(sd->chart, &fs);
        echart_text_object_set(sd->title,
                               echart_chart_title_get(sd->chart),
                               &fs);
        evas_object_geometry_get(sd->title, NULL, NULL, &w_title, &h_title);
        evas_object_move(sd->title, (w - w_title) / 2, 0);
    }

    /* vg */
    w_vg = w - 20;
    h_vg = h - (20 + h_title);
    evas_object_resize(sd->vg, w_vg, h_vg);
    evas_object_move(sd->vg, 10, h_title + 10);

    root = evas_object_vg_root_node_get(sd->vg);

    /* axis */
    line = evas_vg_shape_add(root);
    evas_vg_shape_append_move_to(line, 1, 1);
    evas_vg_shape_append_line_to(line, 1, h_vg - 1);
    evas_vg_shape_append_line_to(line, w_vg - 1, h_vg - 1);
    evas_vg_shape_stroke_width_set(line, 1);
    evas_vg_shape_stroke_color_set(line, 0, 0, 0, 255);

    echart_chart_grid_nbr_get(sd->chart, &gxn, &gyn);

    /* lines */
    data = echart_chart_data_get(sd->chart);
    absciss = echart_data_absciss_get(data);
    x_values = echart_serie_values_get(absciss);
    xv = (double *)x_values->members;
    series = echart_data_series_get(data);

    echart_data_interval_get(data, &ymin, &ymax);
    n = (int)floor(log(ymax - ymin) / log(10));
    ymax = (floor(ymax / pow(10, n - 1)) + 1) * pow(10, n - 1);

    for (i = 1; i <= gyn; i++)
    {
        double y = i * (ymax - ymin) / gyn + ymin;
        int j = (ymax - y) * (h_vg - 1) / (ymax - ymin);
        line = evas_vg_shape_add(root);
        evas_vg_shape_append_move_to(line, 0, j+1);
        evas_vg_shape_append_line_to(line, w_vg - 1, j+1);
        evas_vg_shape_stroke_width_set(line, 1);
        evas_vg_shape_stroke_color_set(line, 0, 0, 0, 255);
    }

    EINA_LIST_FOREACH(series, l, serie)
    {
        const Eina_Inarray *y_values;
        Echart_Colors cols;
        double *yv;

        y_values = echart_serie_values_get(serie);
        yv = (double *)y_values->members;
        line = evas_vg_shape_add(root);
        cols = echart_serie_color_get(serie);

        /* if (echart_data_area_get(data)) */
        /* { */
        /*     evas_vg_shape_append_move_to(line, */
        /*                                  1, h_vg * (ymax - yv[0]) / (ymax -  ymin)); */
        /*     for (i = 1; i < x_values->len; i++) */
        /*     { */
        /*         evas_vg_shape_append_line_to(line, */
        /*                                      w_vg * (xv[i] - xv[0]) / (xv[x_values->len - 1] - xv[0]), */
        /*                                      h_vg * (ymax - yv[i]) / (ymax -  ymin)); */
        /*     } */
        /*     evas_vg_shape_append_line_to(line, w_vg, h_vg - 1); */
        /*     evas_vg_shape_append_line_to(line, 1, h_vg - 1); */
        /*     evas_vg_shape_stroke_width_set(line, 2); */
        /*     evas_vg_shape_stroke_color_set(line, */
        /*                                    COL_TO_R(cols.area), */
        /*                                    COL_TO_G(cols.area), */
        /*                                    COL_TO_B(cols.area), */
        /*                                    COL_TO_A(cols.area)); */
        /* } */

        evas_vg_shape_append_move_to(line,
                                     0, h_vg * (ymax - yv[0]) / (ymax -  ymin));
        for (i = 1; i < (int)x_values->len; i++)
        {
            evas_vg_shape_append_line_to(line,
                                         w_vg * (xv[i] - xv[0]) / (xv[x_values->len - 1] - xv[0]),
                                         h_vg * (ymax - yv[i]) / (ymax -  ymin));
        }
        evas_vg_shape_stroke_width_set(line, 2);
        evas_vg_shape_stroke_color_set(line,
                                       COL_TO_R(cols.line),
                                       COL_TO_G(cols.line),
                                       COL_TO_B(cols.line),
                                       COL_TO_A(cols.line));
    }
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
    evas_object_smart_need_recalculate_set(obj, EINA_TRUE);
}
