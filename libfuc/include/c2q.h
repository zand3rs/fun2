/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2007-04-02
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : c2q.h
 * Description : Two-lock concurrent queueing.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __C2Q_H__
#define __C2Q_H__

#include <pthread.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

typedef struct C2q_node {
    struct C2q_node *next;
    void *entry;
    int entry_sz;
    char next_ok;
} C2q_node_t;

typedef struct C2q {
    pthread_mutex_t h_lock;
    pthread_mutex_t t_lock;
    pthread_mutex_t w_lock;
    pthread_cond_t  notempty;
    C2q_node_t *head;
    C2q_node_t *tail;
    char initialized;
} C2q_t;

/*-functions------------------------------------------------------------------*/

int c2q_init (C2q_t *q);
int c2q_deinit (C2q_t *q);
int c2q_enqueue (C2q_t *q, void *entry, int entry_size);
int c2q_dequeue (C2q_t *q, void *entry, int entry_size);
int c2q_wait (C2q_t *q, int sec);
int c2q_is_empty (C2q_t *q);
int c2q_lock (C2q_t *q);
int c2q_unlock (C2q_t *q);

/*
 * Pointer functions...
 */
int c2q_enqueue_ptr (C2q_t *q, void *entry);
int c2q_dequeue_ptr (C2q_t *q, void **entry);

/*
 * Helper functions for creating and destroying C2q_t objects...
 */
C2q_t* c2q_new(void);
void   c2q_free(C2q_t *q);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

