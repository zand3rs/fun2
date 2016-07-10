/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2016-07-10
 *     /         Updated : 2016-07-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : shampoo_handler.cpp
 * Description : Shampoo request handler.
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
#include "shampoo_handler.hpp"
#include "shampoo_service.hpp"

/*============================================================================*/

void* request_listener (void* arg)
{
    void* retr = NULL;
    ShampooService shampoo_service;

    shampoo_service.initialize(Config::getListenPort(), Config::getThreadCount(), Config::getOraAuth());

    if (0 != shampoo_service.start()) {
        LOG_CRITICAL("%s: Unable to start http handler!", __func__);
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started. Listening on port %d.", __func__, Config::getListenPort());

    while (! IS_SHUTDOWN()) {
        sys_msleep(1000);
    }

    shampoo_service.stop();

    LOG_INFO("%s: Terminated.", __func__);

    return retr;
}

/******************************************************************************/
