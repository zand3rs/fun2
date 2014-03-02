/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-10-12
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : file.c
 * Description : File handling functions...
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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>

#include "file.h"

/*============================================================================*/

void file_trim (char *path)
{
    int len;

    if (path) {
        len = strlen(path);
        while (len > 0) {
            --len;
            if (isspace((int) path[len]) || '/' == path[len]) {
                path[len] = 0;
            }
            else {
                break;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/

void file_basename (char *dest, char *src)
{
    char *p;

    p = strrchr(src, '/');
    if (p) {
        ++p;
        strcpy(dest, p);
    }
    else {
        strcpy(dest, "");
    }
}

/*----------------------------------------------------------------------------*/

void file_dirname (char *dest, char *src)
{
    char *p;

    strcpy(dest, ".");
    if (src && *src) {
        strcpy(dest, src);
        p = strrchr(dest, '/');
        if (p) {
            *p = 0;
        }
    }
}

/*----------------------------------------------------------------------------*/

int file_exists (char *path)
{
    struct stat buf;
    return (0 == stat(path, &buf)) ? 1 : 0;
}

/*----------------------------------------------------------------------------*/

int file_is_dir (char *path)
{
    struct stat buf;

    stat(path, &buf);
    return (int) S_ISDIR(buf.st_mode);
}

/*----------------------------------------------------------------------------*/

int file_mkdir (char *path)
{
    return mkdir(path, 0777);
}

/*----------------------------------------------------------------------------*/

int file_move (char *oldpath, char *newpath)
{
    return rename(oldpath, newpath);
}

/*----------------------------------------------------------------------------*/

int file_copy (char *srcpath, char *destpath)
{
    FILE *fsrc, *fdest;
    char c;

    fsrc = fopen(srcpath, "rb");
    if (! fsrc) {
#ifdef __DEBUG__
        fprintf (stderr, "%s: Error on fopen(src): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

    fdest = fopen(destpath, "wb");
    if (! fdest) {
#ifdef __DEBUG__
        fprintf (stderr, "%s: Error on fopen(dest): %d: %s\n", __func__, errno, strerror(errno));
#endif
        fclose(fsrc);
        return -1;
    }

    while (! feof(fsrc)) {
        c = fgetc(fsrc);
        if (ferror(fsrc)) {
#ifdef __DEBUG__
            fprintf (stderr, "%s: Error on fgetc(src): %d: %s\n", __func__, errno, strerror(errno));
#endif
            fclose(fsrc);
            fclose(fdest);
            return -1;
        }
        if (! feof(fsrc)) {
            fputc(c, fdest);
        }
        if (ferror(fdest)) {
#ifdef __DEBUG__
            fprintf (stderr, "%s: Error on fputc(dest): %d: %s\n", __func__, errno, strerror(errno));
#endif
            fclose(fsrc);
            fclose(fdest);
            return -1;
        }
    }

    fclose(fsrc);
    fclose(fdest);

    return 0;
}

/*----------------------------------------------------------------------------*/

int file_delete (char *path)
{
    return remove(path);
}

/*----------------------------------------------------------------------------*/

int file_list_get_first (const char *dir, const char *pattern, char *fname)
{
    struct dirent **namelist;
    int status, i;
    char _pattern[64];

    *fname = 0;

    if (! pattern || ! *pattern) {
        snprintf(_pattern, sizeof(_pattern), "*");
    }
    else {
        snprintf(_pattern, sizeof(_pattern), "%s", pattern);
    }

    status = scandir(dir, &namelist, 0, alphasort);
    if (0 > status) {
        return -1;
    }
    if (2 < status) {
        for (i = 0; i < status; ++i) {
            if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
                if (0 == fnmatch(_pattern, namelist[i]->d_name, 0)) {
                    strcpy(fname, namelist[i]->d_name);
                    break;
                }
            }
        }
    }

    for (i = 0; i < status; ++i) {
        free(namelist[i]);
    }
    free(namelist);

    return 0;
}

/*----------------------------------------------------------------------------*/

int file_list_get_last (const char *dir, const char *pattern, char *fname)
{
    struct dirent **namelist;
    int status, i;
    char _pattern[64];

    *fname = 0;

    if (! pattern || ! *pattern) {
        snprintf(_pattern, sizeof(_pattern), "*");
    }
    else {
        snprintf(_pattern, sizeof(_pattern), "%s", pattern);
    }

    status = scandir(dir, &namelist, 0, alphasort);
    if (0 > status) {
        return -1;
    }
    if (2 < status) {
        for (i = (status-1); i >= 0; --i) {
            if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
                if (0 == fnmatch(_pattern, namelist[i]->d_name, 0)) {
                    strcpy(fname, namelist[i]->d_name);
                    break;
                }
            }
        }
    }

    for (i = 0; i < status; ++i) {
        free(namelist[i]);
    }
    free(namelist);

    return 0;
}

#if 0
/*----------------------------------------------------------------------------*/

int file_list (const char *dir, File_name_t **flist)
{
    struct dirent **namelist;
    int status, i, idx = 0;
    File_name_t *array;

    *flist = NULL;

    status = scandir(dir, &namelist, 0, alphasort);
    if (0 > status) {
        return -1;
    }
    if (2 == status) {
        goto func_exit;
    }

    array = (File_name_t *) malloc(sizeof(File_name_t) * (status - 2));
    if (! array) {
        idx = -1;
        goto func_exit;
    }

    for (i = 0; i < status; ++i) {
        if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
            snprintf((char *) array[idx++], sizeof(File_name_t), "%s", namelist[i]->d_name);
        }
    }

    *flist = array;

func_exit:
    for (i = 0; i < status; ++i) {
        free(namelist[i]);
    }
    free(namelist);

    return idx;
}
#endif

/*----------------------------------------------------------------------------*/

int file_list (const char *dir, const char *pattern, File_name_t **flist)
{
    struct dirent **namelist;
    int status, i, idx = 0;
    char _pattern[64];
    File_name_t *buf = NULL;
    File_name_t *array = NULL;

    *flist = NULL;

    if (! pattern || ! *pattern) {
        snprintf(_pattern, sizeof(_pattern), "*");
    }
    else {
        snprintf(_pattern, sizeof(_pattern), "%s", pattern);
    }

    status = scandir(dir, &namelist, 0, alphasort);
    if (0 > status) {
        return -1;
    }
    if (2 < status) {
        for (i = 0; i < status; ++i) {
            if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
                if (0 == fnmatch(_pattern, namelist[i]->d_name, 0)) {
                    array = (File_name_t *) realloc((void *) buf, sizeof(File_name_t) * (idx + 1));
                    if (! array) {
                        if (buf) { free(buf); }
                        idx = 0;
                        break;
                    }
                    buf = array;
                    snprintf((char *) array[idx++], sizeof(File_name_t), "%s", namelist[i]->d_name);
                }
            }
        }

        *flist = array;
    }

    for (i = 0; i < status; ++i) {
        free(namelist[i]);
    }
    free(namelist);

    return idx;
}

/*----------------------------------------------------------------------------*/

void file_list_free (File_name_t *flist)
{
    if (flist) {
        free(flist);
    }
}

/*----------------------------------------------------------------------------*/

int file_goto_first_line (const char *fpath, char *line, int line_size)
{
    FILE *fp;
    int status = -1;

    memset(line, 0, line_size);

    fp = fopen(fpath, "r");
    if (fp) {
        if (! feof(fp)) {
            status = (NULL != fgets(line, line_size, fp)) ? 0 : -1;
        }
        fclose(fp);
    }

    return status;
}

/*----------------------------------------------------------------------------*/

int file_goto_last_line (const char *fpath, char *line, int line_size)
{
    FILE *fp;
    int  nline = 0;

    memset(line, 0, line_size);

    fp = fopen(fpath, "r");
    if (fp) {
        while (! feof(fp)) {
            if (NULL == fgets(line, line_size, fp)) {
                break;
            }
            nline++;
        }
        fclose(fp);
    }

    return nline;
}

/******************************************************************************/

