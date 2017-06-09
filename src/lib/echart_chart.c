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

#include <Eina.h>

#include "Echart.h"
#include "echart_private.h"
#include "echart_data.h"
#include "echart_chart.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

typedef struct _Echart_Text
{
    char *text;
    char *font_name;
    int font_size;
    unsigned int font_color;
    Eina_Bool bold : 1;
    Eina_Bool italic : 1;
} Echart_Text;

struct _Echart_Chart
{
    Echart_Data *data;
    Echart_Text title;
    Echart_Text title_haxis;
    Echart_Text title_vaxis;
    unsigned int bg_color;
    int width;
    int height;
    struct
    {
        int x_nbr;
        int y_nbr;
        unsigned int color;
    } grid, sub_grid;
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Echart_Chart *
echart_chart_new(int width, int height)
{
    Echart_Chart *chart;

    chart = (Echart_Chart *)calloc(1, sizeof(Echart_Chart));
    if (!chart)
        return NULL;

    chart->title.font_color = 0xff000000;
    chart->title.bold = EINA_TRUE;
    chart->title_haxis.font_color = 0xff000000;
    chart->title_haxis.italic = EINA_TRUE;
    chart->title_vaxis.font_color = 0xff000000;
    chart->title_vaxis.italic = EINA_TRUE;
    chart->width = width;
    chart->height = height;
    chart->bg_color = 0xff808080;
    chart->grid.x_nbr = 5;
    chart->grid.y_nbr = 5;
    chart->grid.color = 0xff888888;
    chart->sub_grid.x_nbr = 0;
    chart->sub_grid.y_nbr = 0;
    chart->sub_grid.color = 0xffeeeeee;

    return chart;
}

EAPI void
echart_chart_free(Echart_Chart *chart)
{
    if (!chart)
        return;

    if (chart->data)
        echart_data_free(chart->data);
    free(chart);
}

EAPI void
echart_chart_title_set(Echart_Chart *chart, const char *title)
{
    char *t;

    if (!chart || !title || !*title)
        return;

    t = strdup(title);
    if (t)
    {
        free(chart->title.text);
        chart->title.text = t;
    }
}

EAPI const char *
echart_chart_title_get(const Echart_Chart *chart)
{
    if (!chart)
        return NULL;

    return chart->title.text;
}

EAPI void
echart_chart_title_style_set(Echart_Chart *chart,
                             const char *font_name,
                             int font_size,
                             unsigned int font_color,
                             Eina_Bool bold,
                             Eina_Bool italic)
{
    char *fn;

    if (!chart)
        return;

    fn = strdup(font_name);
    if (fn)
    {
        free(chart->title.font_name);
        chart->title.font_name = fn;
    }
    chart->title.font_size = font_size;
    chart->title.font_color = font_color;
    chart->title.bold = bold;
    chart->title.italic = italic;
}

EAPI void
echart_chart_title_style_get(const Echart_Chart *chart,
                             const char **font_name,
                             int *font_size,
                             unsigned int *font_color,
                             Eina_Bool *bold,
                             Eina_Bool *italic)
{
    if (!chart)
        return;

    if (font_name) *font_name = chart->title.font_name;
    if (font_size) *font_size = chart->title.font_size;
    if (font_color) *font_color = chart->title.font_color;
    if (bold) *bold = chart->title.bold;
    if (italic) *italic = chart->title.italic;
}

EAPI void
echart_chart_title_haxis_set(Echart_Chart *chart, const char *title)
{
    char *t;

    if (!chart || !title || !*title)
        return;

    t = strdup(title);
    if (t)
    {
        free(chart->title_haxis.text);
        chart->title_haxis.text = t;
    }
}

EAPI const char *
echart_chart_title_haxis_get(const Echart_Chart *chart)
{
    if (!chart)
        return NULL;

    return chart->title_haxis.text;
}

EAPI void
echart_chart_title_haxis_style_set(Echart_Chart *chart,
                                   const char *font_name,
                                   int font_size,
                                   unsigned int font_color,
                                   Eina_Bool bold,
                                   Eina_Bool italic)
{
    char *fn;

    if (!chart)
        return;

    fn = strdup(font_name);
    if (fn)
    {
        free(chart->title_haxis.font_name);
        chart->title_haxis.font_name = fn;
    }
    chart->title_haxis.font_size = font_size;
    chart->title_haxis.font_color = font_color;
    chart->title_haxis.bold = bold;
    chart->title_haxis.italic = italic;
}

EAPI void
echart_chart_title_haxis_style_get(const Echart_Chart *chart,
                                   const char **font_name,
                                   int *font_size,
                                   unsigned int *font_color,
                                   Eina_Bool *bold,
                                   Eina_Bool *italic)
{
    if (!chart)
        return;

    if (font_name) *font_name = chart->title_haxis.font_name;
    if (font_size) *font_size = chart->title_haxis.font_size;
    if (font_color) *font_color = chart->title_haxis.font_color;
    if (bold) *bold = chart->title_haxis.bold;
    if (italic) *italic = chart->title_haxis.italic;
}

EAPI void
echart_chart_title_vaxis_set(Echart_Chart *chart, const char *title)
{
    char *t;

    if (!chart || !title || !*title)
        return;

    t = strdup(title);
    if (t)
    {
        free(chart->title_vaxis.text);
        chart->title_vaxis.text = t;
    }
}

EAPI const char *
echart_chart_title_vaxis_get(const Echart_Chart *chart)
{
    if (!chart)
        return NULL;

    return chart->title_vaxis.text;
}

EAPI void
echart_chart_title_vaxis_style_set(Echart_Chart *chart,
                                   const char *font_name,
                                   int font_size,
                                   unsigned int font_color,
                                   Eina_Bool bold,
                                   Eina_Bool italic)
{
    char *fn;

    if (!chart)
        return;

    fn = strdup(font_name);
    if (fn)
    {
        free(chart->title_vaxis.font_name);
        chart->title_vaxis.font_name = fn;
    }
    chart->title_vaxis.font_size = font_size;
    chart->title_vaxis.font_color = font_color;
    chart->title_vaxis.bold = bold;
    chart->title_vaxis.italic = italic;
}

EAPI void
echart_chart_title_vaxis_style_get(const Echart_Chart *chart,
                                   const char **font_name,
                                   int *font_size,
                                   unsigned int *font_color,
                                   Eina_Bool *bold,
                                   Eina_Bool *italic)
{
    if (!chart)
        return;

    if (font_name) *font_name = chart->title_vaxis.font_name;
    if (font_size) *font_size = chart->title_vaxis.font_size;
    if (font_color) *font_color = chart->title_vaxis.font_color;
    if (bold) *bold = chart->title_vaxis.bold;
    if (italic) *italic = chart->title_vaxis.italic;
}

EAPI void
echart_chart_background_color_set(Echart_Chart *chart, unsigned int color)
{
    if (!chart)
        return;

    chart->bg_color = color;
}

EAPI unsigned int
echart_chart_background_color_get(const Echart_Chart *chart)
{
    if (!chart)
        return 0xffffffff;

    return chart->bg_color;
}

EAPI void
echart_chart_size_set(Echart_Chart *chart, int width, int height)
{
    if (!chart ||
        (width <= 0) || (height <= 0) ||
        ((chart->width == width) && (chart->height == height)))
        return;

    chart->width = width;
    chart->height = height;
}

EAPI void
echart_chart_size_get(const Echart_Chart *chart, int *width, int *height)
{
    if (!chart)
    {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }

    if (width) *width = chart->width;
    if (height) *height = chart->height;
}

EAPI void
echart_chart_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr)
{
    if (!chart || (grid_x_nbr < 0) || (grid_y_nbr < 0))
        return;

    chart->grid.x_nbr = grid_x_nbr;
    chart->grid.y_nbr = grid_y_nbr;
}

EAPI void
echart_chart_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr)
{
    if (!chart)
    {
        if (grid_x_nbr) *grid_x_nbr = 0;
        if (grid_y_nbr) *grid_y_nbr = 0;
        return;
    }

    if (grid_x_nbr) *grid_x_nbr = chart->grid.x_nbr;
    if (grid_y_nbr) *grid_y_nbr = chart->grid.y_nbr;
}

EAPI void
echart_chart_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    if (!chart)
        return;

    chart->grid.color = (a << 24) | (r << 16) | (g << 8) | b;
}

EAPI unsigned int
echart_chart_grid_color_get(const Echart_Chart *chart)
{
    if (!chart)
        return 0xffcccccc;

    return chart->grid.color;
}

EAPI void
echart_chart_sub_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr)
{
    if (!chart || (grid_x_nbr < 0) || (grid_y_nbr < 0))
        return;

    chart->sub_grid.x_nbr = grid_x_nbr;
    chart->sub_grid.y_nbr = grid_y_nbr;
}

EAPI void
echart_chart_sub_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr)
{
    if (!chart)
    {
        if (grid_x_nbr) *grid_x_nbr = 0;
        if (grid_y_nbr) *grid_y_nbr = 0;
        return;
    }

    if (grid_x_nbr) *grid_x_nbr = chart->sub_grid.x_nbr;
    if (grid_y_nbr) *grid_y_nbr = chart->sub_grid.y_nbr;
}

EAPI void
echart_chart_sub_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    if (!chart)
        return;

    chart->sub_grid.color = (a << 24) | (r << 16) | (g << 8) | b;
}

EAPI unsigned int
echart_chart_sub_grid_color_get(const Echart_Chart *chart)
{
    if (!chart)
        return 0xffcccccc;

    return chart->sub_grid.color;
}

EAPI void
echart_chart_data_set(Echart_Chart *chart, Echart_Data *data)
{
    if (!chart || !data)
        return;

    chart->data = data;
}

EAPI const Echart_Data *
echart_chart_data_get(const Echart_Chart *chart)
{
    if (!chart)
        return NULL;;

    return chart->data;
}
