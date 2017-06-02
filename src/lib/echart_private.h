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

#ifndef ECHART_MAIN_H
#define ECHART_MAIN_H

extern int echart_log_dom_global;

#ifdef ECHART_DEFAULT_LOG_COLOR
# undef ECHART_DEFAULT_LOG_COLOR
#endif
#define ECHART_DEFAULT_LOG_COLOR EINA_COLOR_CYAN

#ifdef ERR
# undef ERR
#endif
#define ERR(...)  EINA_LOG_DOM_ERR(echart_log_dom_global, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...)  EINA_LOG_DOM_DBG(echart_log_dom_global, __VA_ARGS__)

#ifdef INF
# undef INF
#endif
#define INF(...)  EINA_LOG_DOM_INFO(echart_log_dom_global, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif
#define WRN(...)  EINA_LOG_DOM_WARN(echart_log_dom_global, __VA_ARGS__)

#ifdef CRIT
# undef CRIT
#endif
#define CRIT(...) EINA_LOG_DOM_CRIT(echart_log_dom_global, __VA_ARGS__)

#endif
