#ifndef DALCBOOT_HASH_TABLE
#define DALCBOOT_HASH_TABLE

#include <stddef.h>
#include <stdbool.h>
#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/String.h"

typedef size_t (*HashFunction)(String string);
typedef struct HashEntry
{

    ListNode Header;
    String Key;
    void* Value;

} HashEntry;

typedef struct HashTable
{

    HashEntry* Buffer;
    size_t BufferSize;
    HashFunction Hash;

} HashTable;

HashTable createHashTableEmpty(void);
HashTable createHashTableSize(size_t size);
HashTable createHashTableSizeFunction(size_t size, HashFunction hash);

HashTable cloneHashTable(HashTable table);

void* addHashTable(HashTable table, String key, void* value);

void** getHashTableKey(HashTable table, String key);
void** getNextValueInHashTable(HashTable table, HashEntry** entry, size_t* index);

HashEntry* findHashEntryAtKey(HashTable table, String key);
HashEntry* findHashEntryWithKey(HashTable table, String key);

void* removeHashTableKey(HashTable table, String key);

bool containsHashTableKey(HashTable table, String key);

void finalizeHashTable(HashTable* table, void (*destroyData)(void* data));

#endif //DALCBOOT_HASH_TABLE
