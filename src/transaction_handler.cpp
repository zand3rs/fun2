/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : transaction_handler.cpp
 * Description : Transaction handler.
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
#include <dlfcn.h>

#include "fun2rc.hpp"

#include "global.hpp"
#include "config.hpp"
#include "sysmsg.hpp"
#include "misc.hpp"
#include "hlr.hpp"
#include "oradbrequest.hpp"
#include "oradbresponse.hpp"
#include "oradbnotification.hpp"
#include "ccb_handler.hpp"
#include "transaction_handler.hpp"

/*============================================================================*/

static int getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    HLR hlrh(Config::getOraAuth());
    return hlrh.getIMSI(msisdn, imsi, imsi_size);
}

/*============================================================================*/

static void process_tran (OraDBRequest& conn, request_t& request)
{
    if (request.step_no >= 0) {
        //-- not the last step yet...
        return;
    }

    if (TXN_STATUS_SUCCESSFUL == request.status) {
        if (conn.processTran(&request) < 0) {
            LOG_ERROR("%s: process_tran failed request id: %d, tran_type: %d, a_no: %s.", __func__,
                    request.id, request.tran_type, request.a_no);
        } else {
            LOG_INFO("%s: retr: %d, request id: %d, tran_type: %d, customer_type: %s, a_no: %s, b_no: %s, gsm_num: %s"
                    ", min_bal: %d, activation_date: %s, deactivation_date: %s, duration: %d, silent: %d"
                    ", imsi: %s, last_step_no: %d, next_step_no: %d", __func__
                    , request.db_retr, request.id, request.tran_type, request.customer_type, request.a_no, request.b_no, request.gsm_num
                    , request.min_bal , request.activation_date, request.deactivation_date, request.duration, request.silent
                    , request.imsi, request.last_step_no, request.step_no);

            switch (request.db_retr) {
                case DB_RETR_OK:
                    //-- send successful message here...
                    switch (request.tran_type) {
                        case TRAN_TYPE_GROAM_ON:
                            if (request.last_step_no > 0) {
                                //-- insert to ccb queue for processing...
                                ccb_insert(request.a_no, CCB_ACTION_ACTIVATE);

                                //-- insert to notification queue for processing...
                                notification_insert(request.tran_type, request.id, request.a_no);

                                if (! request.silent) {
                                    if (! strcasecmp(request.request_origin, "ARDS")) {
                                        //-- ARDS activation
                                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                                Config::getAccessCode(), request.a_no, SYSMSG_ARDS_ROAM_ACTIVATED, 1);
                                    } else {
                                        //-- regular activation
                                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                                Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_ACTIVATED, 1,
                                                request.activation_date, request.deactivation_date);
#if 0
                                        switch (request.result_code) {
                                            case RESULT_CODE_W_RBT:
                                                if (request.duration > 0) {
                                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_SUCCESSFUL_W_RBT, 1);
                                                } else {
                                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_SUCCESSFUL_W_RBT_OPEN, 1);
                                                }
                                                break;
                                            case RESULT_CODE_WO_RBT:
                                                if (request.duration > 0) {
                                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_SUCCESSFUL_WO_RBT, 1);
                                                } else {
                                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_SUCCESSFUL_WO_RBT_OPEN, 1);
                                                }
                                                break;
                                        }
#endif
                                    }
                                }
                            } else if (request.tran_type == TRAN_TYPE_GROAM_ON) {
                                if (! request.silent) {
                                    //-- pre activation
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_PRE_ACT_SUCCESSFUL, 1,
                                            request.activation_date);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_OFF:
                            if (request.last_step_no > 0) {
                                //-- insert to ccb queue for processing...
                                ccb_insert(request.a_no, CCB_ACTION_DEACTIVATE);

                                //-- insert to notification queue for processing...
                                notification_insert(request.tran_type, request.id, request.a_no);

                                if (! request.silent) {
                                    if (! strcasecmp(request.request_origin, "ARDS")) {
                                        //-- ARDS deactivation
                                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                                Config::getAccessCode(), request.a_no, SYSMSG_ARDS_ROAM_DEACTIVATED, 1);
                                    } else {
                                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                                Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_SUCCESSFUL, 1);
                                    }
                                }

                                //--- call NF here...
                                request.result_code = nf_deprovision(request.a_no, request.service_id);
                                LOG_DEBUG("%s: nf_deprovision: %d", __func__, request.result_code);
                                int usurf_status = conn.usurfDeactivation(&request);
                                LOG_DEBUG("%s: usurfDeactivation: %d", __func__, usurf_status);
                            } else {
                                if (! request.silent) {
                                    //-- pre activation
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_PRE_ACT_SUCCESSFUL, 1);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_YES:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_YES_SUCCESSFUL, 1,
                                        request.activation_date, request.deactivation_date);
                            }
                            break;
                        case TRAN_TYPE_GROAM_NO:
                            if (request.last_step_no > 0) {
                                //-- insert to ccb queue for processing...
                                ccb_insert(request.a_no, CCB_ACTION_DEACTIVATE);

                                //-- insert to notification queue for processing...
                                notification_insert(request.tran_type, request.id, request.a_no);

                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_NO_SUCCESSFUL, 1);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_EXTEND:
                            if (! request.silent) {
                                if (request.duration > 0) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_SUCCESSFUL, 1,
                                            request.duration);
                                } else {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_SUCCESSFUL_W_OPEN_DURATION, 1);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_STATUS:
                            if (! request.silent) {
                                if (*(request.deactivation_date)) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_STATUS, 1,
                                            request.activation_date, request.deactivation_date);
                                } else {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_STATUS_UNLIMITED, 1);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_HELP:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_HELP, 1);
                            }
                            break;
                        case TRAN_TYPE_GLIST:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GLIST_WITH_ROAMING, 1,
                                        request.country, request.zone, request.rate, request.zone, request.zone);
                            }
                            break;
                        case TRAN_TYPE_GLIST_ZONE:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GLIST_ZONE, 1,
                                        request.zone, request.country);
                            }
                            break;
                        case TRAN_TYPE_REG:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_SUCCESSFUL_A, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_REG_SUCCESSFUL_B, 1);
                            }
                            break;
                        case TRAN_TYPE_UNREG:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_UNREG_SUCCESSFUL_A, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_UNREG_SUCCESSFUL_B, 1,
                                        request.a_no);
                            }
                            break;
                        case TRAN_TYPE_LINK:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_SUCCESSFUL_A, 1,
                                        request.gsm_num);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_LINK_SUCCESSFUL_B, 1,
                                        request.a_no);
                            }
                            break;
                        case TRAN_TYPE_CHECKLINK:
                            if (! request.silent) {
                                if (request.local_sim) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_LOCAL_SIM, 1,
                                            request.gsm_num);
                                } else {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK, 1,
                                            request.gsm_num);
                                }
                            }
                            break;
                        case TRAN_TYPE_CANCEL:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_SUCCESSFUL_A, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_CANCEL_SUCCESSFUL_B, 1);
                            }
                            break;
                        case TRAN_TYPE_YES:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_YES_SUCCESSFUL_A, 1,
                                        request.gsm_num);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_YES_SUCCESSFUL_B, 1,
                                        request.a_no);
                            }
                            break;
                    }
                    break;
                case DB_RETR_USURF_OK:
                    //-- send successful message here...
                    switch (request.tran_type) {
                        case TRAN_TYPE_ROAM_USURF_ON:
                            //--- call NF here...
                            request.result_code = nf_provision(request.a_no, request.service_id, request.duration);
                            LOG_DEBUG("%s: nf_provision: %d", __func__, request.result_code);
                            int usurf_status = conn.usurfActivation(&request);
                            LOG_DEBUG("%s: usurfActivation: %d", __func__, usurf_status);

                            if (usurf_status < 0) {
                                LOG_ERROR("%s: usurf_activation failed request id: %d, tran_type: %d, msisdn: %s, country: %s, duration: %d.", __func__,
                                        request.id, request.tran_type, request.a_no, request.country, request.duration);
                            }

                            switch (request.result_code) {
                                case 0:
                                    if (usurf_status < 0) {
                                        LOG_ERROR("%s: usurf_activation failed request id: %d, tran_type: %d, msisdn: %s, country: %s, duration: %d.", __func__,
                                                request.id, request.tran_type, request.a_no, request.country, request.duration);
                                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                                Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_UNSUCCESSFUL, 1);
                                    } else {
                                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                                Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_SUCCESSFUL, 1,
                                                request.partner, request.exptime, request.expdate);
                                    }
                                    break;
                                case -2:
                                    LOG_DEBUG("%s: nf_provision: insufficient balance", __func__);
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_INSUFF_BAL, 1);
                                    break;
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_DEACT_DUE_TO_INSUFF_BAL:
                    switch (request.tran_type) {
                        case TRAN_TYPE_GROAM_OFF:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_DEACT_DUE_TO_INSUFF_BAL, 1);
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_DEACT_DUE_TO_OVER_STAY:
                    switch (request.tran_type) {
                        case TRAN_TYPE_GROAM_OFF:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_DEACT_DUE_TO_OVER_STAY, 1);
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_CANCEL_DUE_TO_INACTIVITY:
                    switch (request.tran_type) {
                        case TRAN_TYPE_CANCEL:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_DUE_TO_INACTIVITY_A, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_CANCEL_DUE_TO_INACTIVITY_B, 1);
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_CANCEL_DUE_TO_INACTIVITY_WITH_SUBS:
                    switch (request.tran_type) {
                        case TRAN_TYPE_CANCEL:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_DUE_TO_INACTIVITY_A_WITH_SUBS, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_CANCEL_DUE_TO_INACTIVITY_B_WITH_SUBS, 1);
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_UNREG_CANCEL_WITH_SUBS:
                    switch (request.tran_type) {
                        case TRAN_TYPE_UNREG:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_UNREG_SUCCESSFUL_A_WITH_SUBS, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_UNREG_SUCCESSFUL_B_WITH_SUBS, 1);
                            }
                            break;
                        case TRAN_TYPE_CANCEL:
                            if (! request.silent) {
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_SUCCESSFUL_A_WITH_SUBS, 1);
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.gsm_num, SYSMSG_CANCEL_SUCCESSFUL_B_WITH_SUBS, 1);
                            }
                            break;
                    }
                    break;
                default:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_SYSTEM_ERROR, 1);
                    }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/

static void init_tran (OraDBRequest& conn, request_t& request)
{
    char msg[MAX_MESSAGE_LEN];
    char* token;
    char* pbuf;

    snprintf(msg, sizeof(msg), "%s", request.msg);

    //-- initialize status...
    request.status = TXN_STATUS_PROCESSED;

    if (! strcasecmp(request.request_origin, "SMS")) {
        //-- initialize customer_type...
        snprintf(request.customer_type, sizeof(request.customer_type), "%s", "DEFAULT");

        //-- initialize tran_type...
        request.tran_type = TRAN_TYPE_UNKNOWN;

        //-- get keyword
        token = strtok_r(msg, " ", &pbuf);
        if (token) {
            if (! strcasecmp(token, "GROAM")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "TRIGGER");

                //-- get sub_keyword
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (! strcasecmp(token, "ON")) {
                        request.tran_type = TRAN_TYPE_GROAM_ON;
                        //-- get date
                        token = strtok_r(NULL, " ", &pbuf);
                        if (token) {
                            snprintf(request.activation_date, sizeof(request.activation_date), "%s", token);
                            //-- get duration
                            token = strtok_r(NULL, " ", &pbuf);
                            if (token) {
                                request.duration = strtol(token, NULL, 10);
                            }
                        }
                    } else if (! strcasecmp(token, "OFF")) {
                        request.tran_type = TRAN_TYPE_GROAM_OFF;
                    } else if (! strcasecmp(token, "EXTEND")) {
                        request.tran_type = TRAN_TYPE_GROAM_EXTEND;
                        //-- get duration
                        token = strtok_r(NULL, " ", &pbuf);
                        if (token) {
                            request.duration = strtol(token, NULL, 10);
                        }
                    } else if (! strcasecmp(token, "STATUS")) {
                        request.tran_type = TRAN_TYPE_GROAM_STATUS;
                        if (0 != csp_charge(request.a_no)) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_GROAM_STATUS_INSUFF_BAL, 1);
                        }
                    } else if (! strcasecmp(token, "HELP")) {
                        request.tran_type = TRAN_TYPE_GROAM_HELP;
                        if (0 != csp_charge(request.a_no)) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_GROAM_HELP_INSUFF_BAL, 1);
                        }
                    } else if (! strcasecmp(token, "YES")) {
                        request.tran_type = TRAN_TYPE_GROAM_YES;
                    } else if (! strcasecmp(token, "NO")) {
                        request.tran_type = TRAN_TYPE_GROAM_NO;
                    } else {
                        request.tran_type = TRAN_TYPE_UNKNOWN;
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_GROAM_HELP, 1);
                    }
                }
            } else if (! strcasecmp(token, "ROAM")) {
                //-- get USURF
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (! strcasecmp(token, "USURF")) {
                        //-- set country
                        snprintf(request.country, sizeof(request.country), "ALL", token);

                        //-- get duration
                        token = strtok_r(NULL, " ", &pbuf);
                        if (token && (strchr(token, 'D') || strchr(token, 'd'))) {
                            request.duration = strtol(token, NULL, 10);

                            //-- get command
                            token = strtok_r(NULL, " ", &pbuf);
                            if (token) {
                                if (! strcasecmp(token, "ON")) {
                                    request.tran_type = TRAN_TYPE_ROAM_USURF_ON;
                                } else if (! strcasecmp(token, "STATUS")) {
                                    request.tran_type = TRAN_TYPE_ROAM_USURF_STATUS;
                                } else {
                                    request.tran_type = TRAN_TYPE_UNKNOWN;
                                    request.status = TXN_STATUS_ERROR;
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_HELP, 1);
                                }
                            } else {
                                request.tran_type = TRAN_TYPE_UNKNOWN;
                                request.status = TXN_STATUS_ERROR;
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_HELP, 1);
                            }
                        } else {
                            request.tran_type = TRAN_TYPE_UNKNOWN;
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_HELP, 1);
                        }
                    } else {
                        request.tran_type = TRAN_TYPE_UNKNOWN;
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_HELP, 1);
                    }
                } else {
                    request.tran_type = TRAN_TYPE_UNKNOWN;
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_HELP, 1);
                }
            } else if (! strcasecmp(token, "HELP")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");

                request.tran_type = TRAN_TYPE_HELP;
                request.status = TXN_STATUS_SUCCESSFUL;
                send_system_msg(request.customer_type, request.tran_type, request.id,
                        Config::getAccessCode(), request.a_no, SYSMSG_HELP, 1);
            } else if (! strcasecmp(token, "REG")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");

                request.tran_type = TRAN_TYPE_REG;
                //-- get mobtel
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (*token != '0') {
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_REG_UNSUCCESSFUL, 1);
                    } else {
                        if (strlen(token) != 11) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_REG_UNSUCCESSFUL, 1);
                        } else {
                            snprintf(request.gsm_num, sizeof(request.gsm_num), "63%s", token+1);
                        }
                    }
                } else {
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_REG_UNSUCCESSFUL, 1);
                }
            } else if (! strcasecmp(token, "UNREG")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");

                request.tran_type = TRAN_TYPE_UNREG;
                //-- get mobtel
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (*token != '0') {
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_UNREG_UNSUCCESSFUL, 1);
                    } else {
                        if (strlen(token) != 11) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_UNREG_UNSUCCESSFUL, 1);
                        } else {
                            snprintf(request.gsm_num, sizeof(request.gsm_num), "63%s", token+1);
                        }
                    }
                } else {
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_UNREG_UNSUCCESSFUL, 1);
                }
            } else if (! strcasecmp(token, "LINK")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");

                request.tran_type = TRAN_TYPE_LINK;
                //-- get mobtel
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (*token != '0') {
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_LINK_UNSUCCESSFUL, 1);
                    } else {
                        if (strlen(token) != 11) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_LINK_UNSUCCESSFUL, 1);
                        } else {
                            snprintf(request.gsm_num, sizeof(request.gsm_num), "63%s", token+1);
                        }
                    }
                } else {
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_LINK_UNSUCCESSFUL, 1);
                }
            } else if (! strcasecmp(token, "CHECKLINK")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");

                request.tran_type = TRAN_TYPE_CHECKLINK;
                //-- get mobtel
                token = strtok_r(NULL, " ", &pbuf);
#if 0
                if (token) {
                    if (*token != '0') {
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_HELP, 1);
                    } else {
                        if (strlen(token) != 11) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_HELP, 1);
                        } else {
                            snprintf(request.gsm_num, sizeof(request.gsm_num), "63%s", token+1);
                        }
                    }
                } else {
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_HELP, 1);
                }
#else
                if (token) {
                    if ('0' == *token && 11 == strlen(token)) {
                        snprintf(request.gsm_num, sizeof(request.gsm_num), "63%s", token+1);
                    } else {
                        snprintf(request.gsm_num, sizeof(request.gsm_num), "%s", token);
                    }
                }
#endif
            } else if (! strcasecmp(token, "CANCEL")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");

                request.tran_type = TRAN_TYPE_CANCEL;
                //-- get mobtel
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (*token != '0') {
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_UNSUCCESSFUL, 1);
                    } else {
                        if (strlen(token) != 11) {
                            request.status = TXN_STATUS_ERROR;
                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                    Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_UNSUCCESSFUL, 1);
                        } else {
                            snprintf(request.gsm_num, sizeof(request.gsm_num), "63%s", token+1);
                        }
                    }
                } else {
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_UNSUCCESSFUL, 1);
                }
            } else if (! strcasecmp(token, "GLIST")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "TRIGGER");

                request.tran_type = TRAN_TYPE_GLIST;
                if (0 != csp_charge(request.a_no)) {
                    request.status = TXN_STATUS_ERROR;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_GLIST_INSUFF_BAL, 1);
                } else {
                    //-- get sub_keyword
                    token = strtok_r(NULL, " ", &pbuf);
                    if (token) {
                        if (! strcasecmp(token, "ZONE")) {
                            request.tran_type = TRAN_TYPE_GLIST_ZONE;
                            //-- get zone
                            token = strtok_r(NULL, " ", &pbuf);
                            if (token) {
                                request.zone = strtol(token, NULL, 10);
                            }
                        } else {
                            //-- sub_keyword is the country...
                            snprintf(request.country, sizeof(request.country), "%s", token);
                        }
                    } else {
                        request.status = TXN_STATUS_ERROR;
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_GLIST_HELP, 1);
                    }
                }
            } else if (! strcasecmp(token, "YES")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "FUN");
                request.tran_type = TRAN_TYPE_YES;
            } else {
                request.tran_type = TRAN_TYPE_UNKNOWN;
                request.status = TXN_STATUS_ERROR;
                send_system_msg(request.customer_type, request.tran_type, request.id,
                        Config::getAccessCode(), request.a_no, SYSMSG_HELP, 1);
            }
        } else {
            request.status = TXN_STATUS_ERROR;
            send_system_msg(request.customer_type, request.tran_type, request.id,
                    Config::getAccessCode(), request.a_no, SYSMSG_HELP, 1);
        }
    } else if (! strcasecmp(request.request_origin, "WEB")
            || ! strcasecmp(request.request_origin, "ARDS")) {
        switch (request.tran_type) {
            case TRAN_TYPE_GROAM_ON:
                break;
            case TRAN_TYPE_GROAM_OFF:
                break;
            case TRAN_TYPE_GROAM_EXTEND:
                break;
            case TRAN_TYPE_CANCEL:
                break;
            case TRAN_TYPE_LINK:
                break;
            case TRAN_TYPE_REG:
                break;
            case TRAN_TYPE_UNREG:
                break;
            default:
                request.status = TXN_STATUS_ERROR;
                break;
        }
    }

    int last_step_no = request.step_no;
    int next_step_no = Global::getNextStepNo(last_step_no, request.tran_type);

    if (TXN_STATUS_PROCESSED == request.status) {
        int itStatus = conn.initTran(&request);

        //-- set error_code
        request.error_code = request.db_retr;

        if (itStatus < 0) {
            LOG_ERROR("%s: failed request id: %d, tran_type: %d, a_no: %s.", __func__,
                    request.id, request.tran_type, request.a_no);
            request.status = TXN_STATUS_ERROR;
            if (! request.silent) {
                send_system_msg(request.customer_type, request.tran_type, request.id,
                        Config::getAccessCode(), request.a_no, SYSMSG_SYSTEM_ERROR, 1);
            }
        } else {
            LOG_INFO("%s: retr: %d, request id: %d, tran_type: %d, a_no: %s, b_no: %s, gsm_num: %s"
                    ", min_bal: %d, activation_date: %s, deactivation_date: %s, duration: %d, silent: %d", __func__
                    , request.db_retr, request.id, request.tran_type, request.a_no, request.b_no, request.gsm_num
                    , request.min_bal , request.activation_date, request.deactivation_date, request.duration, request.silent);

            switch (request.db_retr) {
                case DB_RETR_OK:
                    request.status = TXN_STATUS_SUCCESSFUL;
                    switch (request.tran_type) {
                        case TRAN_TYPE_GROAM_ON:
                            if (next_step_no < 0) {
                                request.status = TXN_STATUS_ERROR;
                                if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_UNSUCCESSFUL, 1);
                                }
                            } else {
                                if (request.min_bal > 0) {
                                    float balance = getBalance(request.a_no);
                                    LOG_DEBUG("%s: GET BALANCE: bal: %0.2f, min_bal: %d", __func__, balance, request.min_bal);
                                    if (balance < (float)0) {
                                        next_step_no = -1;
                                        request.status = TXN_STATUS_ERROR;
                                        if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                                    Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_UNSUCCESSFUL, 1);
                                        }
                                    } else if (balance < (float)request.min_bal) {
                                        next_step_no = -1;
                                        request.status = TXN_STATUS_ERROR;
                                        if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                                    Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_INSUFF_BAL, 1,
                                                    request.min_bal);
                                        }
                                        request.error_code = DB_RETR_INSUFFICIENT_BALANCE;
                                    }
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_OFF:
                            if (next_step_no < 0) {
                                request.status = TXN_STATUS_ERROR;
                                if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_UNSUCCESSFUL, 1);
                                }
                            } else {
                                if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_PRE_DEACT, 1);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_NO:
                            if (next_step_no < 0) {
                                request.status = TXN_STATUS_ERROR;
                            } else {
                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_NO_PRE_DEACT, 1);
                                }
                            }
                            break;
                        case TRAN_TYPE_GROAM_EXTEND:
                            if (0 != csp_charge(request.a_no)) {
                                request.status = TXN_STATUS_ERROR;
                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_INSUFF_BAL, 1);
                                }
                            } else {
                                if (request.min_bal > 0) {
                                    float balance = getBalance(request.a_no);
                                    LOG_DEBUG("%s: GET BALANCE: bal: %0.2f, min_bal: %d", __func__, balance, request.min_bal);
                                    if (balance < (float)0) {
                                        request.status = TXN_STATUS_ERROR;
                                        if (! request.silent) {
                                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                                    Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_UNSUCCESSFUL, 1);
                                        }
                                    } else if (balance < (float)request.min_bal) {
                                        request.status = TXN_STATUS_ERROR;
                                        if (! request.silent) {
                                            send_system_msg(request.customer_type, request.tran_type, request.id,
                                                    Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_INSUFF_BAL, 1);
                                        }
                                        request.error_code = DB_RETR_INSUFFICIENT_BALANCE;
                                    }
                                }
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_PRE_ACTIVATION:
                    request.status = TXN_STATUS_SUCCESSFUL;
                    break;
                case DB_RETR_OK_DEACT_DUE_TO_INSUFF_BAL:
                    request.status = TXN_STATUS_SUCCESSFUL;
                    switch (request.tran_type) {
                        case TRAN_TYPE_GROAM_OFF:
                            if (next_step_no < 0) {
                                request.status = TXN_STATUS_ERROR;
                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_UNSUCCESSFUL, 1);
                                }
                            } else {
                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_PRE_DEACT_DUE_TO_INSUFF_BAL, 1);
                                }
                            }
                            break;
                    }
                    break;
                case DB_RETR_OK_DEACT_DUE_TO_OVER_STAY:
                    request.status = TXN_STATUS_SUCCESSFUL;
                    switch (request.tran_type) {
                        case TRAN_TYPE_GROAM_OFF:
                            if (next_step_no < 0) {
                                request.status = TXN_STATUS_ERROR;
                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_UNSUCCESSFUL, 1);
                                }
                            } else {
                                if (! request.silent) {
                                    send_system_msg(request.customer_type, request.tran_type, request.id,
                                            Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_PRE_DEACT_DUE_TO_OVER_STAY, 1);
                                }
                            }
                            break;
                    }
                    break;
                case DB_RETR_BLACKLISTED:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_BLACKLISTED, 1);
                    }
                    break;
                case DB_RETR_INVALID_DURATION:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_INVALID_DURATION, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_NO_ACTIVE_ROAMING:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_OFF:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_NO_ACTIVE_ROAM, 1);
                                break;
                            case TRAN_TYPE_GROAM_EXTEND:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_NO_ACTIVE_ROAM, 1);
                                break;
                            case TRAN_TYPE_GROAM_STATUS:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_STATUS_NO_ACTIVE_ROAM, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_NO_ACTIVE_FUN_PROFILE:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_UNREG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_UNREG_NO_ACTIVE_FUN_PROFILE, 1);
                                break;
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_NO_ACTIVE_FUN_PROFILE, 1);
                                break;
                            case TRAN_TYPE_CHECKLINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_NO_ACTIVE_FUN_PROFILE, 1);
                                break;
                            case TRAN_TYPE_CANCEL:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_NO_ACTIVE_FUN_PROFILE, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_INVALID_ACTIVATION_DATE_FORMAT:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_INVALID_DATE_FORMAT, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_INVALID_PRE_ACTIVATION_DATE:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_INVALID_PRE_ACT_DATE, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_DUPLICATE_PRE_ACTIVATION:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_INVALID_DUP_PRE_ACT, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_MAX_NUM_LINK_REACHED:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_UNSUCCESSFUL_MAX_LINK, 1);
                                break;
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_UNSUCCESSFUL_MAX_LINK, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_INSUFFICIENT_BALANCE:
                    request.status = TXN_STATUS_ERROR;
                    LOG_DEBUG("%s: Unhandled db retr: DB_RETR_INSUFFICIENT_BALANCE: %d", __func__, DB_RETR_INSUFFICIENT_BALANCE);
                    break;
                case DB_RETR_ALREADY_ROAMER:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_ALREADY_ROAMER, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_ALREADY_OFW_SIM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_ALREADY_OFW_SIM, 1);
                                break;
                            case TRAN_TYPE_REG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_ALREADY_LINKED, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_INVALID_DURATION_EXT:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_EXTEND:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_INVALID_DURATION, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_ALREADY_LINKED:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_ALREADY_LINKED, 1);
                                break;
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_ALREADY_LINKED, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_NOT_IN_HLR_RANGE:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                            case TRAN_TYPE_GROAM_OFF:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_NOT_IN_HLR_RANGE, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_LOCAL_SIM_ALREADY_LINKED:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_LOCAL_SIM_ALREADY_LINKED, 1);
                                break;
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_LOCAL_SIM_ALREADY_LINKED, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_NOT_IN_CUSTTYPE_RANGE:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_NOT_IN_CUSTTYPE_RANGE, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_LOCAL_SIM_SAME_OFW_SIM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                            case TRAN_TYPE_UNREG:
                            case TRAN_TYPE_LINK:
                            case TRAN_TYPE_CANCEL:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_LOCAL_SIM_SAME_OFW_SIM, 1);
                                break;
                            case TRAN_TYPE_CHECKLINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_LOCAL_SIM_SAME_OFW_SIM, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_FOR_PREPAID_ONLY:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_FOR_PREPAID_ONLY, 1);
                                break;
                            case TRAN_TYPE_UNREG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_UNREG_FOR_PREPAID_ONLY, 1);
                                break;
                            case TRAN_TYPE_CANCEL:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_FOR_PREPAID_ONLY, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_LINK_TO_POSTPAID_ERROR:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                            case TRAN_TYPE_LINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_LINK_TO_POSTPAID_ERROR, 1);
                                break;
                            case TRAN_TYPE_UNREG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_UNREG_NOT_IN_CUSTTYPE_RANGE, 1);
                                break;
                            case TRAN_TYPE_CANCEL:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_LINK_NOT_IN_CUSTTYPE_RANGE, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_BREATHING_PERIOD:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent && ! strcasecmp(request.request_origin, "SMS")) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_BREATHING_PERIOD, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_ROAMING_W_OPEN_DURATION:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_GROAM_EXTEND:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_GROAM_EXTEND_ROAMING_W_OPEN_DURATION, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_NO_ACTIVE_LINK:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_YES:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_YES_NO_ACTIVE_LINK, 1);
                                break;
                            case TRAN_TYPE_CHECKLINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_HELP, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_LINKED_TO_OTHER_SIM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_CHECKLINK:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CHECKLINK_LINKED_TO_OTHER_SIM, 1,
                                        request.others);
                                break;
                        }
                    }
                    break;
                case DB_RETR_OFW_SIM_ALREADY_LINKED_AS_LOCAL:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_REG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_REG_OFW_SIM_ALREADY_LINKED_AS_LOCAL, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_OFW_SIM_IS_LOCAL_SIM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_UNREG:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_UNREG_OFW_SIM_IS_LOCAL_SIM, 1);
                                break;
                            case TRAN_TYPE_CANCEL:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_CANCEL_OFW_SIM_IS_LOCAL_SIM, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_USURF_W_ACTIVE_ROAM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_ROAM_USURF_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_W_ACTIVE_ROAM, 1);
                                break;
                            case TRAN_TYPE_ROAM_USURF_STATUS:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_STATUS_W_ACTIVE_ROAM, 1,
                                        request.partner, request.exptime, request.expdate);
                                break;
                        }
                    }
                    break;
                case DB_RETR_USURF_INVALID_COUNTRY:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_INVALID_COUNTRY, 1);
                    }
                    break;
                case DB_RETR_USURF_INVALID_DURATION:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_INVALID_DURATION, 1);
                    }
                    break;
                case DB_RETR_USURF_WO_ACTIVE_ROAM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_STATUS_WO_ACTIVE_ROAM, 1);
                    }
                    break;
                case DB_RETR_USURF_W_PENDING_ROAM:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        switch (request.tran_type) {
                            case TRAN_TYPE_ROAM_USURF_ON:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_W_PENDING_ROAM, 1);
                                break;
                            case TRAN_TYPE_ROAM_USURF_STATUS:
                                send_system_msg(request.customer_type, request.tran_type, request.id,
                                        Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_STATUS_W_PENDING_ROAM, 1);
                                break;
                        }
                    }
                    break;
                case DB_RETR_USURF_OK:
                    request.status = TXN_STATUS_SUCCESSFUL;
                    break;
                case DB_RETR_USURF_PRE_ACT:
                    request.status = TXN_STATUS_SUCCESSFUL;
                    send_system_msg(request.customer_type, request.tran_type, request.id,
                            Config::getAccessCode(), request.a_no, SYSMSG_ROAM_USURF_ON_PRE_ACT_SUCCESSFUL, 1);
                    break;
                default:
                    request.status = TXN_STATUS_ERROR;
                    if (! request.silent) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_SYSTEM_ERROR, 1);
                    }
            }
        }
    }

    request.step_no = next_step_no;
    request.last_step_no = last_step_no;
}

/*----------------------------------------------------------------------------*/

static bool pending_tran (request_t& request)
{
    bool retr = false;

    if (request.status != TXN_STATUS_PENDING) {
        return retr;
    }
    
    if (! strcasecmp(request.request_origin, "SMS")) {
        char msg[MAX_MESSAGE_LEN];
        char* token;
        char* pbuf;

        snprintf(msg, sizeof(msg), "%s", request.msg);
        //-- get keyword
        token = strtok_r(msg, " ", &pbuf);
        if (token) {
            if (! strcasecmp(token, "GROAM")) {
                snprintf(request.customer_type, sizeof(request.customer_type), "%s", "TRIGGER");

                //-- get sub_keyword
                token = strtok_r(NULL, " ", &pbuf);
                if (token) {
                    if (! strcasecmp(token, "ON")) {
                        request.tran_type = TRAN_TYPE_GROAM_ON;
                        request.ref_id = request.id;

                        //-- send successful message
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_SUCCESSFUL, 1);

                        //-- modify msg for enrollment_type
                        snprintf(request.msg, sizeof(request.msg), "ACTIVATION");
                        retr = true;
#if 0
                    } else if (! strcasecmp(token, "OFF")) {
                        request.tran_type = TRAN_TYPE_GROAM_OFF;
                        request.ref_id = request.id;

                        //-- modify msg for enrollment_type
                        snprintf(request.msg, sizeof(request.msg), "DEACTIVATION");
                        retr = true;
#endif
                    }
                }
            }
        }
    }

    if (retr && ! *(request.imsi)) {
        //-- get imsi
        getIMSI(request.a_no, request.imsi, sizeof(request.imsi));
    }

    return retr;
}

/*============================================================================*/

void* raw_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBRequest::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<request_t> requests;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        requests.clear();

        if (conn.getRequests(&requests, Config::getClusterNode(), TXN_STEP_INITIAL, TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (requests.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)requests.size(); ++i) {
            request_t& request = requests[i];

            LOG_DEBUG("%s: request id: %d, msg: %s, a_no: %s, b_no: %s, step_no: %d", __func__,
                    request.id, request.msg, request.a_no, request.b_no, request.step_no);

            if (Config::bypassARDS()) {
                request.status = TXN_STATUS_PROCESSED;
            } else {
                request.status = TXN_STATUS_PENDING;
            }
            
            if (conn.updateRequest(&request) < 0) {
                LOG_ERROR("%s: Unable to update request id: %d, status: %d.", __func__, request.id, request.status);
            } else {
                if (0 != c2q_enqueue(Global::getRequestQ(), (void*) &request, sizeof(request_t))) {
                    LOG_ERROR("%s: Unable to insert to request queue id: %d.", __func__, request.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* raw_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDB::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        request_t request;

        while (! c2q_dequeue(Global::getRequestQ(), &request, sizeof(request_t))) {
            LOG_DEBUG("%s: %d: request id: %d, msg: %s, a_no: %s, b_no: %s, tran_type: %d, request_origin: %s", __func__, proc_id,
                    request.id, request.msg, request.a_no, request.b_no, request.tran_type, request.request_origin);

            if (pending_tran(request)) {
                if (conn.insertKeywordRequest(&request) < 0) {
                    LOG_ERROR("%s: %d: Unable to insert pending request id: %d.", __func__, proc_id, request.id);
                }
            } else {
                if (0 != c2q_enqueue(Global::getTransactionQ(), (void*) &request, sizeof(request_t))) {
                    LOG_ERROR("%s: %d: Unable to insert to transaction queue id: %d.", __func__, proc_id, request.id);
                }
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* request_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBRequest::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<request_t> requests;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        requests.clear();

        if (conn.getRequests(&requests, Config::getClusterNode(), TXN_STEP_INITIAL, TXN_STATUS_CONFIRMED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (requests.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)requests.size(); ++i) {
            request_t& request = requests[i];

            LOG_DEBUG("%s: request id: %d, msg: %s, a_no: %s, b_no: %s, step_no: %d", __func__,
                    request.id, request.msg, request.a_no, request.b_no, request.step_no);

            request.status = TXN_STATUS_PROCESSED;

            if (conn.updateRequest(&request) < 0) {
                LOG_ERROR("%s: Unable to update request id: %d, status: %d.", __func__, request.id, request.status);
            } else {
                if (0 != c2q_enqueue(Global::getTransactionQ(), (void*) &request, sizeof(request_t))) {
                    LOG_ERROR("%s: Unable to insert to transaction queue id: %d.", __func__, request.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* request_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDB::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        request_t request;

        while (! c2q_dequeue(Global::getTransactionQ(), &request, sizeof(request_t))) {
            LOG_DEBUG("%s: %d: request id: %d, msg: %s, a_no: %s, b_no: %s, tran_type: %d, request_origin: %s", __func__, proc_id,
                    request.id, request.msg, request.a_no, request.b_no, request.tran_type, request.request_origin);

            //-- Process request here...
            init_tran(conn, request);
            process_tran(conn, request);

            if (conn.updateRequest(&request) < 0) {
                LOG_ERROR("%s: %d: Unable to update request id: %d, status: %d.", __func__, proc_id, request.id, request.status);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* transaction_fetcher (void* arg)
{
    void* retr = NULL;
    rc_proc_t* rc_proc = (rc_proc_t*) arg;
    rc_t* rc = rc_proc->rc;
    int proc_id = rc_proc->proc_id;
    pid_t ppid = rc_proc->ppid;

    if (OraDBRequest::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    // Thread attributes
    pthread_attr_t pthread_attr_norm;
    pthread_attr_init(&pthread_attr_norm);
    pthread_attr_setstacksize(&pthread_attr_norm, Global::thr_stack_size);

    std::vector<pthread_t> thrs;

    // Transaction handler
    for (int i=0; i<rc->thread_count; ++i) {
        rc_proc_t* rc_proc = new(rc_proc_t);
        rc_proc->rc = rc;
        rc_proc->proc_id = i;
        rc_proc->parent_id = proc_id;
        rc_proc->ppid = ppid;
        pthread_t tran_thr;
        if (0 != pthread_create(&tran_thr, &pthread_attr_norm, transaction_handler, (void*) rc_proc)) {
            LOG_CRITICAL("%s: %d: Unable to create transaction_handler thread (%d)!!!", __func__, proc_id, i);
            conn.destroy_db();
            DO_ABORT();
            return retr;
        }
        thrs.push_back(tran_thr);
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    std::vector<request_t> requests;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        requests.clear();

        int step_no = rc->step_no;
        int status = TXN_STATUS_SUCCESSFUL;

        if (conn.getRequests(&requests, Config::getClusterNode(), step_no, status, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (requests.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)requests.size(); ++i) {
            request_t& request = requests[i];
            request.status = TXN_STATUS_PROCESSED;

            LOG_DEBUG("%s: %d: request id: %d, msg: %s, a_no: %s, b_no: %s, step_no: %d, last_step_no: %d, status: %d"
                    ", tran_type: %d, customer_type: %s, min_bal: %d, error_code: %d, imsi: %s, activation_date: %s"
                    ", deactivation_date: %s, duration: %d, gsm_num: %s, result_code: %d"
                    , __func__, proc_id
                    , request.id, request.msg, request.a_no, request.b_no, request.step_no, request.last_step_no, request.status
                    , request.tran_type, request.customer_type, request.min_bal, request.error_code, request.imsi, request.activation_date
                    , request.deactivation_date, request.duration, request.gsm_num, request.result_code);

            if (conn.updateRequest(&request) < 0) {
                LOG_ERROR("%s: %d: Unable to update request id: %d, status: %d.", __func__, proc_id, request.id, request.status);
            } else {
                if (0 != c2q_enqueue(Global::getTransactionQ(proc_id), (void*) &request, sizeof(request_t))) {
                    LOG_ERROR("%s: %d: Unable to insert to transaction queue id: %d.", __func__, proc_id, request.id);
                }
            }
        }
    }

#if 0
    for (int i=0; i<(int)thrs.size(); ++i) {
        pthread_join(thrs[i], NULL);
    }
#endif

    while (! thrs.empty()) {
        pthread_join(thrs.back(), NULL);
        thrs.pop_back();
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* transaction_handler (void* arg)
{
    void* retr = NULL;
    rc_proc_t* rc_proc = (rc_proc_t*) arg;
    rc_t* rc = rc_proc->rc;
    int proc_id = rc_proc->proc_id;
    int parent_id = rc_proc->parent_id;

    const char *dlsym_error;
    void *libh = dlopen(rc->libpath, RTLD_LAZY|RTLD_LOCAL);

    if (! libh) {
        LOG_CRITICAL("%s: %d-%d: Unable to load library %s: %s.", __func__, parent_id, proc_id, rc->libpath, dlerror());
        DO_ABORT();
        return retr;
    }

    Fun2RC_new* fun2rc_new = (Fun2RC_new*) dlsym(libh, "fun2rc_new");
    dlsym_error = dlerror();
    if (dlsym_error) {
        LOG_CRITICAL("%s: %d-%d: %s.", __func__, parent_id, proc_id, rc->libpath, dlsym_error);
        DO_ABORT();
        return retr;
    }

    Fun2RC_del* fun2rc_del = (Fun2RC_del*) dlsym(libh, "fun2rc_del");
    dlsym_error = dlerror();
    if (dlsym_error) {
        LOG_CRITICAL("%s: %d-%d: %s.", __func__, parent_id, proc_id, rc->libpath, dlsym_error);
        DO_ABORT();
        return retr;
    }

    Fun2RC* fun2rc = fun2rc_new();
    Fun2RC::param_t param;
    memset(&param, 0, sizeof(Fun2RC::param_t));

    snprintf(param.svc_url, sizeof(param.svc_url), "%s", rc->svc_url);
    snprintf(param.svc_name, sizeof(param.svc_name), "%s", rc->svc_name);
    snprintf(param.svc_host, sizeof(param.svc_host), "%s", rc->svc_host);
    snprintf(param.svc_user, sizeof(param.svc_user), "%s", rc->svc_user);
    snprintf(param.svc_pass, sizeof(param.svc_pass), "%s", rc->svc_pass);
    snprintf(param.db_host, sizeof(param.db_host), "%s", rc->db_host);
    snprintf(param.db_user, sizeof(param.db_user), "%s", rc->db_user);
    snprintf(param.db_pass, sizeof(param.db_pass), "%s", rc->db_pass);
    snprintf(param.expiry, sizeof(param.expiry), "%s", rc->expiry);
    param.svc_port = rc->svc_port;
    param.timeout = rc->timeout;
    param.logh = LOG_GET_HANDLE();

    int max_retry = rc->max_retry;

    LOG_DEBUG("%s: %d-%d: svc_url: %s, svc_host: %s, svc_port: %d, svc_user: %s, svc_pass: %s"
            ", db_host: %s, db_user: %s, db_pass: %s, timeout: %d", __func__, parent_id, proc_id,
            param.svc_url, param.svc_host, param.svc_port, param.svc_user, param.svc_pass,
            param.db_host, param.db_user, param.db_pass, param.timeout);

    fun2rc->initialize(&param);

    if (OraDB::init_lib() < 0) {
        LOG_CRITICAL("%s: %d-%d: Unable to initialize libsqlora8!", __func__, parent_id, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d-%d: Unable to connect to db (%s).", __func__, parent_id, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d-%d: Started.", __func__, parent_id, proc_id);

    while (! IS_SHUTDOWN()) {
        request_t request;

        while (! c2q_dequeue(Global::getTransactionQ(parent_id), &request, sizeof(request_t))) {
            LOG_DEBUG("%s: %d-%d: request id: %d, msg: %s, a_no: %s, b_no: %s, tran_type: %d, step_no: %d", __func__, parent_id, proc_id,
                    request.id, request.msg, request.a_no, request.b_no, request.tran_type, request.step_no);

            int res_code = -1;

            char tran_type[32];
            char start_time[32];
            char end_time[32];
            time_t t;
            struct tm lt;

            t = time(NULL);
            localtime_r(&t, &lt);
            strftime(start_time, sizeof(start_time), "%Y-%m-%d %H:%M:%S", &lt);

            switch (request.tran_type) {
                case TRAN_TYPE_GROAM_ON:
                    strcpy(tran_type, "Activation");

                    for (int i = 0; i < max_retry; ++i) {
                        res_code = fun2rc->activate(request.a_no);
                        LOG_INFO("%s: %d-%d: %s: ACTIVATION: try: %d, return: %d", __func__, parent_id, proc_id, rc->description, i, res_code);
                        if (0 <= res_code) {
                            break;
                        }
                    }
                    if (0 > res_code) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_GROAM_ON_UNSUCCESSFUL, 1);
                    }

                    //-- try getting the imsi...
                    if (! *(request.imsi)) {
                        fun2rc->getIMSI(request.a_no, request.imsi, sizeof(request.imsi));
                    }
                    break;
                case TRAN_TYPE_GROAM_OFF:
                    strcpy(tran_type, "Deactivation");

                    for (int i = 0; i < max_retry; ++i) {
                        res_code = fun2rc->deactivate(request.a_no);
                        LOG_INFO("%s: %d-%d: %s: DEACTIVATION: try: %d, return: %d", __func__, parent_id, proc_id, rc->description, i, res_code);
                        if (0 <= res_code) {
                            break;
                        }
                    }
                    if (0 > res_code) {
                        send_system_msg(request.customer_type, request.tran_type, request.id,
                                Config::getAccessCode(), request.a_no, SYSMSG_GROAM_OFF_UNSUCCESSFUL, 1);
                    }

                    //-- try getting the imsi...
                    if (! *(request.imsi)) {
                        fun2rc->getIMSI(request.a_no, request.imsi, sizeof(request.imsi));
                    }
                    break;
                case TRAN_TYPE_GROAM_NO:
                    strcpy(tran_type, "Deactivation");

                    for (int i = 0; i < max_retry; ++i) {
                        res_code = fun2rc->deactivate(request.a_no);
                        LOG_INFO("%s: %d-%d: %s: DEACTIVATION: try: %d, return: %d", __func__, parent_id, proc_id, rc->description, i, res_code);
                        if (0 <= res_code) {
                            break;
                        }
                    }
                    break;
                default:
                    strcpy(tran_type, "Unknown");

                    LOG_ERROR("%s: %d-%d: Unknown transaction type: %d", __func__, parent_id, proc_id, request.tran_type);
            }

            t = time(NULL);
            localtime_r(&t, &lt);
            strftime(end_time, sizeof(end_time), "%Y-%m-%d %H:%M:%S", &lt);

            LOG_INFO("STATS: %s: step_no: %d: request_id: %d, msisdn: %s, start_time: %s, end_time: %s, status: %s"
                    , tran_type, request.step_no, request.id, request.a_no, start_time, end_time, (0 > res_code) ? "Failed" : "Successful");

            int last_step_no = request.step_no;
            int next_step_no = Global::getNextStepNo(last_step_no, request.tran_type);

            request.step_no = next_step_no;
            request.last_step_no = last_step_no;

            if (0 > res_code) {
                request.status = TXN_STATUS_ERROR;
                request.result_code = res_code;
            } else {
                request.status = TXN_STATUS_SUCCESSFUL;
                if (0 < res_code) {
                    request.result_code = res_code;
                }
            }

            //-- Process request here...
            process_tran(conn, request);

            if (conn.updateRequest(&request) < 0) {
                LOG_ERROR("%s: %d-%d: Unable to update request id: %d, status: %d, result_code: %d", __func__, parent_id, proc_id,
                        request.id, request.status, request.result_code);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d-%d: Terminated.", __func__, parent_id, proc_id);

    conn.destroy_db();

    fun2rc->deinitialize();
    fun2rc_del(fun2rc);
    dlclose(libh);

    return retr;
}

/*----------------------------------------------------------------------------*/

void* response_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBResponse::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBResponse conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        response_t response;

        while (! c2q_dequeue(Global::getResponseQ(), &response, sizeof(response_t))) {
            LOG_DEBUG("%s: %d: response txid: %s, ref_id: %d, a_no: %s, b_no: %s, msg_id: %s, msg: %s", __func__, proc_id,
                    response.txid, response.ref_id, response.a_no, response.b_no, response.msg_id, response.msg);

            if (conn.insertResponse(&response) < 0) {
                LOG_ERROR("%s: %d: Unable to insert response ref_id: %d, a_no: %s, b_no: %s, msg: %s", __func__, proc_id,
                        response.ref_id, response.a_no, response.b_no, response.msg);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void notification_insert (int tran_type, int ref_id, const char* msisdn)
{
    notification_t notification;

    memset(&notification, 0, sizeof(notification_t));
    snprintf(notification.msisdn, sizeof(notification.msisdn), "%s", msisdn);
    notification.tran_type = tran_type;
    notification.ref_id = ref_id;
    notification.cluster_node = Config::getClusterNode();
    notification.status = TXN_STATUS_UNPROCESSED;

    if (0 != c2q_enqueue(Global::getNotificationQ(), (void*) &notification, sizeof(notification_t))) {
        LOG_ERROR("%s: Unable to insert to notification queue: tran_type: %d, ref_id: %d, msisdn: %s", __func__,
                notification.tran_type, notification.ref_id, notification.msisdn);
    }
}

/*----------------------------------------------------------------------------*/

void* notification_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

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
            LOG_DEBUG("%s: %d: notification tran_type: %d, ref_id: %d, msisdn: %s", __func__, proc_id,
                    notification.tran_type, notification.ref_id, notification.msisdn);

            if (conn.insertNotification(&notification) < 0) {
                LOG_ERROR("%s: %d: Unable to insert notification tran_type: %d, ref_id: %d, msisdn: %s", __func__, proc_id,
                        notification.tran_type, notification.ref_id, notification.msisdn);
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
