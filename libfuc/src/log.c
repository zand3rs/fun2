/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2005-04-19
 *     /         Updated : 2009-09-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : log.c
 * Description : Functions for manipulating log files.
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
#include <time.h>

#include "log.h"

/*============================================================================*/

static char loglevel_str[LOG_LEVEL_MAX][16] = {
    "",
    "CRITICAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};

static Log_t default_log;
static Log_t* default_plog = NULL;
static int last_status = 0;

/*============================================================================*/

int log_init (Log_t *log, const char *fpath, const int loglevel)
{
    if (! log) {
        last_status = -1;
        return -1;
    }

    memset(log, 0, sizeof(Log_t));
    snprintf(log->fpath, sizeof(log->fpath), "%s", (fpath) ? fpath : "");
    snprintf(log->fmode, sizeof(log->fmode), "%s", "a+");
    pthread_mutex_init(&(log->lock), NULL);

    log->loglevel = loglevel;
    log->initialized = 1;

    last_status = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/

int log_deinit (Log_t *log)
{
    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }

    /* try closing the log first... */
    log_close(log);

    pthread_mutex_destroy(&(log->lock));
    memset(log, 0, sizeof(Log_t));

    last_status = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/

int log_open (Log_t *log)
{
    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (log->fp) {
        last_status = -1;
        return -1;
    }
#ifdef __DEBUG__
    fprintf(stdout, "%s: fpath=[%s]\n", __func__, log->fpath);
#endif

    pthread_mutex_lock(&(log->lock));
    snprintf(log->fmode, sizeof(log->fmode), "%s", "a+");
    log->fp = fopen(log->fpath, log->fmode);
    pthread_mutex_unlock(&(log->lock));

#ifdef __DEBUG__
    fprintf(stdout, "%s: status=[%s]\n", __func__, (log->fp) ? "OK" : "ERROR");
#endif

    last_status = (! log->fp) ? -1 : 0;
    return (! log->fp) ? -1 : 0;
}

/*----------------------------------------------------------------------------*/

int log_close (Log_t *log)
{
    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (! log->fp) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));
    fclose(log->fp);
    log->fp = NULL;
    pthread_mutex_unlock(&(log->lock));

    last_status = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/

int log_refresh (Log_t *log)
{
    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));
    if (log->fp) {
        fclose(log->fp);
    }
    log->fp = fopen(log->fpath, log->fmode);
    pthread_mutex_unlock(&(log->lock));

    last_status = (! log->fp) ? -1 : 0;
    return (! log->fp) ? -1 : 0;
}

/*----------------------------------------------------------------------------*/

int log_rotate (Log_t *log)
{
    time_t epoch;
    struct tm tm;
    char *fpath;
    int fpath_sz = 512;

    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }

    fpath = (char*) malloc(fpath_sz);
    if (! fpath) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));

    if (log->fp) {
        fclose(log->fp);
    }
    epoch = time(NULL);
    localtime_r(&epoch, &tm);
    snprintf(fpath, fpath_sz, "%s.%04d%02d%02d-%02d%02d%02d",
            log->fpath, tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    rename(log->fpath, fpath);
    log->fp = fopen(log->fpath, log->fmode);

    pthread_mutex_unlock(&(log->lock));
    free(fpath);

    last_status = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/

int log_write (Log_t *log, char *buf)
{
    int status;

    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (! log->fp) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));
    status = fputs(buf, log->fp);
    if (EOF != status) {
        status = fflush(log->fp);
    }
    pthread_mutex_unlock(&(log->lock));

    last_status = (EOF != status) ? 0 : -1;
    return (EOF != status) ? 0 : -1;
}

/*----------------------------------------------------------------------------*/

int log_format (Log_t *log, const char *fmt, ...)
{
    va_list ap;
    int status;

    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (! log->fp) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));

    va_start(ap, fmt);
    status = vfprintf(log->fp, fmt, ap);
    va_end(ap);

    if (EOF != status) {
        status = fflush(log->fp);
    }

    pthread_mutex_unlock(&(log->lock));

    last_status = (EOF != status) ? 0 : -1;
    return (EOF != status) ? 0 : -1;
}

/*----------------------------------------------------------------------------*/

int log_mesg (Log_t *log, const char *fmt, ...)
{
    va_list ap;
    time_t epoch;
    struct tm tm;
    char *logfmt;
    int logfmt_sz;

    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (! log->fp) {
        last_status = -1;
        return -1;
    }

    logfmt_sz = strlen(fmt) + 32;
    logfmt = (char*) malloc(logfmt_sz);
    if (! logfmt) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));

    epoch = time(NULL);
    localtime_r(&epoch, &tm);
    sprintf(logfmt, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec, fmt);

    va_start(ap, fmt);
    vfprintf(log->fp, logfmt, ap);
    va_end(ap);

    fflush(log->fp);

    pthread_mutex_unlock(&(log->lock));
    free(logfmt);

    last_status = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/

int log_event (Log_t *log, const int loglevel, const char *fmt, ...)
{
    va_list ap;
    time_t epoch;
    struct tm tm;
    char *logfmt;
    int logfmt_sz;

    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (! log->fp) {
        last_status = -1;
        return -1;
    }
    if (log->loglevel <= LOG_LEVEL_NONE) {
        last_status = -1;
        return 0;
    }
    if (log->loglevel < loglevel) {
        last_status = -1;
        return 0;
    }

    logfmt_sz = strlen(fmt) + 64;
    logfmt = (char*) malloc(logfmt_sz);
    if (! logfmt) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));

    epoch = time(NULL);
    localtime_r(&epoch, &tm);
    strftime(logfmt, logfmt_sz, "[%Y-%m-%d %H:%M:%S] ", &tm);
    if (loglevel > LOG_LEVEL_NONE &&  loglevel < LOG_LEVEL_MAX) {
        snprintf(&logfmt[strlen(logfmt)], logfmt_sz-strlen(logfmt), "%s: ",
                loglevel_str[loglevel]);
    }
    snprintf(&logfmt[strlen(logfmt)], logfmt_sz-strlen(logfmt), "%s\n", fmt);

    va_start(ap, fmt);
    vfprintf(log->fp, logfmt, ap);
    va_end(ap);

    fflush(log->fp);

    pthread_mutex_unlock(&(log->lock));
    free(logfmt);

    last_status = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/

int log_is_open (Log_t *log)
{
    if (! log) {
        return 0;
    }
    if (! log->initialized) {
        return 0;
    }
    if (! log->fp) {
        return 0;
    }
    return 1;
}

/*----------------------------------------------------------------------------*/
/*-Read Only Functions--------------------------------------------------------*/

int log_ropen (Log_t *log)
{
    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (log->fp) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));
    snprintf(log->fmode, sizeof(log->fmode), "%s", "r");
    log->fp = fopen(log->fpath, log->fmode);
    pthread_mutex_unlock(&(log->lock));

    last_status = (! log->fp) ? -1 : 0;
    return (! log->fp) ? -1 : 0;
}

/*----------------------------------------------------------------------------*/

int log_rclose (Log_t *log)
{
    last_status = 0;
    return log_close(log);
}

/*----------------------------------------------------------------------------*/

int log_read (Log_t *log, char *buf, int buf_size)
{
    int ret;

    if (! log) {
        last_status = -1;
        return -1;
    }
    if (! log->initialized) {
        last_status = -1;
        return -1;
    }
    if (! log->fp) {
        last_status = -1;
        return -1;
    }

    pthread_mutex_lock(&(log->lock));
    if (! feof(log->fp) && fgets(buf, buf_size, log->fp) != NULL) {
        ret = 0;
    }
    else {
        ret = -1;
    }
    pthread_mutex_unlock(&(log->lock));

    last_status = ret;
    return ret;
}

/*============================================================================*/

Log_t* log_get_default (void)
{
    return (default_plog) ? default_plog : &default_log;
}

/*----------------------------------------------------------------------------*/

void log_set_default (Log_t *log)
{
    default_plog = log;
}

/*----------------------------------------------------------------------------*/

Log_t* log_new (void)
{
    Log_t *log = (Log_t *) malloc(sizeof(Log_t));

    if (log) {
        memset(log, 0, sizeof(Log_t));
    }

    return log;
}

/*----------------------------------------------------------------------------*/

void log_free (Log_t *log)
{
    if (log) {
        memset(log, 0, sizeof(Log_t));
        free(log);
    }
}

/*----------------------------------------------------------------------------*/

int log_get_status (void)
{
    return last_status;
}

/******************************************************************************/

#ifdef __MAIN__

int main ()
{
    LOG_INIT("test.log", LOG_LEVEL_DEBUG);
    LOG_DEBUG("test debug");
    LOG_INFO("test info %d", 1);
    LOG_WARNING("test warning %d", 2);
    LOG_ERROR("test error %s", "3");
    LOG_CRITICAL("test critical %s", "four");
    LOG_ROTATE();
    LOG_DEINIT();

    sleep(1);

    LOG_INIT("test.log", LOG_LEVEL_ERROR);
    LOG_DEBUG("test debug");
    LOG_INFO("test info %d", 1);

    LOG_SET_HANDLE(LOG_GET_HANDLE());

    LOG_WARNING("test warning %d", 2);
    LOG_ERROR("test error %s", "3");
    LOG_CRITICAL("test critical %s", "four");
    LOG_DEINIT();

    return 0;
}

#endif

