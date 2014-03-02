/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : sysmsg.hpp
 * Description : System message.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _SYSMSG_HPP_
#define _SYSMSG_HPP_

#include <string>
#include <map>

using namespace std;

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

class Sysmsg {

    public:
        static const int MAX_MSG_PART = 3;

        static int load(const char* ora_auth);
        static string* getMessage(const char* key, const int part);

    private:
        typedef std::pair<string, int> key_pair_t;
        typedef std::map<key_pair_t, string> message_map_t;

        static message_map_t _msg_map;

};

#endif

/******************************************************************************/
