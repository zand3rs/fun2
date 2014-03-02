/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2005-04-19
 *     /         Updated : 2009-09-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : log.h
 * Description : Functions for manipulating log files.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

enum {
    LOG_LEVEL_NONE,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX
};

#ifndef LOG_GET_STATUS
#define LOG_GET_STATUS log_get_status
#endif

#ifndef LOG_GET_HANDLE
#define LOG_GET_HANDLE log_get_default
#endif

#ifndef LOG_SET_HANDLE
#define LOG_SET_HANDLE(log) { \
    log_set_default((Log_t*)log); \
}
#endif

#ifndef LOG_INIT
#define LOG_INIT(fpath, loglevel) { \
    log_init(log_get_default(), fpath, loglevel); \
    log_open(log_get_default()); \
}
#endif

#ifndef LOG_DEINIT
#define LOG_DEINIT() { \
    log_close(log_get_default()); \
    log_deinit(log_get_default()); \
}
#endif

#ifndef LOG_OPEN
#define LOG_OPEN() { \
    log_open(log_get_default()); \
}
#endif

#ifndef LOG_CLOSE
#define LOG_CLOSE() { \
    log_close(log_get_default()); \
}
#endif

#ifndef LOG_ROTATE
#define LOG_ROTATE() { \
    log_rotate(log_get_default()); \
}
#endif

#ifndef LOG_CRITICAL
#define LOG_CRITICAL(args...) { \
    log_event(log_get_default(), LOG_LEVEL_CRITICAL, args); \
}
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(args...) { \
    log_event(log_get_default(), LOG_LEVEL_ERROR, args); \
}
#endif

#ifndef LOG_WARNING
#define LOG_WARNING(args...) { \
    log_event(log_get_default(), LOG_LEVEL_WARNING, args); \
}
#endif

#ifndef LOG_INFO
#define LOG_INFO(args...) { \
    log_event(log_get_default(), LOG_LEVEL_INFO, args); \
}
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(args...) { \
    log_event(log_get_default(), LOG_LEVEL_DEBUG, args); \
}
#endif

typedef struct Log {
    pthread_mutex_t lock;
    char fpath[512];
    char fmode[8];
    FILE *fp;
    char loglevel;
    char initialized;
} Log_t;

/*-functions------------------------------------------------------------------*/

int log_init (Log_t *log, const char *fpath, const int loglevel);
int log_deinit (Log_t *log);
int log_open (Log_t *log);
int log_close (Log_t *log);
int log_refresh (Log_t *log);
int log_rotate (Log_t *log);
int log_write (Log_t *log, char *buf);
int log_format (Log_t *log, const char *fmt, ...);
int log_mesg (Log_t *log, const char *fmt, ...);
int log_event (Log_t *log, const int loglevel, const char *fmt, ...);
int log_is_open (Log_t *log);

/* read only */
int log_ropen (Log_t *log);
int log_rclose (Log_t *log);
int log_read (Log_t *log, char *buf, int buf_size);

/*
 * Helper functions...
 */
Log_t* log_get_default (void);
void log_set_default (Log_t *log);

Log_t* log_new (void);
void   log_free (Log_t *log);

int log_get_status (void);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

