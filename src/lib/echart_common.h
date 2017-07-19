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

#ifndef ECHART_COMMON_H
#define ECHART_COMMON_H

#include <Eina.h>
#include <Evas.h>

#define COL_TO_A(col_) (((col_) >> 24) & 0xff)
#define COL_TO_R(col_) (((col_) >> 16) & 0xff)
#define COL_TO_G(col_) (((col_) >> 8 ) & 0xff)
#define COL_TO_B(col_) (((col_)      ) & 0xff)

#define PAD(v_) (sd->common.padding + (v_))
#define PAD2(v_) (2 * sd->common.padding + (v_))

typedef struct
{
    EINA_REFCOUNT;

    const Echart_Chart *chart;
    Eina_Inarray *ord;
    Evas_Object *bg;
    Evas_Object *title;
    Evas_Object *vg;
    Efl_VG *root;
    Evas_Coord w_vg;
    Evas_Coord h_vg;
    Evas_Coord padding;
} Echart_Smart_Common;

typedef struct
{
    Evas_Coord left;
    Evas_Coord right;
    Evas_Coord top;
    Evas_Coord bottom;
} Echart_Offsets;

void echart_background_set(Echart_Smart_Common *esc,
                           int w, int h, unsigned int col);

void echart_vg_set(Echart_Smart_Common *esc);

void echart_offsets_get(Evas_Object *title, Echart_Offsets *offsets);

#endif /* ECHART_COMMON_H */
