/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-10-12
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : file.h
 * Description : File handling functions...
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __FILE_H__
#define __FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

typedef char File_name_t[256];

/*-functions------------------------------------------------------------------*/

void file_trim (char *path);
void file_basename (char *dest, char *src);
void file_dirname (char *dest, char *src);
int  file_exists (char *path);
int  file_is_dir (char *path);
int  file_mkdir (char *path);
int  file_move (char *oldpath, char *newpath);
int  file_copy (char *srcpath, char *destpath);
int  file_delete (char *path);

int  file_list_get_first (const char *dir, const char *pattern, char *fname);
int  file_list_get_last (const char *dir, const char *pattern, char *fname);

/* result of this function must be freed using file_list_free() */
int  file_list (const char *dir, const char *pattern, File_name_t **flist);
void file_list_free (File_name_t *flist);

int  file_goto_first_line (const char *fpath, char *line, int line_size);
int  file_goto_last_line  (const char *fpath, char *line, int line_size);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

