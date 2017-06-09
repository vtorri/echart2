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

#include <stdio.h>

#include <Eina.h>
#include <Evas.h>

#include "Echart.h"
#include "echart_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

static int _echart_init_count = 0;

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

int echart_log_dom_global = -1;

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI int
echart_init(void)
{
    if (++_echart_init_count != 1)
        return _echart_init_count;

    if (!eina_init())
    {
        fprintf(stderr, "Echart: Could not initialize Eina.\n");
        return --_echart_init_count;
    }

    echart_log_dom_global = eina_log_domain_register(PACKAGE,
                                                     ECHART_DEFAULT_LOG_COLOR);
    if (echart_log_dom_global < 0)
    {
        EINA_LOG_ERR("Echart: Could not register log domain 'echart'.");
        goto shutdown_eina;
    }

    if (!evas_init())
    {
        ERR("Could not initialize Evas.");
        goto unregister_log_domain;
    }

    return _echart_init_count;

  unregister_log_domain:
    eina_log_domain_unregister(echart_log_dom_global);
    echart_log_dom_global = -1;
  shutdown_eina:
    eina_shutdown();

    return --_echart_init_count;
}

EAPI int
echart_shutdown(void)
{
    if (_echart_init_count <= 0)
    {
        ERR("Init count not greater than 0 in shutdown.");
        return 0;
    }
    if (--_echart_init_count != 0)
        return _echart_init_count;

    evas_shutdown();
    eina_log_domain_unregister(echart_log_dom_global);
    echart_log_dom_global = -1;
    eina_shutdown();

    return _echart_init_count;
}
