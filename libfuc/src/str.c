/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2003-05-02
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : str.c
 * Description : Functions for manipulating strings.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "str.h"

/*============================================================================*/

int str_chomp (char *s)
{
    int len;

    if (! s || ! *s) {
        return -1;
    }

    len = strlen (s);
    while (len > 0) {
        --len;
        if (s[len] == '\n' || s[len] == '\r') {
            s[len] = 0;
        }
        else {
            break;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_ltrim (char *s)
{
    char *ps;
    int len, ctr;

    if (! s || ! *s) {
        return -1;
    }

    len = strlen (s) + 1;
    ps = s;
    ctr = 0;

    while (*ps) {
        if (isspace ((int) *ps)) {
            ++ctr;
        }
        else {
            break;
        }
        ++ps;
    }

    if (ctr > 0) {
        memmove (s, ps, (len - ctr));
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_rtrim (char *s)
{
    int len;

    if (! s || ! *s) {
        return -1;
    }

    len = strlen (s);
    while (len > 0) {
        --len;
        if (isspace ((int) s[len])) {
            s[len] = 0;
        }
        else {
            break;
        }
    }

    return 0;
} 

/*----------------------------------------------------------------------------*/

int str_trim (char *s)
{
    if (! s || ! *s) {
        return -1;
    }

    str_ltrim (s);
    str_rtrim (s);

    return 0;
} 

/*----------------------------------------------------------------------------*/

int str_ltrim_char (char *s, char c)
{
    char *ps;
    int len, ctr;

    if (! s || ! *s) {
        return -1;
    }

    len = strlen (s) + 1;
    ps = s;
    ctr = 0;

    while (*ps) {
        if (c == *ps) {
            ++ctr;
        }
        else {
            break;
        }
        ++ps;
    }

    if (ctr > 0) {
        memmove (s, ps, (len - ctr));
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_rtrim_char (char *s, char c)
{
    int len;

    if (! s || ! *s) {
        return -1;
    }

    len = strlen (s);
    while (len > 0) {
        --len;
        if (c == s[len]) {
            s[len] = 0;
        }
        else {
            break;
        }
    }

    return 0;
} 

/*----------------------------------------------------------------------------*/

int str_trim_char (char *s, char c)
{
    if (! s || ! *s) {
        return -1;
    }

    str_ltrim_char (s, c);
    str_rtrim_char (s, c);

    return 0;
} 

/*----------------------------------------------------------------------------*/

int str_toupper (char *s)
{
    char *ps;

    if (! s || ! *s) {
        return -1;
    }

    ps = s;
    while (*ps) {
        *ps = (char) toupper ((int) *ps);
        ++ps;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_tolower (char *s)
{
    char *ps;

    if (! s || ! *s) {
        return -1;
    }

    ps = s;
    while (*ps) {
        *ps = (char) tolower ((int) *ps);
        ++ps;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_ucwords (char *s)
{
    char *ps;
    char last = ' ';

    if (! s || ! *s) {
        return -1;
    }

    ps = s;
    while (*ps) {
        if (isspace((int) last) || '-' == last) {
            *ps = (char) toupper ((int) *ps);
        } else {
            *ps = (char) tolower ((int) *ps);
        }
        last = *ps;
        ++ps;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_reverse (char *s)
{
    char *ps;
    int len, i;

    if (! s || ! *s) {
        return -1;
    }

    ps = strdup (s);
    if (ps == NULL) {
        return -1;
    }

    i = 0;
    len = strlen (s);
    while (len > 0) {
        --len;
        s[i] = ps[len];
        ++i;
    }
    free (ps);

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_rand (char *s, int len)
{
    char alphanum[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\0";
    static unsigned int seq = 0;
    unsigned int idx, i;
    time_t lt;

    if (! s) {
        return -1;
    }

    lt = time (NULL);
    srand (lt);
    for (i = 0; i < len; ++i) {
        idx = (int) (61.0 * rand () / RAND_MAX);
        idx += seq;
        idx %= 62;
        s[i] = alphanum[idx];
    }
    s[i] = 0;
    ++seq;
    seq %= 62;

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_tok (char *dest, char *src, const char *needle)
{
    char *pdest, *psrc;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    psrc = src;
    strcpy (dest, src);
    pdest = strstr (dest, needle);
    if (pdest != NULL) {
        *pdest = 0;
        psrc += (pdest - dest) + strlen (needle);
        memmove (src, psrc, (strlen (psrc) + 1));
    }
    else {
        *psrc = 0;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_tok2 (char *dest, char **src, int dest_size, const char *needle)
{
    char *psrc, *p;
    int len;

    if (! dest || ! src || ! **src || dest_size < 1) {
        return -1;
    }

    memset(dest, 0, dest_size);
    psrc = *src;
    p = strstr(psrc, needle);

    if (p) {
        len = p - psrc;
        if (len >= dest_size) {
            len = dest_size - 1;
        }
        memcpy(dest, psrc, len);
        *src = p + strlen(needle);
    } else {
        len = dest_size - 1;
        memcpy(dest, psrc, len);
        *src = psrc + strlen(psrc);
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_slice (char *dest, char *src, int len)
{
    char *psrc;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    memset (dest, 0, (len + 1));
    strncpy (dest, src, len);
    psrc = src + strlen (dest);
    memmove (src, psrc, (strlen (psrc) + 1));

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_substr (char *dest, const char *src, int idx, int len)
{
    int srclen;
    char *psrc;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    srclen = strlen (src);
    if (idx >= srclen) {
        return -1;
    }

    psrc = (char *) &src[idx];
    memset (dest, 0, (len + 1));
    strncpy (dest, psrc, len);

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_pad (char *dest, const char *src, int len, const char *padstr, Strpad_t padtype)
{
    char *pdest;
    int padlen;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    if (strlen (src) >= len) {
        strcpy (dest, src);
    }
    else {
        pdest = dest;
        memset (dest, 0, (len + 1));
        padlen = len - strlen (src);

        if (STR_PAD_LEFT_MASKED == padtype ||
                STR_PAD_RIGHT_MASKED == padtype ||
                STR_PAD_BOTH_MASKED == padtype) {
            while (strlen (dest) < len) {
                strncpy (pdest, padstr, (len - strlen (dest)));
                pdest += strlen (pdest);
            }

            switch ((int) padtype) {
                case STR_PAD_LEFT_MASKED:
                    pdest = dest + padlen; 
                    strncpy (pdest, src, (len - padlen));
                    break;
                case STR_PAD_RIGHT_MASKED:
                    pdest = dest;
                    strncpy (pdest, src, (len - padlen));
                    break;
                case STR_PAD_BOTH_MASKED:
                    pdest = dest + ((int) (padlen / 2)); 
                    strncpy (pdest, src, (len - padlen));
                    break;
                default:
                    return -1;
            }
        }
        else {
            switch ((int) padtype) {
                case STR_PAD_LEFT:
                    while (strlen (dest) < padlen) {
                        strncpy (pdest, padstr, (padlen - strlen (dest)));
                        pdest += strlen (pdest);
                    }
                    strncpy (pdest, src, (len - padlen));
                    break;
                case STR_PAD_RIGHT:
                    strncpy (pdest, src, (len - padlen));
                    pdest += strlen (pdest);
                    while (strlen (dest) < (strlen (src) + padlen)) {
                        strncpy (pdest, padstr, ((strlen (src) + padlen) - strlen (dest)));
                        pdest += strlen (pdest);
                    }
                    break;
                case STR_PAD_BOTH:
                    while (strlen (dest) < ((int) (padlen / 2))) {
                        strncpy (pdest, padstr, (((int) (padlen / 2)) - strlen (dest)));
                        pdest += strlen (pdest);
                    }
                    strncpy (pdest, src, (len - padlen));
                    pdest += strlen (pdest);
                    while (strlen (dest) < (strlen (src) + padlen)) {
                        strncpy (pdest, padstr, ((strlen (src) + padlen) - strlen (dest)));
                        pdest += strlen (pdest);
                    }
                    break;
                default:
                    return -1;
            }
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_escape (char *dest, const char *src, int dest_size) 
{
    int i, j;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    i = 0;
    j = 0;
    memset (dest, 0, dest_size);

    while (src[i] && j < (dest_size - 1)) {
        switch (src[i]) {
            case '\n':
                if ((j + 1) < (dest_size - 1)) {
                    dest[j] = '\\'; ++j;
                    dest[j] = 'n';
                }
                break;
            case '\r':
                if ((j + 1) < (dest_size - 1)) {
                    dest[j] = '\\'; ++j;
                    dest[j] = 'r';
                }
                break;
            case '\t':
                if ((j + 1) < (dest_size - 1)) {
                    dest[j] = '\\'; ++j;
                    dest[j] = 't';
                }
                break;
            case '\f':
                if ((j + 1) < (dest_size - 1)) {
                    dest[j] = '\\'; ++j;
                    dest[j] = 'f';
                }
                break;
            case '\\':
                if ((j + 1) < (dest_size - 1)) {
                    dest[j] = '\\'; ++j;
                    dest[j] = '\\';
                }
                break;
                case '\"':
                    if ((j + 1) < (dest_size - 1)) {
                        dest[j] = '\\'; ++j;
                        dest[j] = '\"';
                    }
                break;
            case '\'':
                if ((j + 1) < (dest_size - 1)) {
                    dest[j] = '\\'; ++j;
                    dest[j] = '\'';
                }
                break;
            default:
                dest[j] = src[i];
        }
        ++i;
        ++j;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_unescape (char *dest, const char *src, int dest_size) 
{
    int i, j;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    i = 0;
    j = 0;
    memset (dest, 0, dest_size);

    while (src[i] && j < (dest_size - 1)) {
        if (src[i] == '\\') {
            ++i;
            switch (src[i]) {
                case 'n':
                    dest[j] = '\n';
                    break;
                case 'r':
                    dest[j] = '\r';
                    break;
                case 't':
                    dest[j] = '\t';
                    break;
                case 'f':
                    dest[j] = '\f';
                    break;
                case '\\':
                    dest[j] = '\\';
                    break;
                    case '\"':
                        dest[j] = '\"';
                    break;
                case '\'':
                    dest[j] = '\'';
                    break;
                default:
                    dest[j] = '\\'; ++j;
                    dest[j] = src[i];
            }
        }
        else {
            dest[j] = src[i];
        }
        ++i;
        ++j;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int str_dos2unix (char *dest, const char *src, int dest_size)
{
    return str_replace (dest, src, dest_size, "\r\n", "\n");
}

/*----------------------------------------------------------------------------*/

int str_unix2dos (char *dest, const char *src, int dest_size)
{
    return str_replace (dest, src, dest_size, "\n", "\r\n");
}

/*----------------------------------------------------------------------------*/

int str_replace (char *dest, const char *src, int dest_size, const char *search, const char *replace)
{
    char *pdest, *psrc, *p;
    int len;

    if (! dest || ! src || ! *src) {
        return -1;
    }

    memset (dest, 0, dest_size);
    pdest = dest;
    psrc = (char *) src;

    while ((p = strstr (psrc, search)) != NULL) {
        len = p - psrc;
        if ((strlen (dest) + len) >= dest_size) {
            break;
        }
        strncpy (pdest, psrc, len);
        pdest += len;
        len = strlen (replace);
        if ((strlen (dest) + len) >= dest_size) {
            break;
        }
        strncpy (pdest, replace, len);
        pdest += len;
        psrc = p + strlen (search);
    }

    if (strlen (psrc) > 0) {
        len = dest_size - strlen (dest) - 1;
        strncpy (pdest, psrc, len);
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

char* str_chr (const char *s)
{
    char *p;

    p = (char*)s;
    while (isspace ((int) *p)) {
        ++p;
    }
    if (*p < 33 || *p > 126) {
        p = NULL;
    }

    return p;
}

/*----------------------------------------------------------------------------*/

char* str_rchr (const char *s)
{
    char *p;
    int ilen;

    ilen = strlen(s);
    while (ilen) {
        --ilen;
        if (! isspace ((int) s[ilen])) {
            break;
        }
    }
    p = (char*)s + ilen;
    if (*p < 33 || *p > 126) {
        p = NULL;
    }

    return p;
}

/*----------------------------------------------------------------------------*/

int str_is_printable (const char *s)
{
    int i, len;

    if(! s || ! *s) {
        return 0;
    }
    len = strlen((char *) s);

    for (i = 0; i < len; i++) {
        if (! isgraph((int) s[i])) {
            return 0;
        }
    }

    return 1;
}

/*----------------------------------------------------------------------------*/

int str_is_alpha (const char *s)
{
    int i, len;

    if(! s || ! *s) {
        return 0;
    }
    len = strlen((char *) s);

    for (i = 0; i < len; i++) {
        if (! isalpha((int) s[i])) {
            return 0;
        }
    }

    return 1;
}

/*----------------------------------------------------------------------------*/

int str_is_numeric (const char *s)
{
    int i, len;

    if(! s || ! *s) {
        return 0;
    }
    len = strlen((char *) s);

    for (i = 0; i < len; i++) {
        if (! isdigit((int) s[i])) {
            return 0;
        }
    }

    return 1;
}

/*----------------------------------------------------------------------------*/

int str_is_alnum (const char *s)
{
    int i, len;

    if(! s || ! *s) {
        return 0;
    }
    len = strlen((char *) s);

    for (i = 0; i < len; i++) {
        if (! isalnum((int) s[i])) {
            return 0;
        }
    }

    return 1;
}

/******************************************************************************/

