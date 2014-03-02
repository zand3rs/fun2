/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httputils.cpp
 * Description : HTTP helper functions.
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

#include <string>

using namespace std;

/*============================================================================*/

void url_encode(const char* src, char* dest, const int dest_size)
{
    string s = src;
    string buf = "";
    int len = s.length();

    for (int i = 0; i < len; i++) {
        char ch = s.at(i);
        if (! isalnum(ch)) {
            char tmpstr[8];
            sprintf(tmpstr, "%%%02X", ch);
            buf += tmpstr;
        } else {
            buf += ch;
        }
    }
    snprintf(dest, dest_size, "%s", buf.c_str());
}

/*----------------------------------------------------------------------------*/

void url_decode(const char* src, char* dest, const int dest_size)
{
    string s = src;   
    string buf = "";
    int len = s.length();

    for (int i = 0; i < len; i++) {
        char ch = s.at(i);
        if (ch == '%') {
            char tmpstr[] = "0x0__";
            int chnum;
            tmpstr[3] = s.at(i+1);
            tmpstr[4] = s.at(i+2);
            chnum = strtol(tmpstr, NULL, 16);   
            buf += chnum;
            i += 2;
        } else if (ch == '+') {
            buf += ' ';
        } else {
            buf += ch;
        }
    }
    snprintf(dest, dest_size, "%s", buf.c_str());
}

/******************************************************************************/
