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
#include <Evas.h>

#include "Echart.h"
#include "echart_private.h"
#include "echart_data.h"
#include "echart_chart.h"
#include "echart_common.h"

void
echart_background_set(Echart_Smart_Common *esc,
                      int w, int h, unsigned int col)
{
    evas_object_move(esc->bg, 0, 0);
    evas_object_resize(esc->bg, w, h);
    evas_object_color_set(esc->bg,
                          COL_TO_R(col), COL_TO_G(col), COL_TO_B(col), COL_TO_A(col));
}

void
echart_vg_set(Echart_Smart_Common *esc)
{
    Echart_Offsets offsets;
    int w;
    int h;

    echart_chart_size_get(esc->chart, &w, &h);
    echart_offsets_get(esc->title, &offsets);
    esc->w_vg = w - (offsets.left + offsets.right);
    esc->h_vg = h - (offsets.top + offsets.bottom);
    evas_object_move(esc->vg, offsets.left, offsets.top);
    evas_object_resize(esc->vg, esc->w_vg, esc->h_vg);
}

void
echart_offsets_get(Evas_Object *title, Echart_Offsets *offsets)
{
    offsets->left = 10;
    offsets->right = 10;
    evas_object_geometry_get(title, NULL, NULL, NULL, &offsets->top);
    offsets->top += 10;
    offsets->bottom = 10;
}
