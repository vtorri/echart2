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

#ifndef ECHART_DATA_H
#define ECHART_DATA_H

typedef struct _Echart_Serie Echart_Serie;
typedef struct _Echart_Data Echart_Data;

EAPI Echart_Serie *echart_serie_new(void);
EAPI void echart_serie_free(Echart_Serie *s);
EAPI void echart_serie_title_set(Echart_Serie *s, const char *title);
EAPI const char *echart_serie_title_get(const Echart_Serie *s);
EAPI Echart_Colors echart_serie_color_get(const Echart_Serie *s);
EAPI void echart_serie_opacity_set(Echart_Serie *s, double opacity);
EAPI double echart_serie_opacity_get(const Echart_Serie *s);
EAPI void echart_serie_value_push(Echart_Serie *s, double y);
EAPI const Eina_Inarray *echart_serie_values_get(const Echart_Serie *s);

EAPI Echart_Data *echart_data_new(void);
EAPI void echart_data_free(Echart_Data *d);
EAPI void echart_data_title_set(Echart_Data *d, const char *title);
EAPI const char *echart_data_title_get(const Echart_Data *d);
EAPI void echart_data_absciss_set(Echart_Data *d, Echart_Serie *s);
EAPI const Echart_Serie *echart_data_absciss_get(const Echart_Data *d);
EAPI Eina_Bool echart_data_serie_append(Echart_Data *d, Echart_Serie *s);
EAPI const Eina_List *echart_data_series_get(const Echart_Data *d);
EAPI void echart_data_ymin_set(Echart_Data *d, double ymin);
EAPI void echart_data_interval_get(const Echart_Data *d, double *ymin, double *ymax);
EAPI void echart_data_area_set(Echart_Data *d, Eina_Bool on);
EAPI Eina_Bool echart_data_area_get(const Echart_Data *d);

#endif /* ECHART_DATA_H */
