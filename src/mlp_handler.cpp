/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2016-06-28
 *     /         Updated : 2016-06-28
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : mlp_handler.cpp
 * Description : MLP request handler.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "global.hpp"
#include "config.hpp"
#include "oradbrequest.hpp"
#include "mlp_handler.hpp"
#include "mlp_service.hpp"

/*============================================================================*/

void* request_listener (void* arg)
{
    void* retr = NULL;
    MlpService mlp_service;

    mlp_service.initialize(Config::getListenPort(), Config::getThreadCount(), Config::getOraAuth());

    if (0 != mlp_service.start()) {
        LOG_CRITICAL("%s: Unable to start http handler!", __func__);
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started. Listening on port %d.", __func__, Config::getListenPort());

    while (! IS_SHUTDOWN()) {
        sys_msleep(1000);
    }

    mlp_service.stop();

    LOG_INFO("%s: Terminated.", __func__);

    return retr;
}

/******************************************************************************/
