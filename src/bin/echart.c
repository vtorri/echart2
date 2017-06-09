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
#include <Ecore.h>
#include <Ecore_Evas.h>

#include <Echart.h>
#include <echart_data.h>
#include <echart_chart.h>

static void
_echart_delete_cb(Ecore_Evas *ee EINA_UNUSED)
{
    ecore_main_loop_quit();
}

static Evas_Object *
echart_text_object_add(Evas *evas, const char *text,
                       const char *font_name,
                       int font_size,
                       unsigned int font_color,
                       Eina_Bool bold,
                       Eina_Bool italic)
{
    char buf[256];
    char buf2[32];
    Evas_Object *o;
    int r;
    int g;
    int b;
    int a;

    if (!text)
        return NULL;

    *buf2 = '\0';
    if (bold || italic)
    {
        memcpy(buf2, ":style=", 7);
        if (!italic)
            memcpy(buf2 + 7, "Bold", 5);
        else if (!bold)
            memcpy(buf2 + 7, "Italic", 7);
        else
            memcpy(buf2 + 7, "Bold Italic", 12);
    }
    snprintf(buf, sizeof(buf), "%s%s",
             font_name ? font_name : "Sans", buf2);
    buf[255] = '\0';
    a = (font_color >> 24) | 0xff;
    r = (font_color >> 16) | 0xff;
    g = (font_color >> 8) | 0xff;
    b = (font_color >> 0) | 0xff;
    o = evas_object_text_add(evas);
    evas_object_text_style_set(o, EVAS_TEXT_STYLE_PLAIN);
    evas_object_color_set(o, r, g, b, a);
    evas_object_text_font_set(o, buf, (font_size <= 0) ? 13 : font_size);
    evas_object_text_text_set(o, text);

    return o;
}

int main()
{
    Ecore_Evas *ee;
    Evas *evas;
    Evas_Object *o;
    Echart_Chart *chart;
    Echart_Data *d;
    Echart_Serie *s;
    int w;
    int h;

    if (!ecore_evas_init())
        return -1;

    if (!echart_init())
        goto shutdown_ecore_evas;

    ee = ecore_evas_new(NULL, 0, 0, 1, 1, NULL);
    if (!ee)
        goto shutdown_echart;

    ecore_evas_callback_delete_request_set(ee, _echart_delete_cb);
    evas = ecore_evas_get(ee);

    d = echart_data_new();
    echart_data_title_set(d, "Company Performance");

    s = echart_serie_new();
    echart_serie_title_set(s, "Year");
    echart_serie_value_push(s, 2004);
    echart_serie_value_push(s, 2005);
    echart_serie_value_push(s, 2006);
    echart_serie_value_push(s, 2007);
    echart_data_absciss_set(d, s);

    s = echart_serie_new();
    echart_serie_title_set(s, "Sales");
    echart_serie_value_push(s, 1000);
    echart_serie_value_push(s, 1170);
    echart_serie_value_push(s, 660);
    echart_serie_value_push(s, 1030);
    echart_data_serie_append(d, s);

    s = echart_serie_new();
    echart_serie_title_set(s, "Expenses");
    echart_serie_value_push(s, 400);
    echart_serie_value_push(s, 460);
    echart_serie_value_push(s, 1120);
    echart_serie_value_push(s, 540);
    echart_data_serie_append(d, s);

    chart = echart_chart_new(800, 600);
    echart_chart_data_set(chart, d);
    echart_chart_size_get(chart, &w, &h);
    echart_chart_background_color_set(chart, 0xffffffff);

    {
        Efl_VG *root;
        Efl_VG *line;
        Evas_Coord w_main_title = 0, h_main_title = 0;
        Evas_Coord w_haxis_title = 0, h_haxis_title = 0;
        Evas_Coord w_haxis_text = 0, h_haxis_text = 0;
        Evas_Coord w_chart, h_chart;

        o = evas_object_rectangle_add(evas);
        evas_object_color_set(o, 255, 255, 255, 255);
        evas_object_focus_set(o, 1);
        evas_object_move(o, 0, 0);
        evas_object_resize(o, w, h);
        //evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, _keydown, NULL);
        evas_object_show(o);

        o = echart_text_object_add(evas, "Company Performance",
                                   NULL, 0,
                                   0xff000000, EINA_TRUE, EINA_TRUE);
        evas_object_geometry_get(o, NULL, NULL, &w_main_title, &h_main_title);
        evas_object_move(o, (w - w_main_title) / 2, 0);
        evas_object_show(o);

        h_chart = h - (h_main_title + h_haxis_title + h_haxis_text);

        o = evas_object_vg_add(evas);
        evas_object_resize(o, w, h);
        evas_object_move(o, 0, 0);
        evas_object_show(o);

        root = evas_object_vg_root_node_get(o);

        line = evas_vg_shape_add(root);
        evas_vg_shape_append_move_to(line, 10, h - 10);
        evas_vg_shape_append_line_to(line, w - 10, h - 10);
        /* evas_vg_shape_shape_append_close(line); */
        evas_vg_shape_stroke_width_set(line, 1);
        evas_vg_shape_stroke_color_set(line, 0, 0, 0, 255);
    }

    ecore_evas_resize(ee, w, h);
    ecore_evas_show(ee);

    ecore_main_loop_begin();

    ecore_evas_shutdown();

    return 0;

  shutdown_echart:
    echart_shutdown();
  shutdown_ecore_evas:
    ecore_evas_shutdown();

    return -1;
}
