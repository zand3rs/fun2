/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : sysmsg.cpp
 * Description : System message.
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

#include "oradbsimple.hpp"
#include "sysmsg.hpp"

/*============================================================================*/
// Initializers...
Sysmsg::message_map_t Sysmsg::_msg_map;

/*============================================================================*/

int Sysmsg::load(const char* ora_auth)
{
    if (OraDBSimple::init_lib() < 0) {
        return -1;
    }

    OraDBSimple conn;
    if (conn.initialize(ora_auth) < 0) {
        return -1;
    }

    OraDBSimple::stmt_handle_t res = OraDBSimple::STH_INIT;
    const char** row;

    string q = "select keyword, sub_keyword, customer_type, message_type,"
        "message_id, msg_1, msg_2, msg_3 from keyword_msg where upper(status)='ACTIVE'";

    conn.query(&res, q.c_str());
    while((row = conn.fetchRow(res)) != NULL) {
        key_pair_t map_key;

        string keyword = row[0];
        string sub_keyword = row[1];
        string message_id = row[4];

        string msg[MAX_MSG_PART+1];
        msg[1] = row[5];
        msg[2] = row[6];
        msg[3] = row[7];

        //string keystr = keyword + "_" + sub_keyword + "_" + message_id;
        string keystr = message_id;

        for (int i=1; i<=MAX_MSG_PART; ++i) {
            map_key = key_pair_t(keystr, i);

            if(_msg_map.find(map_key) == _msg_map.end()) {
                _msg_map.insert(message_map_t::value_type(map_key, msg[i]));
            } else {
                _msg_map[map_key] = msg[i];
            }
        }
    }
    conn.freeResult(res);
    conn.destroy_db();

    return 0;
}

/*----------------------------------------------------------------------------*/

string* Sysmsg::getMessage(const char* key, const int part)
{
    key_pair_t map_key = key_pair_t(key, part);

    return &_msg_map[map_key];
}

/******************************************************************************/
