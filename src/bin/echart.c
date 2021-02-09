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
#include <echart_line.h>
#include <echart_vbar.h>

static void
_echart_delete_cb(Ecore_Evas *ee EINA_UNUSED)
{
    ecore_main_loop_quit();
}

int main(void)
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

    echart_data_ymin_set(d, 0);
    echart_data_area_set(d, EINA_TRUE);

    chart = echart_chart_new(800, 600);
    echart_chart_title_set(chart, "Company Performance");
    echart_chart_data_set(chart, d);
    echart_chart_size_get(chart, &w, &h);
    echart_chart_background_color_set(chart, 0xffffffff);

#if 1
    o = echart_line_object_add(evas);
    echart_line_object_chart_set(o, chart);
#else
    o = echart_vbar_object_add(evas);
    echart_vbar_object_chart_set(o, chart);
#endif
    evas_object_show(o);

    ecore_evas_resize(ee, w, h);
    ecore_evas_show(ee);

    ecore_main_loop_begin();

    echart_chart_free(chart);
    echart_shutdown();
    ecore_evas_shutdown();

    return 0;

  shutdown_echart:
    echart_shutdown();
  shutdown_ecore_evas:
    ecore_evas_shutdown();

    return -1;
}
