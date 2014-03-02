/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2003-05-02
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : str.h
 * Description : Functions for manipulating strings.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __STR_H__
#define __STR_H__

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

typedef enum Strpad { 
    STR_PAD_LEFT,
    STR_PAD_RIGHT,
    STR_PAD_BOTH,
    STR_PAD_LEFT_MASKED,
    STR_PAD_RIGHT_MASKED,
    STR_PAD_BOTH_MASKED
} Strpad_t;

/*-functions------------------------------------------------------------------*/
/*
 * Return value:
 *      int   => 0 if successful, -1 otherwise
 *      char* => pointer to a string if successful, NULL otherwise 
 */ 

/*
 * str_chomp - Derived from perl's chomp function.
 *           - Strip \r and \n from the end of a string.
 */ 
int str_chomp    (char *s);

/*
 * str_ltrim - Strip whitespace from the beginning of a string.
 */ 
int str_ltrim    (char *s);

/*
 * str_rtrim - Strip whitespace from the end of a string.
 */ 
int str_rtrim    (char *s);

/*
 * str_trim - Strip whitespace from the beginning and end of a string.
 */ 
int str_trim     (char *s);

/*
 * str_ltrim_char - Strip the given character from the beginning of a string.
 */ 
int str_ltrim_char    (char *s, char c);

/*
 * str_rtrim - Strip the given character from the end of a string.
 */ 
int str_rtrim_char    (char *s, char c);

/*
 * str_trim - Strip the given character from the beginning and end of a string.
 */ 
int str_trim_char     (char *s, char c);

/*
 * str_toupper - Make a string uppercase.
 */ 
int str_toupper  (char *s);

/*
 * str_tolower - Make a string lowercase.
 */ 
int str_tolower  (char *s);

/*
 * str_ucwords - Uppercase the first character of each word in a string.
 */ 
int str_ucwords  (char *s);

/*
 * str_reverse - Reverse a string.
 */ 
int str_reverse  (char *s);

/*
 * str_rand - Generate a random string.
 */ 
int str_rand     (char *s, int len);

/*
 * str_tok - Tokenize a string.
 *         - dest's size should always be greater than or equal to src's size
 */ 
int str_tok      (char *dest, char *src, const char *needle);

/*
 * str_tok2 - Tokenize a string.
 *          - Out of bounds safe.
 */ 
int str_tok2     (char *dest, char **src, int dest_size, const char *needle);

/*
 * str_slice - Move part of src string to dest buffer.
 */ 
int str_slice    (char *dest, char *src, int len);

/*
 * str_substr - Copy part of src string to dest buffer.
 */ 
int str_substr   (char *dest, const char *src, int idx, int len);

/*
 * str_pad - Pad src string with padstr using padtype and 
 *           copy the result to dest buffer.
 */ 
int str_pad      (char *dest, const char *src, int len
                            , const char *padstr, Strpad_t padtype);

/*
 * str_escape - Escape special characters.
 */ 
int str_escape   (char *dest, const char *src, int dest_size); 

/*
 * str_unescape - Unescape special characters.
 */ 
int str_unescape (char *dest, const char *src, int dest_size); 

/*
 * str_dos2unix - Convert src string from dos to unix format.
 */ 
int str_dos2unix (char *dest, const char *src, int dest_size);

/*
 * str_unix2dos - Convert src string from unix to dos format.
 */ 
int str_unix2dos (char *dest, const char *src, int dest_size);

/*
 * str_replace - Replace search string in src with replace string
 *               and copy the result to dest buffer.
 */ 
int str_replace  (char *dest, const char *src, int dest_size
                            , const char *search, const char *replace);

/*
 * str_chr - Returns a pointer to the first non-whitespace character.
 *           NULL is returned if no printable chars found.
 */ 
char* str_chr    (const char *s);

/*
 * str_rchr - Returns a pointer to the last non-whitespace character.
 *            NULL is returned if no printable chars found.
 */ 
char* str_rchr   (const char *s);

/*
 * str_is_* - Returns 1 if true, otherwise 0...
 *            
 */ 
int str_is_printable (const char *s);
int str_is_alpha (const char *s);
int str_is_numeric (const char *s);
int str_is_alnum (const char *s);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

