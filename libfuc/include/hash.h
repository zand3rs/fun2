/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-09-25
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : hash.h
 * Description : Data hashing and hash table implementation.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __HASH_H__
#define __HASH_H__

#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

/* 
 * Lets set the max table size,
 * collisions will be handled by the linked list key-val mapping anyways...
 * 0x0000FFFF=65kb
 * 0x000FFFFF=1mb
 */
#define HASH_MAX_MAP_SIZE 0x000FFFFF

typedef struct _Hash_map {
    struct _Hash_map *next;
    uint32_t hash;
    char *key;
    void *val;
    int val_sz;
} _Hash_map_t;

typedef struct Hashtable {
    pthread_mutex_t lock;
    _Hash_map_t *map;
    unsigned int map_sz;
    char initialized;
} Hashtable_t;

/*-functions------------------------------------------------------------------*/

uint32_t hash_generate (const char *key, const size_t key_len);
uint32_t hash_get_index (uint32_t hash, uint32_t max_size);

/*
 * Hash table functions...
 */
int hash_table_init (Hashtable_t *t, unsigned int t_size);
int hash_table_deinit (Hashtable_t *t);
int hash_table_set (Hashtable_t *t, char *key, void *val, int val_size);
int hash_table_get (Hashtable_t *t, char *key, void *val, int val_size);
int hash_table_del (Hashtable_t *t, char *key);

/*
 * Pointer functions...
 */
int hash_table_set_ptr (Hashtable_t *t, char *key, void *val);
int hash_table_get_ptr (Hashtable_t *t, char *key, void **val);

/*
 * Helper functions for creating and destroying Hashtable_t objects...
 */
Hashtable_t* hash_table_new (void);
void         hash_table_free (Hashtable_t *t);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

