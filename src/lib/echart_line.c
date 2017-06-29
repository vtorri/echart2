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

#define PAD(v_) (sd->padding + (v_))
#define PAD2(v_) (2 * sd->padding + (v_))

typedef struct
{
    EINA_REFCOUNT;

    const Echart_Chart *chart;
    Evas_Object *bg;
    Evas_Object *title;
    Eina_Inarray *ord_val;
    Evas_Object *vg;
    Efl_VG *root;
    Efl_VG *dot;
    Evas_Coord w_vg;
    Evas_Coord h_vg;
    Evas_Coord padding;
} Echart_Smart_Data;

static Evas_Smart *_echart_line_smart = NULL;

static void
_echart_line_offsets_get(Echart_Smart_Data *sd,
                         Evas_Coord *offset_left,
                         Evas_Coord *offset_right,
                         Evas_Coord *offset_top,
                         Evas_Coord *offset_bottom)
{
    *offset_left = 10;
    *offset_right = 10;
    evas_object_geometry_get(sd->title, NULL, NULL, NULL, offset_top);
    *offset_top += 10;
    *offset_bottom = 10;
}

static void
_echart_line_coords_get(Echart_Smart_Data *sd,
                        double xmin, double xmax,
                        double ymin, double ymax,
                        double x, double y,
                        Evas_Coord *xvg, Evas_Coord *yvg)
{
    *xvg = sd->padding + (sd->w_vg - 2 * sd->padding) * (x - xmin) / (xmax - xmin);
    *yvg = sd->padding + (sd->h_vg - 2 * sd->padding) * (ymax - y) / (ymax - ymin);
}

static void
_echart_line_mouse_move_cb(void *d, Evas *evas EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event)
{
    Echart_Smart_Data *sd;
    Evas_Event_Mouse_Move *ev;
    const Echart_Data *data;
    const Echart_Serie *absciss;
    const Echart_Serie *serie;
    const Eina_List *series;
    const Eina_List *l;
    const Eina_Inarray *x_values;
    Echart_Colors cols;
    Evas_Coord offset_left;
    Evas_Coord offset_right;
    Evas_Coord offset_top;
    Evas_Coord offset_bottom;
    Evas_Coord xd;
    Evas_Coord yd;
    double *xv;
    double ymin;
    double ymax;
    int n;
    Eina_Bool has_dot;

    sd = d;
    ev = event;

    _echart_line_offsets_get(sd,
                             &offset_left, &offset_right,
                             &offset_top, &offset_bottom);

    data = echart_chart_data_get(sd->chart);
    absciss = echart_data_absciss_get(data);
    series = echart_data_series_get(data);

    x_values = echart_serie_values_get(absciss);
    xv = (double *)x_values->members;

    echart_data_interval_get(data, &ymin, &ymax);
    n = (int)floor(log(ymax - ymin) / log(10));
    ymax = (floor(ymax / pow(10, n - 1)) + 1) * pow(10, n - 1);

    has_dot = EINA_FALSE;

    EINA_LIST_FOREACH(series, l, serie)
    {
        const Eina_Inarray *y_values;
        double *yv;
        Evas_Coord x;
        Evas_Coord y;
        size_t i;

        y_values = echart_serie_values_get(serie);
        yv = (double *)y_values->members;

        for (i = 0; i < x_values->len; i++)
        {
            _echart_line_coords_get(sd,
                                    xv[0], xv[x_values->len - 1],
                                    ymin, ymax,
                                    xv[i], yv[i],
                                    &x, &y);
            if ((ev->cur.canvas.x >= (x + offset_left - 3)) &&
                (ev->cur.canvas.x <= (x + offset_left + 3)) &&
                (ev->cur.canvas.y >= (y + offset_top - 3)) &&
                (ev->cur.canvas.y <= (y + offset_top + 3)))
            {
                has_dot = EINA_TRUE;
                xd = x;
                yd = y;
                cols = echart_serie_color_get(serie);
            }

        }
    }

    if (has_dot)
    {
        if (!sd->dot)
        {
            sd->dot = evas_vg_shape_add(sd->root);
            evas_vg_shape_append_circle(sd->dot, xd, yd, 5);
            evas_vg_node_color_set(sd->dot,
                                   COL_TO_R(cols.line),
                                   COL_TO_G(cols.line),
                                   COL_TO_B(cols.line),
                                   COL_TO_A(cols.line));
        }
    }
    else
    {
        evas_vg_shape_reset(sd->dot);
        sd->dot = NULL;
    }
}

static void
_echart_text_object_set(Evas_Object *obj,
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
    sd->root = evas_object_vg_root_node_get(sd->vg);

    evas_object_event_callback_add(sd->vg, EVAS_CALLBACK_MOUSE_MOVE,
                                   _echart_line_mouse_move_cb, sd);

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
    Evas_Coord ox;
    Evas_Coord oy;
    Evas_Coord dx;
    Evas_Coord dy;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_geometry_get(obj, &ox, &oy, NULL, NULL);
    dx = x - ox;
    dy = y - oy;

    evas_object_geometry_get(sd->bg, &ox, &oy, NULL, NULL);
    evas_object_move(sd->bg, ox + dx, oy + dy);
    evas_object_geometry_get(sd->title, &ox, &oy, NULL, NULL);
    evas_object_move(sd->title, ox + dx, oy + dy);
    evas_object_geometry_get(sd->vg, &ox, &oy, NULL, NULL);
    evas_object_move(sd->vg, ox + dx, oy + dy);
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
    Efl_VG *line;
    double *xv;
    double ymin;
    double ymax;
    int gyn;
    int w;
    int h;
    int n;
    int i;
    Evas_Coord offset_left;
    Evas_Coord offset_right;
    Evas_Coord offset_top;
    Evas_Coord offset_bottom;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    sd->padding = 5;

    /* background */
    echart_chart_size_get(sd->chart, &w, &h);
    col = echart_chart_background_color_get(sd->chart);

    evas_object_color_set(sd->bg, COL_TO_R(col), COL_TO_G(col), COL_TO_B(col), COL_TO_A(col));
    evas_object_resize(sd->bg, w, h);

    /* title */
    if (echart_chart_title_get(sd->chart))
    {
        Evas_Coord w_title;

        echart_chart_title_style_get(sd->chart, &fs);
        _echart_text_object_set(sd->title,
                                echart_chart_title_get(sd->chart),
                                &fs);
        evas_object_geometry_get(sd->title, NULL, NULL, &w_title, NULL);
        evas_object_move(sd->title, (w - w_title) / 2, 0);
    }

    /* echart_chart_grid_nbr_get(sd->chart, NULL, &gyn); */
    /* if (gyn > 0) */
    /* { */
    /* } */

    _echart_line_offsets_get(sd,
                             &offset_left, &offset_right,
                             &offset_top, &offset_bottom);

    /* vg */
    sd->w_vg = w - (offset_left + offset_right);
    sd->h_vg = h - (offset_top + offset_bottom);
    evas_object_resize(sd->vg, sd->w_vg, sd->h_vg);
    evas_object_move(sd->vg, offset_left, offset_top);

    /* axis */
    line = evas_vg_shape_add(sd->root);
    evas_vg_shape_append_move_to(line, PAD(0.5), PAD(0.5));
    evas_vg_shape_append_line_to(line, PAD(0.5), sd->h_vg - PAD(0.5));
    evas_vg_shape_append_line_to(line, sd->w_vg - PAD(0.5), sd->h_vg - PAD(0.5));
    evas_vg_shape_stroke_width_set(line, 1);
    evas_vg_shape_stroke_color_set(line, 0, 0, 0, 255);

    /* lines */
    data = echart_chart_data_get(sd->chart);
    absciss = echart_data_absciss_get(data);
    x_values = echart_serie_values_get(absciss);
    xv = (double *)x_values->members;
    series = echart_data_series_get(data);

    echart_data_interval_get(data, &ymin, &ymax);
    n = (int)floor(log(ymax - ymin) / log(10));
    ymax = (floor(ymax / pow(10, n - 1)) + 1) * pow(10, n - 1);
    col = echart_chart_grid_color_get(sd->chart);

    echart_chart_grid_nbr_get(sd->chart, NULL, &gyn);
    if (gyn > 0)
    {
        for (i = 1; i <= gyn; i++)
        {
            double y = i * (ymax - ymin) / gyn + ymin;
            int j = (ymax - y) * (sd->h_vg - 1) / (ymax - ymin);
            line = evas_vg_shape_add(sd->root);
            evas_vg_shape_append_move_to(line, PAD(0.5), j + PAD(0.5));
            evas_vg_shape_append_line_to(line, sd->w_vg - PAD2(0.5), j + PAD(0.5));
            evas_vg_shape_stroke_width_set(line, 1);
            evas_vg_shape_stroke_color_set(line,
                                           COL_TO_R(col), COL_TO_G(col), COL_TO_B(col), COL_TO_A(col));
        }
    }

    EINA_LIST_FOREACH(series, l, serie)
    {
        const Eina_Inarray *y_values;
        Efl_VG *line_area;
        Echart_Colors cols;
        double *yv;
        Evas_Coord x;
        Evas_Coord y;

        y_values = echart_serie_values_get(serie);
        yv = (double *)y_values->members;
        cols = echart_serie_color_get(serie);

        line = evas_vg_shape_add(sd->root);
        _echart_line_coords_get(sd,
                                xv[0], xv[x_values->len - 1],
                                ymin, ymax,
                                xv[0], yv[0],
                                &x, &y);
        evas_vg_shape_append_move_to(line, x + 1, y);
        if (echart_data_area_get(data))
        {
            line_area = evas_vg_shape_add(sd->root);
            evas_vg_shape_append_move_to(line_area, x + 1, y);
        }
        for (i = 1; i < (int)x_values->len; i++)
        {
            _echart_line_coords_get(sd,
                                    xv[0], xv[x_values->len - 1],
                                    ymin, ymax,
                                    xv[i], yv[i],
                                    &x, &y);
            evas_vg_shape_append_line_to(line, x, y);
            if (echart_data_area_get(data))
            {
                evas_vg_shape_append_line_to(line_area, x, y);
            }
        }
        evas_vg_shape_stroke_width_set(line, 2);
        evas_vg_shape_stroke_color_set(line,
                                       COL_TO_R(cols.line),
                                       COL_TO_G(cols.line),
                                       COL_TO_B(cols.line),
                                       COL_TO_A(cols.line));
        if (echart_data_area_get(data))
        {
            int a, r, g, b;

            evas_vg_shape_append_line_to(line_area, sd->w_vg - sd->padding, sd->h_vg - sd->padding - 1);
            evas_vg_shape_append_line_to(line_area, sd->padding + 1, sd->h_vg - sd->padding - 1);
            evas_vg_shape_append_close(line_area);
            a = 255 * echart_serie_opacity_get(serie);
            r = ((COL_TO_R(cols.area) * a) >> 8);
            g = ((COL_TO_G(cols.area) * a) >> 8);
            b = ((COL_TO_B(cols.area) * a) >> 8);
            evas_vg_node_color_set(line_area, r, g, b, a);
        }
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
