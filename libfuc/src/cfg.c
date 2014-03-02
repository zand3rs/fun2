/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2004-04-21
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : cfg.c
 * Description : Functions for loading/editing config files.
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

#include "str.h"
#include "cfg.h"

/*============================================================================*/

static short is_section_(const char *s)
{
    char *p;

    if (! s || strlen(s) < 3) {
        return 0;
    }
    p = str_chr(s);
    if (p != NULL && *p == '[' && *(p+1) != ']') {
        p = strchr (s, ']');
        if (p != NULL) {
            return 1;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

static short is_comment_(const char *s)
{
    char *p;

    if (! s || strlen(s) < 1) {
        return 0;
    }
    p = str_chr(s);
    if (p != NULL && *p == '#') {
        return 1;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

static short is_blank_(const char *s)
{
    char *p;

    if (! s || ! *s) {
        return 1;
    }
    p = str_chr(s);
    if (p == NULL) {
        return 1;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

static short is_secmatch_(const char *buf, const char *sec) {
    char *p;
    short len;

    if (! buf || ! *buf || ! sec || ! *sec) {
        return 0;
    }
    if ((p = strchr(buf, '[')) != NULL) {
        ++p;
        len = strlen(sec);
        if (strncmp(p, sec, len) == 0 && *(p + len) == ']') {
            /* found a section match */
            return 1;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

static short is_keymatch_(const char *buf, const char *key) {
    char *p;
    short len;

    if (! buf || ! *buf || ! key || ! *key) {
        return 0;
    }
    len = strlen(key);
    p = str_chr(buf);
    if (p != NULL && strncmp(p, key, len) == 0 
            && (*(p + len) == '=' || *(p + len) == '#' || isspace((int)*(p + len)))) {
        return 1;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

static void comment_escape_(char *dest, const char *src, int dest_size)
{
    int i, j;

    if (! dest || ! src || ! *src) {
        return;
    }
    i = 0; j = 0;
    memset (dest, 0, dest_size);
    while (src[i] && j < (dest_size - 1)) {
        if (src[i] == '#') {
            if ((j + 1) < (dest_size - 1)) {
                dest[j] = '\\'; ++j;
                dest[j] = '#';
            }
        }
        else {
            dest[j] = src[i];
        }
        ++i; ++j;
    }
}

/*----------------------------------------------------------------------------*/

static void comment_unescape_(char *dest, const char *src, int dest_size) 
{
    int i, j;

    if (! dest || ! src || ! *src) {
        return;
    }
    i = 0; j = 0;
    while (src[i] && j < (dest_size - 1)) {
        if (src[i] == '\\' && src[i+1] == '#') {
            ++i;
            dest[j] = src[i];
        }
        else {
            dest[j] = src[i];
        }
        ++i; ++j;
    }
    dest[j] = 0;
}

/*----------------------------------------------------------------------------*/

static char* get_comment_(const char *src)
{
    char *p, *ps;

    if (! src || ! *src) {
        return NULL;
    }
    ps = (char*)src;
    while ((p = strchr(ps, '#')) != NULL) {
        if (p > ps && *(p - 1) == '\\') {
            /* comment symbol is escaped, continue searching... */
            ps = p + 1;
        }
        else {
            /* comment found */
            break;
        }
    }
    return p;
}

/*----------------------------------------------------------------------------*/

static int get_property_value_(char *dest, const char *src, const int dest_size)
{
    char *p, *pstart, *plast;

    if (! dest || ! src || ! *src) {
        return -1;
    }
    memset (dest, 0, dest_size);

    p = strchr(src, '=');
    if (p != NULL) {
        pstart = p + 1;
        plast = pstart;
        while ((p = strchr(plast, '#')) != NULL) {
            if (p > plast && *(p - 1) == '\\') {
                /* comment symbol is escaped, continue searching... */
                plast = p + 1;
            }
            else {
                /* comment found */
                break;
            }
        }
        comment_unescape_(dest, pstart, dest_size);
    }
    str_trim(dest);

    return 0;
}

/*----------------------------------------------------------------------------*/

static void unloadbuf_(Cfgbuf_t *buf)
{
    Cfgbuf_t *pbuf, *phead;

    phead = buf;
    while (phead != NULL) {
        pbuf = phead;
        phead = phead->next;
        pbuf->next = NULL;
        memset(pbuf->line, 0, sizeof(pbuf->line));
        free (pbuf);
    }
}

/*----------------------------------------------------------------------------*/

static Cfgbuf_t* loadbuf_(const char *fpath)
{
    FILE *fp;
    Cfgbuf_t *pbuf, *phead, *plast;
    short err = 0;

    pbuf = NULL;
    phead = NULL;
    plast = NULL;

    fp = fopen(fpath, "r");
    if (! fp) {
        return NULL;
    }
    while (! feof(fp)) {
        pbuf = (Cfgbuf_t *)malloc(sizeof(Cfgbuf_t));
        if (pbuf == NULL) {
            err = 1;
            break;    
        }
        pbuf->next = NULL;
        memset(pbuf->line, 0, sizeof(pbuf->line));
        if (fgets(pbuf->line, sizeof(pbuf->line), fp) != NULL) {
            str_chomp(pbuf->line);
            if (phead == NULL) {
                phead = pbuf;
                plast = pbuf;
            }
            else {
                plast->next = pbuf;
                plast = pbuf;
            }
        }
        else {
            free(pbuf);
        }
    }
    fclose(fp);
    if (err) {
        unloadbuf_(phead);
        return NULL;
    }

    return phead;
}

/*----------------------------------------------------------------------------*/

static Cfgbuf_t* searchbuf_(const Cfgbuf_t *buf, const char *sec, const char *key)
{
    Cfgbuf_t *phead, *plast, *pbuf;
    short secfound = 0;

    if (! buf || ! key || ! *key) {
        return NULL;
    }
    if (! sec || ! *sec) {
        /* no section to find */
        secfound = 1;
    }
    plast = NULL;
    phead = (Cfgbuf_t*)buf;
    while (phead != NULL) {
        pbuf = phead;
        phead = phead->next;
        if (is_blank_(pbuf->line) || is_comment_(pbuf->line)) {
            continue;
        }
        if (secfound) {
            if (is_section_(pbuf->line)) {
                /* found the next section, stop searching */
                break;
            }
            else {
                if (is_keymatch_(pbuf->line, key)) {
                    /* found what we are looking for */
                    plast = pbuf;
                    break;
                }
            }
        }
        else {
            if (is_section_(pbuf->line)) {
                if (is_secmatch_(pbuf->line, sec)) {
                    /* found a section match */
                    secfound = 1;
                }
            }
        }
    }

    return plast;
}

/*============================================================================*/

int cfg_init(Cfg_t *config, const char *fpath)
{
    if (! config) {
        return -1;
    }
    memset(config, 0, sizeof(Cfg_t));
    snprintf(config->fpath, sizeof(config->fpath), "%s", (fpath) ? fpath : "");

    return 0;
}

/*----------------------------------------------------------------------------*/

int cfg_load(Cfg_t *config)
{
    Cfgbuf_t *pbuf;

    if (! config || config->loaded) {
        return -1;
    }
    pbuf = loadbuf_(config->fpath);
    if (pbuf == NULL) {
        return -1;
    }
    config->buf = pbuf;
    config->loaded = 1;

    return 0;
}

/*----------------------------------------------------------------------------*/

int cfg_unload(Cfg_t *config)
{
    if (! config || ! config->loaded) {
        return -1;
    }
    unloadbuf_(config->buf);
    config->buf = NULL;
    config->loaded = 0;

    return 0;
}

/*----------------------------------------------------------------------------*/

int cfg_save(Cfg_t *config)
{
    FILE *fp;
    Cfgbuf_t *pbuf, *phead;

    if (! config || ! config->loaded || config->buf == NULL) {
        return -1;
    }

    fp = fopen(config->fpath, "w+");
    if (! fp) {
        return -1;
    }
    phead = config->buf;
    while (phead != NULL) {
        pbuf = phead;
        phead = phead->next;
        fputs(pbuf->line, fp);
        fputs("\n", fp);
    }
    fclose(fp);

    return 0;
}

/*----------------------------------------------------------------------------*/

int cfg_setval(Cfg_t *config, const char *sec, const char *key, const char *val)
{
    Cfgbuf_t *pbuf;
    char *pcom, *comment = NULL;
    int linesize;

    if (! config || ! key || ! *key || ! val) {
        return -1;
    }
    if (! config->loaded) {
        return -1;
    }
    pbuf = searchbuf_(config->buf, sec, key);
    if (pbuf == NULL) {
        return -1;
    }
    pcom = get_comment_(pbuf->line);

    if (pcom != NULL) {
        comment = strdup(pcom);
    }
    linesize = sizeof(pbuf->line);
    memset(pbuf->line, 0, linesize);
    snprintf(pbuf->line, linesize, "%s=", key);
    comment_escape_(&(pbuf->line[strlen(pbuf->line)]), val, linesize - strlen(pbuf->line));
    if (comment != NULL) {
        snprintf(&(pbuf->line[strlen(pbuf->line)]), linesize - strlen(pbuf->line), " %s", comment);
        /* free our malloc'ed comment */
        free(comment);
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int cfg_getval(Cfg_t *config, const char *sec, const char *key, char *val, const int val_size)
{
    Cfgbuf_t *pbuf;

    if (val != NULL) {
        *val = 0;
    }
    if (! config || ! key || ! *key || ! val) {
        return -1;
    }
    if (! config->loaded) {
        return -1;
    }
    pbuf = searchbuf_(config->buf, sec, key);
    if (pbuf == NULL) {
        return -1;
    }

    return get_property_value_(val, pbuf->line, val_size);
}

/******************************************************************************/
