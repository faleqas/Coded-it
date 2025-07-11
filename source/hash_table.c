#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>

#include "hash_table.h"

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define TABLE_EXPAND_COUNT 2 //the amount of elements the table is expanded by
                             //when a resize is needed (in case of an index collision)

#define TABLE_DEFAULT_START_CAPACITY 10

static uint64_t fnv_hash(const char* key);


void hash_table_init(hash_table* table, int initial_len,
                     size_t element_size)
{
    assert(element_size >= 0 && "Size of element less than 0 ?");

    if (initial_len == 0)
    {
        initial_len = TABLE_DEFAULT_START_CAPACITY;
    }

    table->len = initial_len;
    table->count = 0;
    table->element_size = element_size;

    table->vals = calloc(table->len, table->element_size);
    table->key_hashes = calloc(table->len, sizeof(uint64_t));
}


void hash_table_clear(hash_table* table)
{
    if (!table) return;

    free(table->vals);
    free(table->key_hashes);
    for (int i = 0; i < table->len; i++)
    {
        if (table->keys[i])
        {
            free(table->keys[i]);
        }
    }
    free(table->keys);
    
    table->vals = NULL;
    table->key_hashes = NULL;
    table->keys = NULL;
    table->len = 0;
}


void hash_table_set(hash_table* table, const char* key, const char* val)
{
    assert(table->element_size && "Trying to use hash table without an element size. call hash_table_init() first");

    if (table->len == 0)
    {
        //the table was just cleared since element_size is initialized. 
        //create a new one
        hash_table_init(&table, TABLE_DEFAULT_START_CAPACITY, table->element_size);
    }

    uint64_t hash = fnv_hash(key);
    uint64_t index = hash % table->len;

//    printf("Key: %s, Index: %llu, Hash: %llu\n", key, index, hash);

    if (table->key_hashes[index])
    {
//        printf("KEY: %s, %llu vs %llu, %d\n", key, hash, table->key_hashes[index], index);
        if (table->key_hashes[index] != hash)
        {
            _hash_table_resize(table, table->len + TABLE_EXPAND_COUNT);
//            printf("resize\n");
            hash_table_set(table, key, val);
            return;
        }
    }
    
    char* dst_val = table->vals + (index * table->element_size);
    for (int i = 0; i < table->element_size; i++)
    {
        dst_val[i] = val[i];
    }

    table->key_hashes[index] = hash;

    table->count++;
    if (table->keys)
    {
        table->keys = realloc(table->keys, sizeof(char*) * table->count);
    }
    else
    {
        table->keys = malloc(sizeof(char*) * table->count);
    }

    char** key_dst = table->keys + (table->count-1);
    *key_dst = malloc(sizeof(char) * (strlen(key)+1));

    memcpy(*key_dst, key, sizeof(char) * (strlen(key) + 1));
}


char* hash_table_get(const hash_table* table, const char* key)
{
    if (table == NULL) return NULL;
    if (key == NULL) return NULL;

    uint64_t hash = fnv_hash(key);
    uint64_t index = hash % table->len;

    if (index >= table->len) return NULL;

    if (table->key_hashes[index] != hash)
    {
        return NULL;
    }
    
    return table->vals + (index * table->element_size);
}


char* hash_table_get_by_index(const hash_table* table, int index)
{
    if (table == NULL) return NULL;
    if (index >= table->len) return NULL;

    return table->vals + (index * table->element_size);
}


char* hash_table_get_key(const hash_table* table, int index)
{
    if (table == NULL) return NULL;
    if (index >= table->count) return NULL;
    
    return table->keys[index];
}


void _hash_table_resize(hash_table* table, int new_len)
{
    //create new key hashes and values arrays of the new_len. and recopy the
    //key value pairs while updating their indices to be the hash % the new length of
    //the table

    char* new_vals = calloc(new_len, table->element_size);
    uint64_t* new_key_hashes = calloc(new_len, sizeof(uint64_t));
    char** new_keys = calloc(new_len, sizeof(char*));

    uint64_t hash;
    char* val;
    int new_index = -1;
    for (int i = 0; i < table->len; i++)
    {
        //get the key, value pair at index i
        hash = table->key_hashes[i];
        val = table->vals + (i * table->element_size);

        //generate the new index (since the hash table's length changed)
        new_index = hash % new_len;
        if (new_key_hashes[new_index])
        {
            //TODO(omar): find a better way to handle this collision rather than expanding again and again
            free(new_vals);
            free(new_key_hashes);
            free(new_keys);
            _hash_table_resize(table, new_len + TABLE_EXPAND_COUNT);
            return;
        }

        //set the new index of the hash to the hash in the new key hashes array
        new_key_hashes[new_index] = hash;
       
        //same for the values array
        char* new_val = new_vals + (new_index * table->element_size);
        for (int j = 0; j < table->element_size; j++)
        {
            new_val[j] = *(val + j);
        }
    }

    free(table->vals);
    free(table->key_hashes);

    table->vals = new_vals;
    table->key_hashes = new_key_hashes;

    table->len = new_len;
}


void hash_table_print(const hash_table* table)
{
    if (table == NULL) return;

    for (int i = 0; i < table->count; i++)
    {
        printf("%s\n", table->keys[i]);
    }
}



//FNV-1a
static uint64_t fnv_hash(const char* key)
{
    if (key == NULL) return 0;
    uint64_t hash = FNV_OFFSET;

    for (int i = 0; i < strlen(key); i++)
    {
        hash ^= key[i];
        hash *= FNV_PRIME;
    }

    return hash;
}