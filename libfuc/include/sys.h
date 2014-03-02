/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-09-05
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : sys.h
 * Description : System miscellaneous functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __SYS_H__
#define __SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/
    
/*-functions------------------------------------------------------------------*/

void sys_msleep (int msec);
void sys_daemon (void);
int  sys_bind (int port);

/*
 *  opt = sys_getopt(argc, argv, "c:", opt_arg, int opt_arg_size);
 *  Returns the option character, -1 otherwise...
 */
int  sys_getopt (int argc, char **argv, char *opts, char *opt_arg, int opt_arg_size);

void sys_sigblock (void);
/*
 * Wait for a signal and returns it, -1 on error...
 */
int  sys_sigwait (void);

/*
 * Send SIGABRT signal to every process in the process group of the current process.
 */
void sys_abort (void);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

