/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2004-04-21
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : cfg.h
 * Description : Functions for loading/editing config files.
 *
 * $Id$
 */

/******************************************************************************/
/*
 * Recognized configuration settings...
 *
 * #comment1
 *    # comment2
 * key1=val1 #Recommended key/val pair convention
 * key2 = val2
 *    key3=
 * key4#comment3
 *
 * [section1] #comment4
 * #comment5
 * key1=sec1key1#comment6
 * key2=sec1\#key2 #comment7
 *
 *
 * Sample function calls for the above config settings...
 *
 * status = cfg_getval(&config, "", "key1", val, sizeof(val));
 *          Result: val = val1
 * status = cfg_getval(&config, "section1", "key2", val, sizeof(val));
 *          Result: val = sec1#key2
 *
 */
/******************************************************************************/

#ifndef __CFG_H__
#define __CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

#define CFG_MAX_BUF_LEN  1024

typedef struct Cfgbuf {
    struct Cfgbuf *next;
    char line[CFG_MAX_BUF_LEN];
} Cfgbuf_t;

typedef struct Cfg {
    Cfgbuf_t *buf;
    char fpath[256];
    char loaded;
} Cfg_t;

/*-functions------------------------------------------------------------------*/
/*
 * Return value:
 *      int => 0 if successful, -1 otherwise
 *
 * For cfg_getval(Cfg_t *config, const char *sec, const char *key, char *val, const int val_size)...
 *      If the return value is 0 and val is not NULL, val will always be set to
 *      empty string whenever a search key or sec/key search pair is not found 
 *      in the config buffer.
 */ 

/*
 * cfg_init - Initialize config buffer. 
 */ 
int  cfg_init   (Cfg_t *config, const char *fpath);

/*
 * cfg_load - Load configuration into config buffer.
 */ 
int  cfg_load   (Cfg_t *config);

/*
 * cfg_unload - Unload config buffer and free resources used.
 */ 
int  cfg_unload (Cfg_t *config);

/*
 * cfg_save - Write config back into the file currently loaded in the buffer.
 */ 
int  cfg_save   (Cfg_t *config);

/*
 * cfg_setval - Set config value to val in section sec with searchkey equals key.
 */ 
int  cfg_setval(Cfg_t *config, const char *sec, const char *key, const char *val);

/*
 * cfg_getval - Copy config value into val in section sec with searchkey equals key.
 */ 
int  cfg_getval(Cfg_t *config, const char *sec, const char *key, char *val, const int val_size);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

