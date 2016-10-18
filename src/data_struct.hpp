/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : data_struct.hpp
 * Description : ...
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _DATA_STRUCT_HPP_
#define _DATA_STRUCT_HPP_

/*-defines--------------------------------------------------------------------*/

#define SYSMSG_SYSTEM_ERROR                        "SYSTEM_ERROR"
#define SYSMSG_HELP                                "HELP"

#define SYSMSG_REG_HELP                            "REG_HELP"
#define SYSMSG_REG_SUCCESSFUL                      "REG_SUCCESSFUL"
#define SYSMSG_REG_SUCCESSFUL_A                    "REG_SUCCESSFUL_A"
#define SYSMSG_REG_SUCCESSFUL_B                    "REG_SUCCESSFUL_B"
#define SYSMSG_REG_UNSUCCESSFUL                    "REG_UNSUCCESSFUL"
#define SYSMSG_REG_UNSUCCESSFUL_MAX_LINK           "REG_UNSUCCESSFUL_MAX_LINK"
#define SYSMSG_REG_WITH_ACTIVE_ROAMING             "REG_WITH_ACTIVE_ROAMING"
#define SYSMSG_REG_ALREADY_LINKED                  "REG_ALREADY_LINKED"
#define SYSMSG_REG_LOCAL_SIM_ALREADY_LINKED        "REG_LOCAL_SIM_ALREADY_LINKED"
#define SYSMSG_REG_NOT_IN_CUSTTYPE_RANGE           "REG_NOT_IN_CUSTTYPE_RANGE"
#define SYSMSG_REG_LOCAL_SIM_SAME_OFW_SIM          "REG_LOCAL_SIM_SAME_OFW_SIM"
#define SYSMSG_REG_FOR_PREPAID_ONLY                "REG_FOR_PREPAID_ONLY"
#define SYSMSG_REG_LINK_TO_POSTPAID_ERROR          "REG_LINK_TO_POSTPAID_ERROR"
#define SYSMSG_REG_OFW_SIM_ALREADY_LINKED_AS_LOCAL "REG_OFW_SIM_ALREADY_LINKED_AS_LOCAL"

#define SYSMSG_UNREG_HELP                          "UNREG_HELP"
#define SYSMSG_UNREG_SUCCESSFUL                    "UNREG_SUCCESSFUL"
#define SYSMSG_UNREG_SUCCESSFUL_A                  "UNREG_SUCCESSFUL_A"
#define SYSMSG_UNREG_SUCCESSFUL_B                  "UNREG_SUCCESSFUL_B"
#define SYSMSG_UNREG_SUCCESSFUL_A_WITH_SUBS        "UNREG_SUCCESSFUL_A_WITH_SUBS"
#define SYSMSG_UNREG_SUCCESSFUL_B_WITH_SUBS        "UNREG_SUCCESSFUL_B_WITH_SUBS"
#define SYSMSG_UNREG_UNSUCCESSFUL                  "UNREG_UNSUCCESSFUL"
#define SYSMSG_UNREG_NO_ACTIVE_FUN_PROFILE         "UNREG_NO_ACTIVE_FUN_PROFILE"
#define SYSMSG_UNREG_NOT_IN_CUSTTYPE_RANGE         "UNREG_NOT_IN_CUSTTYPE_RANGE"
#define SYSMSG_UNREG_FOR_PREPAID_ONLY              "UNREG_FOR_PREPAID_ONLY"
#define SYSMSG_UNREG_OFW_SIM_IS_LOCAL_SIM          "UNREG_OFW_SIM_IS_LOCAL_SIM"

#define SYSMSG_LINK_HELP                           "LINK_HELP"
#define SYSMSG_LINK_SUCCESSFUL                     "LINK_SUCCESSFUL"
#define SYSMSG_LINK_SUCCESSFUL_A                   "LINK_SUCCESSFUL_A"
#define SYSMSG_LINK_SUCCESSFUL_B                   "LINK_SUCCESSFUL_B"
#define SYSMSG_LINK_UNSUCCESSFUL                   "LINK_UNSUCCESSFUL"
#define SYSMSG_LINK_UNSUCCESSFUL_MAX_LINK          "LINK_UNSUCCESSFUL_MAX_LINK"
#define SYSMSG_LINK_NO_ACTIVE_FUN_PROFILE          "LINK_NO_ACTIVE_FUN_PROFILE"
#define SYSMSG_LINK_ALREADY_OFW_SIM                "LINK_ALREADY_OFW_SIM"
#define SYSMSG_LINK_ALREADY_LINKED                 "LINK_ALREADY_LINKED"
#define SYSMSG_LINK_LOCAL_SIM_ALREADY_LINKED       "LINK_LOCAL_SIM_ALREADY_LINKED"
#define SYSMSG_LINK_NOT_IN_CUSTTYPE_RANGE          "LINK_NOT_IN_CUSTTYPE_RANGE"

#define SYSMSG_CHECKLINK                           "CHECKLINK"
#define SYSMSG_CHECKLINK_LOCAL_SIM                 "CHECKLINK_LOCAL_SIM"
#define SYSMSG_CHECKLINK_HELP                      "CHECKLINK_HELP"
#define SYSMSG_CHECKLINK_NO_ACTIVE_FUN_PROFILE     "CHECKLINK_NO_ACTIVE_FUN_PROFILE"
#define SYSMSG_CHECKLINK_LINKED_TO_OTHER_SIM       "CHECKLINK_LINKED_TO_OTHER_SIM"
#define SYSMSG_CHECKLINK_LOCAL_SIM_SAME_OFW_SIM    "CHECKLINK_LOCAL_SIM_SAME_OFW_SIM"

#define SYSMSG_CANCEL_HELP                           "CANCEL_HELP"
#define SYSMSG_CANCEL_SUCCESSFUL                     "CANCEL_SUCCESSFUL"
#define SYSMSG_CANCEL_SUCCESSFUL_A                   "CANCEL_SUCCESSFUL_A"
#define SYSMSG_CANCEL_SUCCESSFUL_B                   "CANCEL_SUCCESSFUL_B"
#define SYSMSG_CANCEL_SUCCESSFUL_A_WITH_SUBS         "CANCEL_SUCCESSFUL_A_WITH_SUBS"
#define SYSMSG_CANCEL_SUCCESSFUL_B_WITH_SUBS         "CANCEL_SUCCESSFUL_B_WITH_SUBS"
#define SYSMSG_CANCEL_UNSUCCESSFUL                   "CANCEL_UNSUCCESSFUL"
#define SYSMSG_CANCEL_NO_ACTIVE_FUN_PROFILE          "CANCEL_NO_ACTIVE_FUN_PROFILE"
#define SYSMSG_CANCEL_FOR_PREPAID_ONLY               "CANCEL_FOR_PREPAID_ONLY"
#define SYSMSG_CANCEL_DUE_TO_INACTIVITY_A            "CANCEL_DUE_TO_INACTIVITY_A"
#define SYSMSG_CANCEL_DUE_TO_INACTIVITY_B            "CANCEL_DUE_TO_INACTIVITY_B"
#define SYSMSG_CANCEL_DUE_TO_INACTIVITY_A_WITH_SUBS  "CANCEL_DUE_TO_INACTIVITY_A_WITH_SUBS"
#define SYSMSG_CANCEL_DUE_TO_INACTIVITY_B_WITH_SUBS  "CANCEL_DUE_TO_INACTIVITY_B_WITH_SUBS"
#define SYSMSG_CANCEL_OFW_SIM_IS_LOCAL_SIM           "CANCEL_OFW_SIM_IS_LOCAL_SIM"

#define SYSMSG_GROAM_HELP                          "GROAM_HELP"
#define SYSMSG_GROAM_HELP_INSUFF_BAL               "GROAM_HELP_INSUFF_BAL"

#define SYSMSG_GROAM_ON_SUCCESSFUL                 "GROAM_ON_SUCCESSFUL"
#define SYSMSG_GROAM_ON_SUCCESSFUL_W_RBT           "GROAM_ON_SUCCESSFUL_W_RBT"
#define SYSMSG_GROAM_ON_SUCCESSFUL_W_RBT_OPEN      "GROAM_ON_SUCCESSFUL_W_RBT_OPEN"
#define SYSMSG_GROAM_ON_SUCCESSFUL_WO_RBT          "GROAM_ON_SUCCESSFUL_WO_RBT"
#define SYSMSG_GROAM_ON_SUCCESSFUL_WO_RBT_OPEN     "GROAM_ON_SUCCESSFUL_WO_RBT_OPEN"
#define SYSMSG_GROAM_ON_PRE_ACT_SUCCESSFUL         "GROAM_ON_PRE_ACT_SUCCESSFUL"
#define SYSMSG_GROAM_ON_ACTIVATED                  "GROAM_ON_ACTIVATED"
#define SYSMSG_GROAM_ON_UNSUCCESSFUL               "GROAM_ON_UNSUCCESSFUL"
#define SYSMSG_GROAM_ON_INVALID_DURATION           "GROAM_ON_INVALID_DURATION"
#define SYSMSG_GROAM_ON_INSUFF_BAL                 "GROAM_ON_INSUFF_BAL"
#define SYSMSG_GROAM_ON_INVALID_DATE_FORMAT        "GROAM_ON_INVALID_DATE_FORMAT"
#define SYSMSG_GROAM_ON_INVALID_PRE_ACT_DATE       "GROAM_ON_INVALID_PRE_ACT_DATE"
#define SYSMSG_GROAM_ON_INVALID_DUP_PRE_ACT        "GROAM_ON_INVALID_DUP_PRE_ACT"
#define SYSMSG_GROAM_ON_ALREADY_ROAMER             "GROAM_ON_ALREADY_ROAMER"
#define SYSMSG_GROAM_ON_NOT_IN_HLR_RANGE           "GROAM_ON_NOT_IN_HLR_RANGE"
#define SYSMSG_GROAM_ON_BREATHING_PERIOD           "GROAM_ON_BREATHING_PERIOD"
#define SYSMSG_GROAM_ON_W_PENDING_PRE_ACT          "GROAM_ON_W_PENDING_PRE_ACT"

#define SYSMSG_GROAM_OFF_SUCCESSFUL                   "GROAM_OFF_SUCCESSFUL"
#define SYSMSG_GROAM_OFF_PRE_ACT_SUCCESSFUL           "GROAM_OFF_PRE_ACT_SUCCESSFUL"
#define SYSMSG_GROAM_OFF_UNSUCCESSFUL                 "GROAM_OFF_UNSUCCESSFUL"
#define SYSMSG_GROAM_OFF_NO_ACTIVE_ROAM               "GROAM_OFF_NO_ACTIVE_ROAM"
#define SYSMSG_GROAM_OFF_PRE_DEACT                    "GROAM_OFF_PRE_DEACT"
#define SYSMSG_GROAM_OFF_PRE_DEACT_WARNING            "GROAM_OFF_PRE_DEACT_WARNING"
#define SYSMSG_GROAM_OFF_PRE_DEACT_DUE_TO_INSUFF_BAL  "GROAM_OFF_PRE_DEACT_DUE_TO_INSUFF_BAL"
#define SYSMSG_GROAM_OFF_PRE_DEACT_DUE_TO_OVER_STAY   "GROAM_OFF_PRE_DEACT_DUE_TO_OVER_STAY"
#define SYSMSG_GROAM_OFF_DEACT_DUE_TO_INSUFF_BAL      "GROAM_OFF_DEACT_DUE_TO_INSUFF_BAL"
#define SYSMSG_GROAM_OFF_DEACT_DUE_TO_OVER_STAY       "GROAM_OFF_DEACT_DUE_TO_OVER_STAY"

#define SYSMSG_GROAM_EXTEND_SUCCESSFUL                  "GROAM_EXTEND_SUCCESSFUL"
#define SYSMSG_GROAM_EXTEND_SUCCESSFUL_W_OPEN_DURATION  "GROAM_EXTEND_SUCCESSFUL_W_OPEN_DURATION"
#define SYSMSG_GROAM_EXTEND_UNSUCCESSFUL                "GROAM_EXTEND_UNSUCCESSFUL"
#define SYSMSG_GROAM_EXTEND_NO_ACTIVE_ROAM              "GROAM_EXTEND_NO_ACTIVE_ROAM"
#define SYSMSG_GROAM_EXTEND_INSUFF_BAL                  "GROAM_EXTEND_INSUFF_BAL"
#define SYSMSG_GROAM_EXTEND_INVALID_DURATION            "GROAM_EXTEND_INVALID_DURATION"
#define SYSMSG_GROAM_EXTEND_ROAMING_W_OPEN_DURATION     "GROAM_EXTEND_ROAMING_W_OPEN_DURATION"

#define SYSMSG_GROAM_STATUS                        "GROAM_STATUS"
#define SYSMSG_GROAM_STATUS_UNLIMITED              "GROAM_STATUS_UNLIMITED"
#define SYSMSG_GROAM_STATUS_NO_ACTIVE_ROAM         "GROAM_STATUS_NO_ACTIVE_ROAM"
#define SYSMSG_GROAM_STATUS_INSUFF_BAL             "GROAM_STATUS_INSUFF_BAL"

#define SYSMSG_GLIST_WITH_ROAMING                  "GLIST_WITH_ROAMING"
#define SYSMSG_GLIST_WITHOUT_ROAMING               "GLIST_WITHOUT_ROAMING"
#define SYSMSG_GLIST_INSUFF_BAL                    "GLIST_INSUFF_BAL"
#define SYSMSG_GLIST_HELP                          "GLIST_HELP"
#define SYSMSG_GLIST_ZONE                          "GLIST_ZONE"

#define SYSMSG_BLACKLISTED                         "BLACKLISTED"

#define SYSMSG_DAILY_BALANCE                       "DAILY_BALANCE"
#define SYSMSG_DAILY_BALANCE_INSUFF_BAL            "DAILY_BALANCE_INSUFF_BAL"
#define SYSMSG_DAILY_BALANCE_INSUFF_BAL_DEACT      "DAILY_BALANCE_INSUFF_BAL_DEACT"

#define SYSMSG_GEO_PROBE                                       "GEO_PROBE"
#define SYSMSG_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY             "GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY"
#define SYSMSG_GEO_PROBE_CHANGE_TO_HOT_COUNTRY                 "GEO_PROBE_CHANGE_TO_HOT_COUNTRY"
#define SYSMSG_GEO_PROBE_EXCEED_STAY_LIMIT_ON_HOT_COUNTRY      "GEO_PROBE_EXCEED_STAY_LIMIT_ON_HOT_COUNTRY"
#define SYSMSG_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY_W_DURATION  "GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY_W_DURATION"

#define SYSMSG_YES_SUCCESSFUL_A                   "YES_SUCCESSFUL_A"
#define SYSMSG_YES_SUCCESSFUL_B                   "YES_SUCCESSFUL_B"
#define SYSMSG_YES_NO_ACTIVE_LINK                 "YES_NO_ACTIVE_LINK"

#define SYSMSG_ARDS_ROAM_ACTIVATED                "ARDS_ROAM_ACTIVATED"
#define SYSMSG_ARDS_ROAM_DEACTIVATED              "ARDS_ROAM_DEACTIVATED"
#define SYSMSG_GROAM_YES_SUCCESSFUL               "GROAM_YES_SUCCESSFUL"
#define SYSMSG_GROAM_NO_PRE_DEACT                 "GROAM_NO_PRE_DEACT"
#define SYSMSG_GROAM_NO_SUCCESSFUL                "GROAM_NO_SUCCESSFUL"

#define SYSMSG_ROAM_USURF_HELP                    "ROAM_USURF_HELP"

#define SYSMSG_ROAM_USURF_ON_SUCCESSFUL           "ROAM_USURF_ON_SUCCESSFUL"
#define SYSMSG_ROAM_USURF_ON_PRE_ACT_SUCCESSFUL   "ROAM_USURF_ON_PRE_ACT_SUCCESSFUL"
#define SYSMSG_ROAM_USURF_ON_UNSUCCESSFUL         "ROAM_USURF_ON_UNSUCCESSFUL"
#define SYSMSG_ROAM_USURF_ON_INVALID_COUNTRY      "ROAM_USURF_ON_INVALID_COUNTRY"
#define SYSMSG_ROAM_USURF_ON_INVALID_DURATION     "ROAM_USURF_ON_INVALID_DURATION"
#define SYSMSG_ROAM_USURF_ON_INSUFF_BAL           "ROAM_USURF_ON_INSUFF_BAL"
#define SYSMSG_ROAM_USURF_ON_INVALID_COEX         "ROAM_USURF_ON_INVALID_COEX"

#define SYSMSG_ROAM_USURF_OFF_PRE_ACT_SUCCESSFUL  "ROAM_USURF_OFF_PRE_ACT_SUCCESSFUL"
#define SYSMSG_ROAM_USURF_OFF_WO_ACTIVE_ROAM      "ROAM_USURF_OFF_WO_ACTIVE_ROAM"

#define SYSMSG_ROAM_USURF_STATUS_PRE_ACT_SUCCESSFUL  "ROAM_USURF_STATUS_PRE_ACT_SUCCESSFUL"
#define SYSMSG_ROAM_USURF_STATUS_WO_ACTIVE_ROAM      "ROAM_USURF_STATUS_WO_ACTIVE_ROAM"

#define SYSMSG_ROAM_SURF1D_ON_SUCCESSFUL          "ROAM_SURF1D_ON_SUCCESSFUL"
#define SYSMSG_ROAM_SURF3D_ON_SUCCESSFUL          "ROAM_SURF3D_ON_SUCCESSFUL"
#define SYSMSG_ROAM_SURF5D_ON_SUCCESSFUL          "ROAM_SURF5D_ON_SUCCESSFUL"
#define SYSMSG_ROAM_SURF299_ON_SUCCESSFUL         "ROAM_SURF299_ON_SUCCESSFUL"
#define SYSMSG_ROAM_SURF399_ON_SUCCESSFUL         "ROAM_SURF399_ON_SUCCESSFUL"
#define SYSMSG_ROAM_SURF499_ON_SUCCESSFUL         "ROAM_SURF499_ON_SUCCESSFUL"
#define SYSMSG_ROAM_VIBER_ON_SUCCESSFUL           "ROAM_VIBER_ON_SUCCESSFUL"
#define SYSMSG_ROAM_FACEBOOK_ON_SUCCESSFUL        "ROAM_FACEBOOK_ON_SUCCESSFUL"

#define SYSMSG_ROAM_SURF1D_OFF_SUCCESSFUL         "ROAM_SURF1D_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_SURF3D_OFF_SUCCESSFUL         "ROAM_SURF3D_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_SURF5D_OFF_SUCCESSFUL         "ROAM_SURF5D_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_SURF299_OFF_SUCCESSFUL        "ROAM_SURF299_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_SURF399_OFF_SUCCESSFUL        "ROAM_SURF399_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_SURF499_OFF_SUCCESSFUL        "ROAM_SURF499_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_VIBER_OFF_SUCCESSFUL          "ROAM_VIBER_OFF_SUCCESSFUL"
#define SYSMSG_ROAM_FACEBOOK_OFF_SUCCESSFUL       "ROAM_FACEBOOK_OFF_SUCCESSFUL"

#define SYSMSG_ROAM_SURF1D_STATUS_SUCCESSFUL      "ROAM_SURF1D_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_SURF3D_STATUS_SUCCESSFUL      "ROAM_SURF3D_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_SURF5D_STATUS_SUCCESSFUL      "ROAM_SURF5D_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_SURF299_STATUS_SUCCESSFUL     "ROAM_SURF299_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_SURF399_STATUS_SUCCESSFUL     "ROAM_SURF399_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_SURF499_STATUS_SUCCESSFUL     "ROAM_SURF499_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_VIBER_STATUS_SUCCESSFUL       "ROAM_VIBER_STATUS_SUCCESSFUL"
#define SYSMSG_ROAM_FACEBOOK_STATUS_SUCCESSFUL    "ROAM_FACEBOOK_STATUS_SUCCESSFUL"

#define SYSMSG_ROAM_SURF1D_ON_INVALID_COEX        "ROAM_SURF1D_ON_INVALID_COEX"
#define SYSMSG_ROAM_SURF3D_ON_INVALID_COEX        "ROAM_SURF3D_ON_INVALID_COEX"
#define SYSMSG_ROAM_SURF5D_ON_INVALID_COEX        "ROAM_SURF5D_ON_INVALID_COEX"
#define SYSMSG_ROAM_SURF299_ON_INVALID_COEX       "ROAM_SURF299_ON_INVALID_COEX"
#define SYSMSG_ROAM_SURF399_ON_INVALID_COEX       "ROAM_SURF399_ON_INVALID_COEX"
#define SYSMSG_ROAM_SURF499_ON_INVALID_COEX       "ROAM_SURF499_ON_INVALID_COEX"
#define SYSMSG_ROAM_VIBER_ON_INVALID_COEX         "ROAM_VIBER_ON_INVALID_COEX"
#define SYSMSG_ROAM_FACEBOOK_ON_INVALID_COEX      "ROAM_FACEBOOK_ON_INVALID_COEX"

#define SYSMSG_ROAM_SURF1D_OFF_INVALID            "ROAM_SURF1D_OFF_INVALID"
#define SYSMSG_ROAM_SURF3D_OFF_INVALID            "ROAM_SURF3D_OFF_INVALID"
#define SYSMSG_ROAM_SURF5D_OFF_INVALID            "ROAM_SURF5D_OFF_INVALID"
#define SYSMSG_ROAM_SURF299_OFF_INVALID           "ROAM_SURF299_OFF_INVALID"
#define SYSMSG_ROAM_SURF399_OFF_INVALID           "ROAM_SURF399_OFF_INVALID"
#define SYSMSG_ROAM_SURF499_OFF_INVALID           "ROAM_SURF499_OFF_INVALID"
#define SYSMSG_ROAM_VIBER_OFF_INVALID             "ROAM_VIBER_OFF_INVALID"
#define SYSMSG_ROAM_FACEBOOK_OFF_INVALID          "ROAM_FACEBOOK_OFF_INVALID"

#define SYSMSG_ROAM_SURF1D_STATUS_INVALID         "ROAM_SURF1D_STATUS_INVALID"
#define SYSMSG_ROAM_SURF3D_STATUS_INVALID         "ROAM_SURF3D_STATUS_INVALID"
#define SYSMSG_ROAM_SURF5D_STATUS_INVALID         "ROAM_SURF5D_STATUS_INVALID"
#define SYSMSG_ROAM_SURF299_STATUS_INVALID        "ROAM_SURF299_STATUS_INVALID"
#define SYSMSG_ROAM_SURF399_STATUS_INVALID        "ROAM_SURF399_STATUS_INVALID"
#define SYSMSG_ROAM_SURF499_STATUS_INVALID        "ROAM_SURF499_STATUS_INVALID"
#define SYSMSG_ROAM_VIBER_STATUS_INVALID          "ROAM_VIBER_STATUS_INVALID"
#define SYSMSG_ROAM_FACEBOOK_STATUS_INVALID       "ROAM_FACEBOOK_STATUS_INVALID"

enum tran_type {
    TRAN_TYPE_UNKNOWN,           //-- 0
    TRAN_TYPE_HELP,              //-- 1
    TRAN_TYPE_REG,               //-- 2
    TRAN_TYPE_UNREG,             //-- 3
    TRAN_TYPE_LINK,              //-- 4
    TRAN_TYPE_CHECKLINK,         //-- 5
    TRAN_TYPE_CANCEL,            //-- 6
    TRAN_TYPE_GROAM_HELP,        //-- 7
    TRAN_TYPE_GROAM_ON,          //-- 8
    TRAN_TYPE_GROAM_OFF,         //-- 9
    TRAN_TYPE_GROAM_EXTEND,      //-- 10
    TRAN_TYPE_GROAM_STATUS,      //-- 11
    TRAN_TYPE_GLIST,             //-- 12
    TRAN_TYPE_GLIST_ZONE,        //-- 13
    TRAN_TYPE_DAILY_BALANCE,     //-- 14
    TRAN_TYPE_GEO_PROBE,         //-- 15
    TRAN_TYPE_BROADCAST,         //-- 16
    TRAN_TYPE_YES,               //-- 17
    TRAN_TYPE_GROAM_YES,         //-- 18
    TRAN_TYPE_GROAM_NO,          //-- 19
    TRAN_TYPE_ROAM_USURF_ON,     //-- 20
    TRAN_TYPE_ROAM_USURF_OFF,    //-- 21
    TRAN_TYPE_ROAM_USURF_STATUS  //-- 22
};

#define TXN_STEP_INITIAL     0
#define MAX_MESSAGE_LEN      1024
#define MAX_DB_COMMAND_RETRY 3

enum result_code {
    RESULT_CODE_W_RBT = 110,
    RESULT_CODE_WO_RBT = 100
};

enum txn_status {
    TXN_STATUS_ERROR       = -1,
    TXN_STATUS_UNPROCESSED =  0,
    TXN_STATUS_PROCESSED   =  1,
    TXN_STATUS_SUCCESSFUL  =  2,
    TXN_STATUS_PENDING     =  3,
    TXN_STATUS_CONFIRMED   =  4
};

enum db_retr {
    DB_RETR_OK = 1,
    DB_RETR_OK_PRE_ACTIVATION = 2,
    DB_RETR_OK_DEACT_DUE_TO_INSUFF_BAL = 3,
    DB_RETR_OK_DEACT_DUE_TO_OVER_STAY = 4,
    DB_RETR_OK_CANCEL_DUE_TO_INACTIVITY = 5,
    DB_RETR_OK_CANCEL_DUE_TO_INACTIVITY_WITH_SUBS = 6,
    DB_RETR_OK_UNREG_CANCEL_WITH_SUBS = 7,
    DB_RETR_BLACKLISTED = 100,
    DB_RETR_INVALID_DURATION = 101,
    DB_RETR_NO_ACTIVE_ROAMING = 102,
    DB_RETR_NO_ACTIVE_FUN_PROFILE = 103,
    DB_RETR_INVALID_ACTIVATION_DATE_FORMAT = 104,
    DB_RETR_INVALID_PRE_ACTIVATION_DATE = 105,
    DB_RETR_DUPLICATE_PRE_ACTIVATION = 106,
    DB_RETR_MAX_NUM_LINK_REACHED = 107,
    DB_RETR_INSUFFICIENT_BALANCE = 108,
    DB_RETR_ALREADY_ROAMER = 109,
    DB_RETR_ALREADY_OFW_SIM = 110,
    DB_RETR_INVALID_DURATION_EXT = 111,
    DB_RETR_ALREADY_LINKED = 112,
    DB_RETR_NOT_IN_HLR_RANGE = 113,
    DB_RETR_LOCAL_SIM_ALREADY_LINKED = 114,
    DB_RETR_NOT_IN_CUSTTYPE_RANGE = 115,
    DB_RETR_LOCAL_SIM_SAME_OFW_SIM = 116,
    DB_RETR_FOR_PREPAID_ONLY = 117,
    DB_RETR_LINK_TO_POSTPAID_ERROR = 118,
    DB_RETR_BREATHING_PERIOD = 119,
    //-- new set of retr... :(
    DB_RETR_ROAMING_W_OPEN_DURATION = 140,
    DB_RETR_NO_ACTIVE_LINK = 141,
    DB_RETR_LINKED_TO_OTHER_SIM = 142,
    DB_RETR_OFW_SIM_ALREADY_LINKED_AS_LOCAL = 143,
    DB_RETR_OFW_SIM_IS_LOCAL_SIM = 144,
    DB_RETR_USURF_W_ACTIVE_ROAM = 145,
    DB_RETR_USURF_INVALID_COUNTRY = 146,
    DB_RETR_USURF_INVALID_DURATION = 147,
    DB_RETR_USURF_WO_ACTIVE_ROAM = 148,
    DB_RETR_USURF_W_PENDING_ROAM = 149,
    DB_RETR_USURF_OK = 150,
    DB_RETR_USURF_PRE_ACT = 151,
    DB_RETR_USURF_INVALID_KEYWORD = 152,
    DB_RETR_USURF_INVALID_COEX = 153,

    DB_RETR_DAILY_BALANCE_INSUFF_BAL = 120,
    DB_RETR_DAILY_BALANCE_INSUFF_BAL_DEACT = 121,

    DB_RETR_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY = 130,
    DB_RETR_GEO_PROBE_CHANGE_TO_HOT_COUNTRY = 131,
    DB_RETR_GEO_PROBE_EXCEED_STAY_LIMIT_ON_HOT_COUNTRY = 132,
    DB_RETR_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY_W_DURATION = 133,

    DB_RETR_INIT = 999
};

typedef struct rc_t {
    int  step_no;
    char description[255];
    char svc_url[255];
    char svc_name[64];
    char svc_host[64];
    int  svc_port;
    char svc_user[32];
    char svc_pass[32];
    char db_host[32];
    char db_user[32];
    char db_pass[32];
    char libpath[64];
    char expiry[32];
    int  timeout;
    int  thread_count;
    int  max_retry;
} rc_t;

typedef struct rc_proc_t {
    rc_t* rc;
    int proc_id;
    int parent_id;
    pid_t ppid;
} rc_proc_t;

typedef struct request_t {

    char CSP_Txid[128];
    char SMS_Message_String[MAX_MESSAGE_LEN];
    char SMS_SourceAddr[64];
    char SUB_Mobtel[64];
    char SUB_Device_Details[256];

#define txid   CSP_Txid
#define msg    SMS_Message_String
#define msisdn SUB_Mobtel
#define a_no   SUB_Mobtel
#define b_no   SMS_SourceAddr

    int id;
    int ref_id;
    int step_no;
    int last_step_no;
    int status;
    int cluster_node;
    int tran_type;
    char customer_type[32];
    char request_origin[32];
    char gsm_num[16];
    char msg_id[64];
    int db_retr;
    int min_bal;
    char activation_date[32];
    char deactivation_date[32];
    int duration;
    char country[32];
    char rate[32];
    char imsi[32];
    int zone;
    int error_code;
    int result_code;
    int silent;
    int local_sim;
    int nsn_flag;
    char partner[32];
    char exptime[32];
    char expdate[32];
    char service_id[32];
    char promo_name[32];
    char promo_code[32];
    char promo_value[32];
    char others[256];

#define svc_txid         CSP_Txid
#define svc_txcode       msg_id
#define svc_msisdn       gsm_num
#define svc_soc          service_id
#define svc_type         customer_type
#define svc_bill_cycle   exptime
#define svc_eff_date     expdate

#define svc_plan         service_id
#define svc_start        exptime
#define svc_end          expdate

    request_t () {
    }
    request_t (const request_t& right) {
        *this = right;
    }
    request_t& operator= (const request_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(request_t));
        return *this;
    }

} request_t;

typedef request_t response_t;

typedef struct daily_bal_t {
    int id;
    int cluster_node;
    char msg[MAX_MESSAGE_LEN];
    char msisdn[16];
    char deactivation_date[32];
    int daily_ctr;
    int daily_balance;
    int current_balance;
    int status;
    int db_retr;

    daily_bal_t () {
    }
    daily_bal_t (const daily_bal_t& right) {
        *this = right;
    }
    daily_bal_t& operator= (const daily_bal_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(daily_bal_t));
        return *this;
    }
} daily_bal_t;

typedef struct geo_probe_t {
    int id;
    int cluster_node;
    char msisdn[16];
    char country[32];
    char deactivation_date[32];
    char imsi[32];
    char sccp[32];
    int status;
    int db_retr;

    geo_probe_t () {
    }
    geo_probe_t (const geo_probe_t& right) {
        *this = right;
    }
    geo_probe_t& operator= (const geo_probe_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(geo_probe_t));
        return *this;
    }
} geo_probe_t;

typedef struct broadcast_t {
    int id;
    int cluster_node;
    char msisdn[16];
    char msg_id[64];
    char msg[MAX_MESSAGE_LEN];
    int status;
    int db_retr;

    broadcast_t () {
    }
    broadcast_t (const broadcast_t& right) {
        *this = right;
    }
    broadcast_t& operator= (const broadcast_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(broadcast_t));
        return *this;
    }
} broadcast_t;

typedef struct ods_t {
    int id;
    int cluster_node;
    char msisdn[16];
    int status;
    int db_retr;

    ods_t () {
    }
    ods_t (const ods_t& right) {
        *this = right;
    }
    ods_t& operator= (const ods_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(ods_t));
        return *this;
    }
} ods_t;

typedef struct ccb_t {
    int id;
    int cluster_node;
    char msisdn[16];
    char ccb_action[8];
    char ccb_status[8];
    int status;
    int db_retr;

    ccb_t () {
    }
    ccb_t (const ccb_t& right) {
        *this = right;
    }
    ccb_t& operator= (const ccb_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(ccb_t));
        return *this;
    }
} ccb_t;

typedef struct notification_t {
    int id;
    int ref_id;
    int cluster_node;
    char msisdn[16];
    int tran_type;
    int status;
    int db_retr;

    notification_t () {
    }
    notification_t (const notification_t& right) {
        *this = right;
    }
    notification_t& operator= (const notification_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(notification_t));
        return *this;
    }
} notification_t;

typedef struct default_unli_t {
    int id;
    int cluster_node;
    char msisdn[16];
    char mnc[32];
    char mcc[32];
    char sgsn_ip[32];
    char date[32];
    char start_date[32];
    char end_date[32];
    char filename[128];
    int db_retr;

    default_unli_t () {
    }
    default_unli_t (const default_unli_t& right) {
        *this = right;
    }
    default_unli_t& operator= (const default_unli_t& right) {
        memcpy((void*)this, (void*)&right, sizeof(default_unli_t));
        return *this;
    }
} default_unli_t;

/*-functions------------------------------------------------------------------*/

#endif

/******************************************************************************/
