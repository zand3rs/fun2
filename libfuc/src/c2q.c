/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2007-04-02
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : c2q.c
 * Description : Two-lock concurrent queueing.
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
#include <errno.h>

#include "c2q.h"

/*============================================================================*/

static void c2q_node_free (C2q_node_t *q_node)
{
    if (q_node) {
        if (q_node->entry && q_node->entry_sz > 0) {
            free(q_node->entry);
        }
        memset(q_node, 0, sizeof(C2q_node_t));
        free(q_node);
    }
}

/*----------------------------------------------------------------------------*/

static C2q_node_t* c2q_node_new (void)
{
    C2q_node_t *q_node;

    q_node = (C2q_node_t *) malloc(sizeof(C2q_node_t));
    if (q_node) {
        memset(q_node, 0, sizeof(C2q_node_t));
    }
    return q_node;
}

/*============================================================================*/

int c2q_init (C2q_t *q)
{
    C2q_node_t *q_node;

    if (! q) {
        return -1;
    }

    memset(q, 0, sizeof(C2q_t));
    q_node = c2q_node_new();
    if (! q_node) {
        return -1;
    }
    q->head = q->tail = q_node;
    pthread_mutex_init(&(q->h_lock), NULL);
    pthread_mutex_init(&(q->t_lock), NULL);
    pthread_mutex_init(&(q->w_lock), NULL);
    pthread_cond_init (&(q->notempty), NULL);

    q->initialized = 1;

    return 0;
}

/*----------------------------------------------------------------------------*/

int c2q_deinit (C2q_t *q)
{
    C2q_node_t *q_head;
    C2q_node_t *q_node;

    if (! q || ! q->initialized) {
        return -1;
    }

    pthread_mutex_lock(&(q->h_lock));
    q_head = q->head;
    while (q_head) {
        q_node = q_head->next;
        c2q_node_free(q_head);
        q_head = q_node;
    }
    pthread_mutex_unlock(&(q->h_lock));

    pthread_cond_destroy (&(q->notempty));
    pthread_mutex_destroy(&(q->w_lock));
    pthread_mutex_destroy(&(q->t_lock));
    pthread_mutex_destroy(&(q->h_lock));

    memset(q, 0, sizeof(C2q_t));

    return 0;
}

/*----------------------------------------------------------------------------*/

int c2q_enqueue (C2q_t *q, void *entry, int entry_size)
{
    C2q_node_t *q_node;

    if (! q || ! q->initialized || ! entry || entry_size <= 0) {
        return -1;
    }

    q_node = c2q_node_new();
    if (! q_node) {
        return -1;
    }

    q_node->entry = (void *) malloc(entry_size);
    if (! q_node->entry) {
        c2q_node_free(q_node);
        return -1;
    }

    memcpy(q_node->entry, entry, entry_size);
    q_node->entry_sz = entry_size;

    pthread_mutex_lock(&(q->t_lock));
    q->tail->next = q_node;
    q->tail->next_ok = 1;
    q->tail = q_node;
    pthread_mutex_unlock(&(q->t_lock));

    pthread_cond_broadcast(&(q->notempty));

    return 0;
}

/*----------------------------------------------------------------------------*/

int c2q_dequeue (C2q_t *q, void *entry, int entry_size)
{
    C2q_node_t *q_head;
    C2q_node_t *q_node;
    int retr = -1;

    if (! q || ! q->initialized || ! entry || entry_size <= 0) {
        return -1;
    }

    pthread_mutex_lock(&(q->h_lock));
    q_head = q->head;
    if (q_head->next && q_head->next_ok) {
        q_node = q_head->next;
        if (q_node->entry_sz > 0) {
            memcpy(entry, q_node->entry, 
                    (q_node->entry_sz < entry_size) ? q_node->entry_sz : entry_size);
            q->head = q_node;
            c2q_node_free(q_head);
            retr = 0;
        }
    }
    pthread_mutex_unlock(&(q->h_lock));

    return retr;
}

/*----------------------------------------------------------------------------*/

int c2q_wait (C2q_t *q, int sec)
{
    struct timespec ts;
    int retr;

    if (! q || ! q->initialized) {
        return -1;
    }

    pthread_mutex_lock(&(q->w_lock));
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += sec;
    switch (pthread_cond_timedwait(&(q->notempty), &(q->w_lock), &ts)) {
        case 0: //-successfull...
            retr = 0;
            break;
        case ETIMEDOUT: //-timed out, return sleep sec...
            retr = sec;
            break;
        default: //-failed...
            retr = -1;
            break;
    }
    pthread_mutex_unlock(&(q->w_lock));

    return retr;
}

/*----------------------------------------------------------------------------*/

int c2q_is_empty (C2q_t *q)
{
    C2q_node_t *q_head;
    C2q_node_t *q_node;
    int retr = 1;

    if (! q || ! q->initialized) {
        return retr;
    }

    pthread_mutex_lock(&(q->h_lock));
    q_head = q->head;
    q_node = q_head->next;
    if (q_node) {
        retr = 0;
    }
    pthread_mutex_unlock(&(q->h_lock));

    return retr;
}

/*----------------------------------------------------------------------------*/

int c2q_lock (C2q_t *q)
{
    if (! q || ! q->initialized) {
        return -1;
    }

    pthread_mutex_lock(&(q->t_lock));
    pthread_mutex_lock(&(q->h_lock));

    return 0;
}

/*----------------------------------------------------------------------------*/

int c2q_unlock (C2q_t *q)
{
    if (! q || ! q->initialized) {
        return -1;
    }

    pthread_mutex_unlock(&(q->t_lock));
    pthread_mutex_unlock(&(q->h_lock));

    return 0;
}

/*============================================================================*/

int c2q_enqueue_ptr (C2q_t *q, void *entry)
{
    C2q_node_t *q_node;

    if (! q || ! q->initialized || ! entry) {
        return -1;
    }

    q_node = c2q_node_new();
    if (! q_node) {
        return -1;
    }

    q_node->entry = entry;
    q_node->entry_sz = 0;

    pthread_mutex_lock(&(q->t_lock));
    q->tail->next = q_node;
    q->tail->next_ok = 1;
    q->tail = q_node;
    pthread_mutex_unlock(&(q->t_lock));

    pthread_cond_broadcast(&(q->notempty));

    return 0;
}

/*----------------------------------------------------------------------------*/

int c2q_dequeue_ptr (C2q_t *q, void **entry)
{
    C2q_node_t *q_head;
    C2q_node_t *q_node;
    int retr = -1;

    if (! q || ! q->initialized || ! entry) {
        return -1;
    }

    pthread_mutex_lock(&(q->h_lock));
    q_head = q->head;
    if (q_head->next && q_head->next_ok) {
        q_node = q_head->next;
        if (q_node->entry_sz <= 0) {
            *entry = q_node->entry;
            q->head = q_node;
            c2q_node_free(q_head);
            retr = 0;
        }
    }
    pthread_mutex_unlock(&(q->h_lock));

    return retr;
}

/*============================================================================*/

C2q_t* c2q_new (void)
{
    C2q_t *q = (C2q_t *) malloc(sizeof(C2q_t));

    if (q) {
        memset(q, 0, sizeof(C2q_t));
    }

    return q;
}

/*----------------------------------------------------------------------------*/

void c2q_free (C2q_t *q)
{
    if (q) {
        memset(q, 0, sizeof(C2q_t));
        free(q);
    }
}

/******************************************************************************/

#ifdef __MAIN__

int main ()
{
    C2q_t q;
    char buf[64], key[256], val[256];
    char *pval;

    c2q_init(&q);

    //-- start setting...
    snprintf(val, sizeof(val), "%s", "val0");
    pval = val;

    printf("value is [%s]\n", pval);

    c2q_enqueue_ptr(&q, (void*)pval);

    //-- change val after set...
    snprintf(val, sizeof(val), "%s", "val1");

    //-- start getting...
    printf("\nAfter set...\n\n");

    pval = NULL;
    c2q_dequeue_ptr(&q, (void*)&pval);
    printf("value is [%s]\n", pval);

    c2q_deinit(&q);

    return 0;
}

#endif
