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

#ifndef ECHART_LINE_H
#define ECHART_LINE_H

EAPI Evas_Object *echart_line_object_add(Evas *evas);
EAPI void echart_line_object_chart_set(Evas_Object *obj, const Echart_Chart *chart);

#endif /* ECHART_LINE_H */
