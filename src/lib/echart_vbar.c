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
#include "echart_common.h"
#include "echart_vbar.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

#define ECHART_VBAR_SMART_OBJ_GET(smart, o, type) \
{ \
    char *_echart_vbar_smart_str; \
    if (!o) return; \
    smart = evas_object_smart_data_get(o); \
    if (!smart) return; \
    _echart_vbar_smart_str = (char *)evas_object_type_get(o); \
    if (!_echart_vbar_smart_str) return; \
    if (strcmp(_echart_vbar_smart_str, type)) return; \
}

#define ECHART_VBAR_SMART_OBJ_GET_ERROR(smart, o, type) \
{ \
    char *_echart_vbar_smart_str; \
    if (!o) goto _err; \
    smart = evas_object_smart_data_get(o); \
    if (!smart) goto _err; \
    _echart_vbar_smart_str = (char *)evas_object_type_get(o); \
    if (!_echart_vbar_smart_str) goto _err; \
    if (strcmp(_echart_vbar_smart_str, type)) goto _err; \
}

#define ECHART_VBAR_OBJ_NAME "echart_vbar_object"

typedef struct
{
    Echart_Smart_Common common;
    double group_width;
} Echart_Smart_Data;

static Evas_Smart *_echart_vbar_smart = NULL;

static void
_echart_vbar_coords_get(const Echart_Smart_Data *sd,
                        double xmin, double xmax,
                        double ymin, double ymax,
                        double x, double y,
                        Evas_Coord *xvg, Evas_Coord *yvg)
{
    *xvg = sd->common.padding + (sd->common.w_vg - 2 * sd->common.padding) * (x - xmin) / (xmax - xmin);
    *yvg = sd->common.padding + (sd->common.h_vg - 2 * sd->common.padding) * (ymax - y) / (ymax - ymin);
}

static void
_echart_vbar_mouse_move_cb(void *d, Evas *evas EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event)
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
    Echart_Offsets offsets;
    Evas_Coord xd;
    Evas_Coord yd;
    double *xv;
    double ymin;
    double ymax;
    int n;

    sd = d;
    ev = event;

    echart_offsets_get(sd->common.title, &offsets);

    data = echart_chart_data_get(sd->common.chart);
    absciss = echart_data_absciss_get(data);
    series = echart_data_series_get(data);

    x_values = echart_serie_values_get(absciss);
    xv = (double *)x_values->members;

    echart_data_interval_get(data, &ymin, &ymax);
    n = (int)floor(log(ymax - ymin) / log(10));
    ymax = (floor(ymax / pow(10, n - 1)) + 1) * pow(10, n - 1);


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
            _echart_vbar_coords_get(sd,
                                    xv[0], xv[x_values->len - 1],
                                    ymin, ymax,
                                    xv[i], yv[i],
                                    &x, &y);
            if ((ev->cur.canvas.x >= (x + offsets.left - 3)) &&
                (ev->cur.canvas.x <= (x + offsets.left + 3)) &&
                (ev->cur.canvas.y >= (y + offsets.top - 3)) &&
                (ev->cur.canvas.y <= (y + offsets.top + 3)))
            {
            }

        }
    }

    /* if (has_dot) */
    /* { */
    /*     if (!sd->dot) */
    /*     { */
    /*         sd->dot = evas_vg_shape_add(sd->root); */
    /*         evas_vg_shape_append_circle(sd->dot, xd, yd, 5); */
    /*         evas_vg_node_color_set(sd->dot, */
    /*                                COL_TO_R(cols.line), */
    /*                                COL_TO_G(cols.line), */
    /*                                COL_TO_B(cols.line), */
    /*                                COL_TO_A(cols.line)); */
    /*     } */
    /* } */
    /* else */
    /* { */
    /*     evas_vg_shape_reset(sd->dot); */
    /*     sd->dot = NULL; */
    /* } */
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
_echart_vbar_smart_add(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = calloc(1, sizeof(Echart_Smart_Data));
    EINA_SAFETY_ON_NULL_RETURN(sd);

    EINA_REFCOUNT_INIT((Echart_Smart_Common *)sd);

    sd->common.bg = evas_object_rectangle_add(evas_object_evas_get(obj));
    sd->common.title = evas_object_text_add(evas_object_evas_get(obj));
    sd->common.vg = evas_object_vg_add(evas_object_evas_get(obj));
    sd->common.root = evas_object_vg_root_node_get(sd->common.vg);

    sd->group_width = 0.6180339887; /* golden number^-1 */

    evas_object_event_callback_add(sd->common.vg, EVAS_CALLBACK_MOUSE_MOVE,
                                   _echart_vbar_mouse_move_cb, sd);

    evas_object_smart_data_set(obj, sd);
}

static void
_echart_vbar_smart_del(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    EINA_REFCOUNT_UNREF((Echart_Smart_Common *)sd)
    {
        evas_object_del(sd->common.bg);
        evas_object_del(sd->common.title);
        evas_object_del(sd->common.vg);
        free(sd);
    }
}

static void
_echart_vbar_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
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

    evas_object_geometry_get(sd->common.bg, &ox, &oy, NULL, NULL);
    evas_object_move(sd->common.bg, ox + dx, oy + dy);
    evas_object_geometry_get(sd->common.title, &ox, &oy, NULL, NULL);
    evas_object_move(sd->common.title, ox + dx, oy + dy);
    evas_object_geometry_get(sd->common.vg, &ox, &oy, NULL, NULL);
    evas_object_move(sd->common.vg, ox + dx, oy + dy);
}

static void
_echart_vbar_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    /* FIXME: manage resize */

    fprintf(stderr, " %s 2 : %dx%d\n", __FUNCTION__, w, h);
}

static void
_echart_vbar_smart_show(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_show(sd->common.bg);
    evas_object_show(sd->common.title);
    evas_object_show(sd->common.vg);

}

static void
_echart_vbar_smart_hide(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_hide(sd->common.bg);
    evas_object_hide(sd->common.title);
    evas_object_hide(sd->common.vg);
}

static void
_echart_vbar_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_clip_set(sd->common.bg, clip);
    evas_object_clip_set(sd->common.title, clip);
    evas_object_clip_set(sd->common.vg, clip);
}

static void
_echart_vbar_smart_clip_unset(Evas_Object *obj)
{
    Echart_Smart_Data *sd;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_clip_unset(sd->common.bg);
    evas_object_clip_unset(sd->common.title);
    evas_object_clip_unset(sd->common.vg);
}

static void
_echart_vbar_add(const Echart_Smart_Data *sd,
                 const Eina_List *series,
                 unsigned int i,
                 unsigned int idx,
                 double xmin,
                 double xmax,
                 double ymin,
                 double ymax,
                 double x,
                 double L)
{
    Evas_VG *r;
    const Eina_Inarray *y_values;
    const Echart_Serie *serie;
    double *yv;
    Echart_Colors cols;
    Evas_Coord xc;
    Evas_Coord yc;

    serie = (const Echart_Serie *)eina_list_nth(series, idx);
    y_values = echart_serie_values_get(serie);
    yv = (double *)y_values->members;
    cols = echart_serie_color_get(serie);

    r = evas_vg_shape_add(sd->common.root);
    _echart_vbar_coords_get(sd, xmin, xmax, ymin, ymax,
                            x, ymin, &xc, &yc);
    evas_vg_shape_append_move_to(r, xc, yc);
    _echart_vbar_coords_get(sd, xmin, xmax, ymin, ymax,
                            x, yv[i], &xc, &yc);
    evas_vg_shape_append_line_to(r, xc, yc);
    _echart_vbar_coords_get(sd, xmin, xmax, ymin, ymax,
                            x + L, yv[i], &xc, &yc);
    evas_vg_shape_append_line_to(r, xc, yc);
    _echart_vbar_coords_get(sd, xmin, xmax, ymin, ymax,
                            x + L, ymin, &xc, &yc);
    evas_vg_shape_append_line_to(r, xc, yc);
    evas_vg_shape_append_close(r);
    evas_vg_node_color_set(r,
                           COL_TO_R(cols.line),
                           COL_TO_G(cols.line),
                           COL_TO_B(cols.line),
                           COL_TO_A(cols.line));
}

static void
_echart_vbar_smart_calculate(Evas_Object *obj)
{
    Echart_Font_Style fs;
    const Echart_Data *data;
    const Echart_Serie *absciss;
    const Eina_List *series;
    const Eina_Inarray *x_values;
    Echart_Smart_Data *sd;
    unsigned int col;
    Efl_VG *vbar;
    double *xv;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double x;
    double l;
    double L;
    int gyn;
    int w;
    int h;
    int n;
    unsigned int nbr_series;
    unsigned int i;

    fprintf(stderr, " ** %s\n", __FUNCTION__);

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    sd->common.padding = 5;

    echart_chart_size_get(sd->common.chart, &w, &h);
    col = echart_chart_background_color_get(sd->common.chart);

    /* background */
    echart_background_set((Echart_Smart_Common *)sd, w, h, col);

    /* title */
    if (echart_chart_title_get(sd->common.chart))
    {
        Evas_Coord w_title;

        echart_chart_title_style_get(sd->common.chart, &fs);
        _echart_text_object_set(sd->common.title,
                                echart_chart_title_get(sd->common.chart),
                                &fs);
        evas_object_geometry_get(sd->common.title, NULL, NULL, &w_title, NULL);
        evas_object_move(sd->common.title, (w - w_title) / 2, 0);
    }

    /* echart_chart_grid_nbr_get(sd->common.chart, NULL, &gyn); */
    /* if (gyn > 0) */
    /* { */
    /* } */

    /* vg */
    echart_vg_set((Echart_Smart_Common *)sd);

    /* axis */
    vbar = evas_vg_shape_add(sd->common.root);
    evas_vg_shape_append_move_to(vbar, PAD(0.5), PAD(0.5));
    evas_vg_shape_append_line_to(vbar, PAD(0.5), sd->common.h_vg - PAD(0.5));
    evas_vg_shape_append_line_to(vbar, sd->common.w_vg - PAD(0.5), sd->common.h_vg - PAD(0.5));
    evas_vg_shape_stroke_width_set(vbar, 1);
    evas_vg_shape_stroke_color_set(vbar, 0, 0, 0, 255);

    /* vbars */
    data = echart_chart_data_get(sd->common.chart);
    absciss = echart_data_absciss_get(data);
    x_values = echart_serie_values_get(absciss);
    xv = (double *)x_values->members;
    series = echart_data_series_get(data);
    nbr_series = eina_list_count(series);

    echart_data_interval_get(data, &ymin, &ymax);
    n = (int)floor(log(ymax - ymin) / log(10));
    ymax = (floor(ymax / pow(10, n - 1)) + 1) * pow(10, n - 1);
    col = echart_chart_grid_color_get(sd->common.chart);

    echart_chart_grid_nbr_get(sd->common.chart, NULL, &gyn);
    if (gyn > 0)
    {
        for (i = 1; i <= (unsigned int)gyn; i++)
        {
            double y = i * (ymax - ymin) / gyn + ymin;
            int j = (ymax - y) * (sd->common.h_vg - 1) / (ymax - ymin);
            vbar = evas_vg_shape_add(sd->common.root);
            evas_vg_shape_append_move_to(vbar, PAD(0.5), j + PAD(0.5));
            evas_vg_shape_append_line_to(vbar, sd->common.w_vg - PAD2(0.5), j + PAD(0.5));
            evas_vg_shape_stroke_width_set(vbar, 1);
            evas_vg_shape_stroke_color_set(vbar,
                                           COL_TO_R(col), COL_TO_G(col), COL_TO_B(col), COL_TO_A(col));
        }
    }

    xmin = xv[0];
    xmax = xv[x_values->len - 1];
    l = (1 - sd->group_width) * (xmax - xmin) / (x_values->len + 1);
    L = sd->group_width * (xmax - xmin) / x_values->len;
    x = xmin + l;
    for (i = 0; i < x_values->len; i++)
    {
        double bx;
        double bl;
        unsigned int j;


        bl = L / nbr_series;
        bx = x;
        for (j = 0; j < nbr_series; j++)
        {
            _echart_vbar_add(sd, series, i, j,
                             xmin, xmax, ymin, ymax,
                             bx, bl);
            bx += bl;
        }

        x += l + L;
    }
}

static void
_echart_vbar_smart_init(void)
{
    static Evas_Smart_Class sc = EVAS_SMART_CLASS_INIT_NAME_VERSION(ECHART_VBAR_OBJ_NAME);

    if (_echart_vbar_smart) return;

    if (!sc.add)
    {
        sc.add = _echart_vbar_smart_add;
        sc.del = _echart_vbar_smart_del;
        sc.move = _echart_vbar_smart_move;
        sc.resize = _echart_vbar_smart_resize;
        sc.show = _echart_vbar_smart_show;
        sc.hide = _echart_vbar_smart_hide;
        sc.clip_set = _echart_vbar_smart_clip_set;
        sc.clip_unset = _echart_vbar_smart_clip_unset;
/*         sc.callbacks = _echart_vbar_smart_callbacks; */
        sc.calculate = _echart_vbar_smart_calculate;
    }
    _echart_vbar_smart = evas_smart_class_new(&sc);
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
echart_vbar_object_add(Evas *evas)
{
    Evas_Object *obj;

    _echart_vbar_smart_init();
    obj = evas_object_smart_add(evas, _echart_vbar_smart);

    return obj;
}

EAPI void
echart_vbar_object_chart_set(Evas_Object *obj, const Echart_Chart *chart)
{
    Echart_Smart_Data *sd;

    ECHART_VBAR_SMART_OBJ_GET(sd, obj, ECHART_VBAR_OBJ_NAME);
    INF("chart set");

    sd->common.chart = chart;
    evas_object_smart_need_recalculate_set(obj, EINA_TRUE);
}

EAPI void
echart_vbar_object_group_width_set(Evas_Object *obj, double group_width)
{
    Echart_Smart_Data *sd;

    ECHART_VBAR_SMART_OBJ_GET(sd, obj, ECHART_VBAR_OBJ_NAME);
    INF("chart set");

    if ((group_width < 0.0) || (group_width > 1.0))
        return;

    sd->group_width = group_width;
    evas_object_smart_need_recalculate_set(obj, EINA_TRUE);
}
