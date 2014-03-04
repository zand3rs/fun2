/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2012-10-03
 *     /         Updated : 2012-10-03
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : notification_handler.cpp
 * Description : Notification handler.
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
#include "misc.hpp"
#include "httpclient.hpp"
#include "httputils.hpp"
#include "oradbnotification.hpp"
#include "notification_handler.hpp"

/*============================================================================*/

void* notification_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBNotification::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBNotification conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<notification_t> notifications;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        notifications.clear();

        if (conn.getNotifications(&notifications, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (notifications.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)notifications.size(); ++i) {
            notification_t& notification = notifications[i];
            notification.status = TXN_STATUS_PROCESSED;

            if (conn.updateNotification(&notification) < 0) {
                LOG_ERROR("%s: Unable to update notification id: %d, status: %d.", __func__, notification.id, notification.status);
            } else {
                if (0 != c2q_enqueue(Global::getNotificationQ(), (void*) &notification, sizeof(notification_t))) {
                    LOG_ERROR("%s: Unable to insert to notification queue id: %d.", __func__, notification.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* notification_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;
    time_t t;
    struct tm lt;
    char timestamp[32];
    char tran_type[8];

    if (OraDBNotification::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBNotification conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        notification_t notification;

        while (! c2q_dequeue(Global::getNotificationQ(), &notification, sizeof(notification_t))) {
            LOG_DEBUG("%s: %d: notification id: %d, tran_type: %d, ref_id: %d, msisdn: %s", __func__, proc_id,
                    notification.id, notification.tran_type, notification.ref_id, notification.msisdn);

            t = time(NULL);
            localtime_r(&t, &lt);
            strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", &lt);
            memset(tran_type, 0, sizeof(tran_type));

            switch (notification.tran_type) {
                case TRAN_TYPE_GROAM_ON:
                    snprintf(tran_type, sizeof(tran_type), "1");
                    break;
                case TRAN_TYPE_GROAM_OFF:
                case TRAN_TYPE_GROAM_NO:
                    snprintf(tran_type, sizeof(tran_type), "2");
                    break;
                default:
                    LOG_ERROR("%s: %d: Invalid notification tran_type: %d, id: %d.", __func__, proc_id,
                            notification.tran_type, notification.id);
                    break;
            }

            if (! *tran_type) {
                //-- invalid tran_type
                continue;
            }

            HttpClient hc;
            std::string req = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                              "<soapenv:Envelope\n"
                              " xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\"\n"
                              " xmlns:mbf=\"http://danateq.com/soap/link/raven/mbfun2\">\n"
                              "<soapenv:Header/>\n"
                              "<soapenv:Body>\n"
                              "    <mbf:RoamingStatusRequest>\n"
                              "        <MSISDN>" + std::string(notification.msisdn) + "</MSISDN>\n"
                              "        <DeactAct>" + std::string(tran_type) + "</DeactAct>\n"
                              "        <Timestamp>" + std::string(timestamp) + "</Timestamp>\n"
                              "    </mbf:RoamingStatusRequest>\n"
                              "</soapenv:Body>\n"
                              "</soapenv:Envelope>\n";
            int res_code = hc.httpPost(Config::getEndpointUrl(), req.c_str(), "text/xml", Config::getEndpointTimeoutSec());

            LOG_INFO("%s: %d: url: %s, res_code: %d, res_body: %s, res_error: %s", __func__, proc_id,
                    Config::getEndpointUrl(), res_code, hc.getResponseBody(), hc.getError());

            notification.status = (200 == res_code) ? TXN_STATUS_SUCCESSFUL : TXN_STATUS_ERROR;

            if (conn.updateNotification(&notification) < 0) {
                LOG_ERROR("%s: %d: Unable to update notification id: %d, status: %d.", __func__, proc_id,
                        notification.id, notification.status);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/******************************************************************************/

