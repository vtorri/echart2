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

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

struct _Echart_Serie
{
    char *title;
    Eina_Inarray *values;
    double ymin;
    double ymax;
    Echart_Colors color;
};

struct _Echart_Data
{
    char *title;
    Echart_Serie *absciss;
    Eina_List *series;
    double ymin;
    double ymax;
    Eina_Bool area : 1;
};

static Echart_Colors _echart_chart_colors_default[20] =
{
    { 0xff3366CC, 0xffc2d1f0 },
    { 0xffDC3912, 0xfff5c4b8 },
    { 0xffFF9900, 0xffffe0b3 },
    { 0xff109618, 0xffb7dfba },
    { 0xff990099, 0xffe0b3e0 },
    { 0xff3B3EAC, 0xffb3e0ee },
    { 0xff0099C6, 0xfff5c7d6 },
    { 0xffDD4477, 0xffd1e6b3 },
    { 0xff66AA00, 0xffeac0c0 },
    { 0xffB82E2E, 0xffc1d0df },
    { 0xff316395, 0xffe0c7e0 },
    { 0xff994499, 0xffbde6e0 },
    { 0xff22AA99, 0xffe6e6b8 },
    { 0xffAAAA11, 0xffd1c2f0 },
    { 0xff6633CC, 0xfff7d5b3 },
    { 0xffE67300, 0xffdcb5b5 },
    { 0xff8B0707, 0xffd1b7d1 },
    { 0xff329262, 0xffc2ded0 },
    { 0xff5574A6, 0xffccd5e4 },
    { 0xff3B3EAC, 0xffc4c5e6 }
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

EAPI Echart_Serie *
echart_serie_new(void)
{
    Echart_Serie *s;

    s = (Echart_Serie *)malloc(sizeof(Echart_Serie));

    s->values = eina_inarray_new(sizeof(double), 0);
    if (!s->values)
    {
        free(s);
        return NULL;
    }

    return s;
}

EAPI void
echart_serie_free(Echart_Serie *s)
{
    if (!s)
        return;

    eina_inarray_free(s->values);
    free(s->title);
    free(s);
}

EAPI void
echart_serie_title_set(Echart_Serie *s, const char *title)
{
    if (!s || !title || !*title)
        return;

    s->title = strdup(title);
}

EAPI Echart_Colors
echart_serie_color_get(const Echart_Serie *s)
{
    if (!s)
    {
        Echart_Colors black = { 0xff000000, 0xff000000 };
        return black;
    }

    return s->color;
}

EAPI const char *
echart_serie_title_get(const Echart_Serie *s)
{
    return s ? s->title : NULL;
}

EAPI void
echart_serie_value_push(Echart_Serie *s, double y)
{
    eina_inarray_push(s->values, &y);
    if (eina_inarray_count(s->values) == 1)
    {
        s->ymin = y;
        s->ymax = y;
    }
    else
    {
        if (y < s->ymin) s->ymin = y;
        if (y > s->ymax) s->ymax = y;
    }
}

EAPI const Eina_Inarray *
echart_serie_values_get(const Echart_Serie *s)
{
    return s ? s->values : NULL;
}

EAPI Echart_Data *
echart_data_new(void)
{
    Echart_Data *d;

    d = (Echart_Data *)calloc(1, sizeof(Echart_Data));

    return d;
}

EAPI void
echart_data_free(Echart_Data *d)
{
    Echart_Serie *s;

    if (!d)
        return;

    EINA_LIST_FREE(d->series, s)
        echart_serie_free(s);
    echart_serie_free(d->absciss);
    free(d->title);
}

EAPI void
echart_data_title_set(Echart_Data *d, const char *title)
{
    if (!d || !title || !*title)
        return;

    d->title = strdup(title);
}

EAPI const char *
echart_data_title_get(const Echart_Data *d)
{
    return d ? d->title : NULL;
}

EAPI void
echart_data_absciss_set(Echart_Data *d, Echart_Serie *s)
{
    if (!d || !s)
        return;

    d->absciss = s;
}

EAPI const Echart_Serie *
echart_data_absciss_get(const Echart_Data *d)
{
    return d ? d->absciss : NULL;
}

EAPI Eina_Bool
echart_data_serie_append(Echart_Data *d, Echart_Serie *s)
{
    unsigned int count;

    if (!d || !s)
        return EINA_FALSE;

    if (eina_inarray_count(d->absciss->values) != eina_inarray_count(s->values))
    {
        WRN("Adding an item with different values count from the absciss");
        return EINA_FALSE;
    }

    count = eina_list_count(d->series);
    s->color = _echart_chart_colors_default[count % 20];
    d->series = eina_list_append(d->series, s);
    if (count == 0)
    {
        d->ymin = s->ymin;
        d->ymax = s->ymax;
    }
    else
    {
        if (s->ymin < d->ymin) d->ymin = s->ymin;
        if (s->ymax > d->ymax) d->ymax = s->ymax;
    }

    return EINA_TRUE;
}

EAPI const Eina_List *
echart_data_series_get(const Echart_Data *d)
{
    return d ? d->series : NULL;
}

EAPI void
echart_data_ymin_set(Echart_Data *d, double ymin)
{
    if (d)
        d->ymin = ymin;
}

EAPI void
echart_data_interval_get(const Echart_Data *d, double *ymin, double *ymax)
{
    if (!d)
    {
        if (ymin) *ymin = 0;
        if (ymax) *ymax = 0;
        return;
    }

    if (ymin) *ymin = d->ymin;
    if (ymax) *ymax = d->ymax;
}

EAPI void
echart_data_area_set(Echart_Data *d, Eina_Bool on)
{
    if (d)
        d->area = !!on;
}

EAPI Eina_Bool
echart_data_area_get(const Echart_Data *d)
{
    return d ? d->area : EINA_FALSE;
}
