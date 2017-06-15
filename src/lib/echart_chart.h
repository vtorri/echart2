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

#ifndef ECHART_CHART_H
#define ECHART_CHART_H

typedef struct _Echart_Chart Echart_Chart;

typedef struct
{
    char *font_name;
    int font_size;
    unsigned int font_color;
    Eina_Bool bold : 1;
    Eina_Bool italic : 1;
} Echart_Font_Style;

EAPI Echart_Chart *echart_chart_new(int width, int height);
EAPI void echart_chart_free(Echart_Chart *chart);

EAPI void echart_chart_title_set(Echart_Chart *chart, const char *title);
EAPI const char *echart_chart_title_get(const Echart_Chart *chart);
EAPI void echart_chart_title_style_set(Echart_Chart *chart,
                                       const Echart_Font_Style *fs);
EAPI Eina_Bool echart_chart_title_style_get(const Echart_Chart *chart,
                                            Echart_Font_Style *fs);

EAPI void echart_chart_title_haxis_set(Echart_Chart *chart, const char *title);
EAPI const char *echart_chart_title_haxis_get(const Echart_Chart *chart);
EAPI void echart_chart_title_haxis_style_set(Echart_Chart *chart,
                                             const Echart_Font_Style *fs);
EAPI Eina_Bool echart_chart_title_haxis_style_get(const Echart_Chart *chart,
                                                  Echart_Font_Style *fs);

EAPI void echart_chart_title_vaxis_set(Echart_Chart *chart, const char *title);
EAPI const char *echart_chart_title_vaxis_get(const Echart_Chart *chart);
EAPI void echart_chart_title_vaxis_style_set(Echart_Chart *chart,
                                             const Echart_Font_Style *fs);
EAPI Eina_Bool echart_chart_title_vaxis_style_get(const Echart_Chart *chart,
                                                  Echart_Font_Style *fs);

EAPI void echart_chart_background_color_set(Echart_Chart *chart, unsigned int color);
EAPI unsigned int echart_chart_background_color_get(const Echart_Chart *chart);

EAPI void echart_chart_size_set(Echart_Chart *chart, int width, int height);
EAPI void echart_chart_size_get(const Echart_Chart *chart, int *width, int *height);

EAPI void echart_chart_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr);
EAPI void echart_chart_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr);
EAPI void echart_chart_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI unsigned int echart_chart_grid_color_get(const Echart_Chart *chart);
EAPI void echart_chart_sub_grid_nbr_set(Echart_Chart *chart, int grid_x_nbr, int grid_y_nbr);
EAPI void echart_chart_sub_grid_nbr_get(const Echart_Chart *chart, int *grid_x_nbr, int *grid_y_nbr);
EAPI void echart_chart_sub_grid_color_set(Echart_Chart *chart, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI unsigned int echart_chart_sub_grid_color_get(const Echart_Chart *chart);
EAPI void echart_chart_data_set(Echart_Chart *chart, Echart_Data *data);
EAPI const Echart_Data *echart_chart_data_get(const Echart_Chart *chart);

#endif /* ECHART_CHART_H */
