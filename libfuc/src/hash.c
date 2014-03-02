/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-09-25
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : hash.c
 * Description : Data hashing and hash table implementation.
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

#include "hash.h"

/*============================================================================*/

static uint32_t jenkins_one_at_a_time_hash (const char *key, const size_t key_len)
{
    uint32_t hash = 0;
    size_t i;

    for (i = 0; i < key_len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/*----------------------------------------------------------------------------*/

static uint32_t _hash_get_index (uint32_t hash, uint32_t max)
{
    //return (uint32_t) (hash % max);
    return (uint32_t) (hash & max);
}

/*----------------------------------------------------------------------------*/

static uint32_t num_to_num2 (uint32_t n)
{
    if (n & 0xF0000000) {
        return (uint32_t) 0xFFFFFFFF;
    }
    if (n & 0x0F000000) {
        return (uint32_t) 0x0FFFFFFF;
    }
    if (n & 0x00F00000) {
        return (uint32_t) 0x00FFFFFF;
    }
    if (n & 0x000F0000) {
        return (uint32_t) 0x000FFFFF;
    }
    if (n & 0x0000F000) {
        return (uint32_t) 0x0000FFFF;
    }
    if (n & 0x00000F00) {
        return (uint32_t) 0x00000FFF;
    }
    if (n & 0x000000F0) {
        return (uint32_t) 0x000000FF;
    }
    if (n & 0x0000000F) {
        return (uint32_t) 0x0000000F;
    }
    else {
        return (uint32_t) 0x0000000F;
    }
}

/*============================================================================*/

static _Hash_map_t* map_node_new (void)
{
    _Hash_map_t *map_node;

    map_node = (_Hash_map_t*) malloc(sizeof(_Hash_map_t));
    if (map_node) {
        memset(map_node, 0, sizeof(_Hash_map_t));
    }

    return map_node;
}

/*----------------------------------------------------------------------------*/

static void map_node_free (_Hash_map_t *map_node)
{
    if (map_node) {
        if (map_node->key) {
            free(map_node->key);
        }
        if (map_node->val && map_node->val_sz > 0) {
            free(map_node->val);
        }
        memset(map_node, 0, sizeof(_Hash_map_t));
        free(map_node);
    }
}

/*----------------------------------------------------------------------------*/

static void map_node_free_all (_Hash_map_t *map_root)
{
    _Hash_map_t *map_head;
    _Hash_map_t *map_node;

    if (map_root) {
        map_head = map_root->next;
        while (map_head) {
            map_node = map_head->next;
            map_node_free(map_head);
            map_head = map_node;
        }
    }
}

/*----------------------------------------------------------------------------*/

static int map_node_set (_Hash_map_t *map_root, uint32_t hash, char *key, void *val, int val_size)
{
    _Hash_map_t *map_head;
    _Hash_map_t *map_tail;
    _Hash_map_t *map_node;

    if (! map_root) {
        return -1;
    }

    map_tail = map_node_new();
    if (! map_tail) {
        return -1;
    }

    map_tail->hash = hash;
    map_tail->key = strdup(key);
    if (! map_tail->key) {
        map_node_free(map_tail);
        return -1;
    }
    
    map_tail->val = (void *) malloc(val_size);
    if (! map_tail->val) {
        map_node_free(map_tail);
        return -1;
    }
    memcpy(map_tail->val, val, val_size);
    map_tail->val_sz = val_size;
    
    map_head = map_root;
    map_node = map_head->next;
    while (map_node) {
        if (map_node->hash == hash && ! strcmp(map_node->key, key) && map_node->val_sz > 0) {
            map_head->next = map_node->next;
            map_node_free(map_node);
            map_node = map_head->next;
        } else {
            map_head = map_node;
            map_node = map_head->next;
        }
    }

    map_head->next = map_tail;

    return 0;
}

/*----------------------------------------------------------------------------*/

static int map_node_set_ptr (_Hash_map_t *map_root, uint32_t hash, char *key, void *val)
{
    _Hash_map_t *map_head;
    _Hash_map_t *map_tail;
    _Hash_map_t *map_node;

    if (! map_root) {
        return -1;
    }

    map_tail = map_node_new();
    if (! map_tail) {
        return -1;
    }

    map_tail->hash = hash;
    map_tail->key = strdup(key);
    if (! map_tail->key) {
        map_node_free(map_tail);
        return -1;
    }
    
    map_tail->val = val;
    map_tail->val_sz = 0;
    
    map_head = map_root;
    map_node = map_head->next;
    while (map_node) {
        if (map_node->hash == hash && ! strcmp(map_node->key, key) && map_node->val_sz <= 0) {
            map_head->next = map_node->next;
            map_node_free(map_node);
            map_node = map_head->next;
        } else {
            map_head = map_node;
            map_node = map_head->next;
        }
    }

    map_head->next = map_tail;

    return 0;
}

/*----------------------------------------------------------------------------*/

static int map_node_get (_Hash_map_t *map_root, uint32_t hash, char *key, void *val, int val_size)
{
    _Hash_map_t *map_node;

    if (! map_root) {
        return -1;
    }

    map_node = map_root->next;
    while (map_node) {
        if (map_node->hash == hash && ! strcmp(map_node->key, key) && map_node->val_sz > 0) {
            break;
        }
        map_node = map_node->next;
    }
    if (! map_node) {
        return -1;
    }

    memcpy(val, map_node->val,
            (map_node->val_sz < val_size) ? map_node->val_sz : val_size);

    return 0;
}

/*----------------------------------------------------------------------------*/

static int map_node_get_ptr (_Hash_map_t *map_root, uint32_t hash, char *key, void **val)
{
    _Hash_map_t *map_node;

    if (! map_root) {
        return -1;
    }

    map_node = map_root->next;
    while (map_node) {
        if (map_node->hash == hash && ! strcmp(map_node->key, key) && map_node->val_sz <= 0) {
            break;
        }
        map_node = map_node->next;
    }
    if (! map_node) {
        return -1;
    }

    *val = map_node->val;

    return 0;
}

/*----------------------------------------------------------------------------*/

static int map_node_del (_Hash_map_t *map_root, uint32_t hash, char *key)
{
    _Hash_map_t *map_head;
    _Hash_map_t *map_node;
    int retr = -1;

    if (! map_root) {
        return -1;
    }

    map_head = map_root;
    map_node = map_head->next;
    while (map_node) {
        if (map_node->hash == hash && ! strcmp(map_node->key, key)) {
            map_head->next = map_node->next;
            map_node_free(map_node);
            map_node = map_head->next;
            retr = 0;
        } else {
            map_head = map_node;
            map_node = map_head->next;
        }
    }

    return retr;
}

/*============================================================================*/

uint32_t hash_generate (const char *key, const size_t key_len)
{
    return (uint32_t) jenkins_one_at_a_time_hash(key, key_len);
}

/*----------------------------------------------------------------------------*/

uint32_t hash_get_index (uint32_t hash, uint32_t max_size)
{
    return (uint32_t) _hash_get_index(hash, max_size);
}

/*----------------------------------------------------------------------------*/

int hash_table_init (Hashtable_t *t, unsigned int t_size)
{
    if (! t) {
        return -1;
    }

    memset(t, 0, sizeof(Hashtable_t));
    t->map_sz = (unsigned int) (num_to_num2((uint32_t) t_size) & HASH_MAX_MAP_SIZE);
    if (! t->map_sz) {
        t->map_sz = (unsigned int) HASH_MAX_MAP_SIZE;
    }
    t->map = (_Hash_map_t *) malloc(sizeof(_Hash_map_t) * (t->map_sz + 1)); 
    if (! t->map) {
        return -1;
    }
    memset(t->map, 0, sizeof(_Hash_map_t) * (t->map_sz + 1));
    pthread_mutex_init(&(t->lock), NULL);

    t->initialized = 1;

    return 0;
}

/*----------------------------------------------------------------------------*/

int hash_table_deinit (Hashtable_t *t)
{
    unsigned int i;

    if (! t || ! t->initialized) {
        return -1;
    }

    pthread_mutex_lock(&(t->lock));
    if (t->map) {
        for (i = 0; i <= t->map_sz; ++i) {
            map_node_free_all(&(t->map[i]));
        }
        free(t->map);
    }
    pthread_mutex_unlock(&(t->lock));
    pthread_mutex_destroy(&(t->lock));

    memset(t, 0, sizeof(Hashtable_t));

    return 0;
}

/*----------------------------------------------------------------------------*/

int hash_table_set (Hashtable_t *t, char *key, void *val, int val_size)
{
    uint32_t hash;
    unsigned int idx;
    int retr;

    if (! t || ! t->initialized) {
        return -1;
    }
    if (! key || ! *key || val_size <= 0) {
        return -1;
    }

    pthread_mutex_lock(&(t->lock));

    hash = hash_generate(key, strlen(key));
    idx = (unsigned int) hash_get_index(hash, t->map_sz); 
    retr = map_node_set(&(t->map[idx]), hash, key, val, val_size);

    pthread_mutex_unlock(&(t->lock));

    return retr;
}

/*----------------------------------------------------------------------------*/

int hash_table_get (Hashtable_t *t, char *key, void *val, int val_size)
{
    uint32_t hash;
    unsigned int idx;
    int retr;

    if (! t || ! t->initialized) {
        return -1;
    }
    if (! key || ! *key) {
        return -1;
    }

    pthread_mutex_lock(&(t->lock));

    hash = hash_generate(key, strlen(key));
    idx = (unsigned int) hash_get_index(hash, t->map_sz); 
    retr = map_node_get(&(t->map[idx]), hash, key, val, val_size);

    pthread_mutex_unlock(&(t->lock));

    return retr;
}

/*----------------------------------------------------------------------------*/

int hash_table_del (Hashtable_t *t, char *key)
{
    uint32_t hash;
    unsigned int idx;
    int retr;

    if (! t || ! t->initialized) {
        return -1;
    }
    if (! key || ! *key) {
        return -1;
    }

    pthread_mutex_lock(&(t->lock));

    hash = hash_generate(key, strlen(key));
    idx = (unsigned int) hash_get_index(hash, t->map_sz); 
    retr = map_node_del(&(t->map[idx]), hash, key);

    pthread_mutex_unlock(&(t->lock));

    return retr;
}

/*============================================================================*/

int hash_table_set_ptr (Hashtable_t *t, char *key, void *val)
{
    uint32_t hash;
    unsigned int idx;
    int retr;

    if (! t || ! t->initialized) {
        return -1;
    }
    if (! key || ! *key) {
        return -1;
    }

    pthread_mutex_lock(&(t->lock));

    hash = hash_generate(key, strlen(key));
    idx = (unsigned int) hash_get_index(hash, t->map_sz); 
    retr = map_node_set_ptr(&(t->map[idx]), hash, key, val);

    pthread_mutex_unlock(&(t->lock));

    return retr;
}

/*----------------------------------------------------------------------------*/

int hash_table_get_ptr (Hashtable_t *t, char *key, void **val)
{
    uint32_t hash;
    unsigned int idx;
    int retr;

    if (! t || ! t->initialized) {
        return -1;
    }
    if (! key || ! *key) {
        return -1;
    }

    pthread_mutex_lock(&(t->lock));

    hash = hash_generate(key, strlen(key));
    idx = (unsigned int) hash_get_index(hash, t->map_sz); 
    retr = map_node_get_ptr(&(t->map[idx]), hash, key, val);

    pthread_mutex_unlock(&(t->lock));

    return retr;
}

/*============================================================================*/

Hashtable_t* hash_table_new (void)
{
    Hashtable_t *t = (Hashtable_t *) malloc(sizeof(Hashtable_t));

    if (t) {
        memset(t, 0, sizeof(Hashtable_t));
    }

    return t;
}

/*----------------------------------------------------------------------------*/

void hash_table_free (Hashtable_t *t)
{
    if (t) {
        memset(t, 0, sizeof(Hashtable_t));
        free(t);
    }
}

/******************************************************************************/

#ifdef __MAIN__

int main ()
{
#if 0
    char buf[64], key[256], val[256];
    uint32_t hash, n;
    Hashtable_t ht;

    n = 1;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 10;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 100;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 1000;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 10000;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 100000;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 1000000;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 10000000;
    printf("num2 of [%lu] is [%lu]\n", n, num_to_num2(n));
    n = 100000000;
    printf("num2 of [%lu] is [%lu]\n\n", n, num_to_num2(n));

    snprintf(buf, sizeof(buf), "%s", "key0");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key1");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key2");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key3");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key4");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key5");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key6");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key7");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key8");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    snprintf(buf, sizeof(buf), "%s", "key9");
    hash = jenkins_one_at_a_time_hash(buf, strlen(buf));
    printf("hash of [%s] is [%lu], index is [%d]\n", buf, hash, hash_get_index(hash, num_to_num2(10)));

    hash_table_init(&ht, 10);
    
    //-- start setting...
    snprintf(key, sizeof(key), "%s", "key0");
    snprintf(val, sizeof(val), "%s", "val0");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key1");
    snprintf(val, sizeof(val), "%s", "val1");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key2");
    snprintf(val, sizeof(val), "%s", "val2");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key3");
    snprintf(val, sizeof(val), "%s", "val3");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key4");
    snprintf(val, sizeof(val), "%s", "val4");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key5");
    snprintf(val, sizeof(val), "%s", "val5");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key6");
    snprintf(val, sizeof(val), "%s", "val6");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key7");
    snprintf(val, sizeof(val), "%s", "val7");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key8");
    snprintf(val, sizeof(val), "%s", "val8");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key9");
    snprintf(val, sizeof(val), "%s", "val9");
    hash_table_set(&ht, key, val, sizeof(val));

    snprintf(key, sizeof(key), "%s", "key0");
    snprintf(val, sizeof(val), "%s", "val10");
    hash_table_set(&ht, key, val, sizeof(val));
    

    //-- start getting...
    printf("\nAfter set...\n\n");

    snprintf(key, sizeof(key), "%s", "key0");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key1");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key2");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key3");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key4");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key5");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key6");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key7");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key8");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key9");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);
    
    //-- start deleting...
    snprintf(key, sizeof(key), "%s", "key0");
    hash_table_del(&ht, key);

    snprintf(key, sizeof(key), "%s", "key1");
    hash_table_del(&ht, key);

    //-- start getting again...
    printf("\nAfter del...\n\n");

    snprintf(key, sizeof(key), "%s", "key0");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key1");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key2");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key3");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key4");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key5");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key6");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key7");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key8");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key9");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    hash_table_deinit(&ht);


    char buf[64], key[256], val[256];
    struct buf sval;
    Hashtable_t ht;

    hash_table_init(&ht, 10);

    //-- start setting...
    snprintf(key, sizeof(key), "%s", "key0");
    snprintf(val, sizeof(val), "%s", "val0");
    sval.pval = val;
    hash_table_set(&ht, key, (void*) &sval, sizeof(sval));

    snprintf(key, sizeof(key), "%s", "key0");
    sval.pval = NULL;
    hash_table_get(&ht, key, (void*) &sval, sizeof(sval));
    printf("value of key [%s] is [%s]\n", key, sval.pval);

    hash_table_deinit(&ht);

#endif

    char buf[64], key[256], val[256];
    char *pval;
    Hashtable_t ht;

    hash_table_init(&ht, 10);

    //-- start setting...
    printf("\nSet...\n\n");
    snprintf(key, sizeof(key), "%s", "key0");
    snprintf(val, sizeof(val), "%s", "val0");
    pval = val;
    hash_table_set_ptr(&ht, key, (void*)pval);

    //-- get val after set...
    snprintf(key, sizeof(key), "%s", "key0");
    pval = NULL;
    hash_table_get_ptr(&ht, key, (void*)&pval);
    printf("value of key [%s] is [%s]\n", key, pval);

    //-- change val after set...
    snprintf(val, sizeof(val), "%s", "val1");

    //-- start getting...
    printf("\nAfter set...\n\n");

    snprintf(key, sizeof(key), "%s", "key0");
    pval = NULL;
    hash_table_get_ptr(&ht, key, (void*)&pval);
    printf("value of key [%s] is [%s]\n", key, pval);

    printf("\nSet again...\n\n");
    snprintf(key, sizeof(key), "%s", "key0");
    snprintf(val, sizeof(val), "%s", "val0");
    hash_table_set(&ht, key, val, sizeof(val));
    
    snprintf(key, sizeof(key), "%s", "key1");
    snprintf(val, sizeof(val), "%s", "val1");
    hash_table_set(&ht, key, val, sizeof(val));
    
    //-- start getting...
    snprintf(key, sizeof(key), "%s", "key0");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key1");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    printf("\nDelete...\n\n");
    snprintf(key, sizeof(key), "%s", "key0");
    hash_table_del(&ht, key);

    snprintf(key, sizeof(key), "%s", "key1");
    hash_table_del(&ht, key);

    //-- start getting...
    snprintf(key, sizeof(key), "%s", "key0");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    snprintf(key, sizeof(key), "%s", "key1");
    memset(val, 0, sizeof(val));
    hash_table_get(&ht, key, val, sizeof(val));
    printf("value of key [%s] is [%s]\n", key, val);

    hash_table_deinit(&ht);

    return 0;
}
#endif

