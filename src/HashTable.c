#include "DaLanguageCBootstrap/HashTable.h"
#include "DaLanguageCBootstrap/String.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

size_t AHashFunction(String data)
{

    size_t hash = 5381;

    iterateStringNodesForward(data, curNode)
    {
        iterateStringNodeForward(curNode, i, curChar)
        {
            hash = ((hash << 5) + hash) + *curChar;
        }
    }

    return hash;

}

HashTable createHashTableEmpty(void)
{
    return (HashTable){0};
}
HashTable createHashTableSize(size_t size)
{
    return createHashTableSizeFunction(size, AHashFunction);
}
HashTable createHashTableSizeFunction(size_t size, HashFunction hash)
{

    assert(hash != NULL);

    HashTable ret = createHashTableEmpty();

    if(size == 0)
    {
        return ret;
    }

    ret.Buffer = calloc(size, sizeof(HashEntry));
    assert(ret.Buffer != NULL);
    ret.BufferSize = size;
    ret.Hash = hash;

    memset(ret.Buffer, 0, ret.BufferSize * sizeof(HashEntry));

    return ret;

}

HashTable cloneHashTable(HashTable table)
{

    HashTable ret;

    ret.Buffer = calloc(table.BufferSize, sizeof(HashEntry));
    assert(ret.Buffer != NULL);
    ret.BufferSize = table.BufferSize;
    ret.Hash = table.Hash;

    for(size_t i = 0; i < ret.BufferSize; ++i)
    {
        HashEntry* curEntry = ret.Buffer + i;
        HashEntry* otherEntry = table.Buffer + i;

        curEntry->Value = otherEntry->Value;
        curEntry->Key = cloneString(otherEntry->Key);
        curEntry->Header.Prev = NULL;

        HashEntry* prev = curEntry;
        iterateListNodeForwardType(
                entry, (HashEntry*)otherEntry->Header.Next, HashEntry, Header.Next
                )
        {
            HashEntry* newEntry = malloc(sizeof(HashEntry));
            assert(newEntry != NULL);

            prev->Header.Next = &newEntry->Header;
            newEntry->Header.Prev = &prev->Header;

            newEntry->Key = cloneString(entry->Key);
            newEntry->Value = entry->Value;

            prev = newEntry;
        }

        prev->Header.Next = NULL;

    }

    return ret;

}

void* addHashTable(HashTable table, String key, void* value)
{

    assert(table.BufferSize > 0);
    if(stringIsEqualToString(key, EMPTY_STRING)) assert(value != NULL);

    HashEntry* oldEntry = findHashEntryWithKey(table, key);

    if(oldEntry == NULL)
    {
        HashEntry* entry = findHashEntryAtKey(table, key);
        if(stringIsEqualToString(entry->Key, EMPTY_STRING) && entry->Value == NULL)
        {
            //We can put the value here, entry empty
            oldEntry = entry;
        }
        //Collision
        else
        {
            HashEntry* newEntry = malloc(sizeof(HashEntry));
            assert(newEntry != NULL);

            *newEntry = (struct HashEntry){0};

            HashEntry* lastEntry = (HashEntry*)listLast(&entry->Header);

            lastEntry->Header.Next = &newEntry->Header;
            newEntry->Header.Prev = &lastEntry->Header;
            newEntry->Header.Next = NULL;

            oldEntry = newEntry;
        }

        finalizeString(&oldEntry->Key);
        oldEntry->Key = cloneString(key);
    }

    void* oldVal = oldEntry->Value;

    oldEntry->Value = value;

    return oldVal;

}

void** getHashTableKey(HashTable table, String key)
{

    if(table.Buffer == NULL)
    {
        return NULL;
    }

    HashEntry* entry = findHashEntryWithKey(table, key);
    if(entry == NULL)
    {
        return NULL;
    }
    else
    {
        return &entry->Value;
    }

}
void** getNextValueInHashTable(HashTable table, HashEntry** entry, size_t* index)
{

    assert(table.BufferSize > 0);
    assert(entry != NULL);
    assert(index != NULL);
    assert(*index <= table.BufferSize);

    for(; *index < table.BufferSize; ++index)
    {
        for(; *entry != NULL; *entry = (HashEntry*)(*entry)->Header.Next)
        {
            *entry = (HashEntry*)(*entry)->Header.Next;
            return &(*entry)->Value;
        }

        *entry = table.Buffer + *index;
    }

    return NULL;

}

HashEntry* findHashEntryAtKey(HashTable table, String key)
{
    return table.Buffer + (table.Hash(key) % table.BufferSize);
}
HashEntry* findHashEntryWithKey(HashTable table, String key)
{

    assert(table.BufferSize > 0);

    iterateListNodeForwardType(curEntry, findHashEntryAtKey(table, key), HashEntry, Header.Next)
    {
        if(stringIsEqualToString(curEntry->Key, key))
        {
            return curEntry;
        }
    }

    return NULL;

}

void* removeHashTableKey(HashTable table, String key)
{

    assert(table.BufferSize > 0);

    HashEntry* entry = findHashEntryWithKey(table, key);
    if(entry != NULL)
    {
        void* ret = entry->Value;
        finalizeString(&entry->Key);

        //Entry in the buffer
        if(entry->Header.Prev == NULL)
        {
            if(entry->Header.Next != NULL)
            {
                HashEntry* junk = (HashEntry*)entry->Header.Next;

                *entry = *((HashEntry*)entry->Header.Next);

                entry->Header.Next = junk->Header.Next;
                if(junk->Header.Next != NULL)
                {
                    ((HashEntry*)junk->Header.Next)->Header.Prev = &entry->Header;
                }

                free(junk);
            }
            else
            {
                *entry = (struct HashEntry){0};
            }
        }
        //Floating entry
        else
        {
            ((HashEntry*)entry->Header.Prev)->Header.Next = entry->Header.Next;
            if(entry->Header.Next != NULL)
            {
                ((HashEntry*)entry->Header.Next)->Header.Prev = entry->Header.Prev;
            }

            free(entry);
        }

        return ret;

    }
    else
    {
        return NULL;
    }

}

//TODO: Refactor, 100% something incorrect, but unused so deferring.
void resizeHashTable(HashTable* table, size_t newSize)
{

    assert(table != NULL);
    assert(table->BufferSize > 0);
    assert(newSize > 0);

    if(table->BufferSize == newSize)
    {
        return;
    }

    HashEntry* newBuffer = calloc(newSize, sizeof(HashEntry));
    assert(newBuffer != NULL);

    memset(newBuffer, 0, newSize * sizeof(HashEntry));

    List freeEntries = {0};
    List waitingEntries = {0};
    for(size_t i = 0; i < table->BufferSize; ++i)
    {
        HashEntry* curEntry = table->Buffer + i;
        HashEntry* newEntry = newBuffer + (table->Hash(curEntry->Key) % newSize);

        if(newEntry->Key.TotalCharacters != 0)
        {
            for
                (
                 HashEntry* reusedEntry = (HashEntry*)listPopBackNode(&freeEntries);
                 reusedEntry != NULL;
                 reusedEntry = (HashEntry*)listPopBackNode(&freeEntries)
                )
            {
                HashEntry* newNewEntry = newBuffer + (table->Hash(reusedEntry->Key) % newSize);
                if(newNewEntry->Key.TotalCharacters != 0)
                {
                    HashEntry* last = (HashEntry*)listLast(&newNewEntry->Header);
                    last->Header.Next = &reusedEntry->Header;
                    reusedEntry->Header.Prev = &last->Header;
                    reusedEntry->Header.Next = NULL;
                    continue;
                }

                *newNewEntry = *reusedEntry;
                *reusedEntry = *curEntry;

                HashEntry* last = (HashEntry*)listLast(&newEntry->Header);
                last->Header.Next = &reusedEntry->Header;
                reusedEntry->Header.Prev = &last->Header;
                reusedEntry->Header.Next = NULL;
                break;
            }

            listPushFrontNode(&curEntry->Header, &waitingEntries);
            continue;
        }

        *newEntry = *curEntry;
        if(curEntry->Header.Next != NULL)
        {
            for(curEntry = (HashEntry*)curEntry->Header.Next; curEntry != NULL; curEntry = (HashEntry*)curEntry->Header.Next)
            {
                listPushFrontNode(&curEntry->Header, &freeEntries);
            }
        }
    }

    for
        (
         HashEntry* waiting = (HashEntry*)listPopBackNode(&waitingEntries);
         waiting != NULL;
         waiting = (HashEntry*)listPopBackNode(&waitingEntries)
        )
        {
        HashEntry* newEntry = NULL;

        for
            (
             newEntry = (HashEntry*)listPopBackNode(&freeEntries);
             newEntry != NULL;
             newEntry = (HashEntry*)listPopBackNode(&waitingEntries)
            )
        {
            HashEntry* newNewEntry = newBuffer + (table->Hash(newEntry->Key) % newSize);
            if(newNewEntry->Key.TotalCharacters != 0)
            {
                newEntry->Header.Next = newNewEntry->Header.Next;
                newEntry->Header.Prev = &newNewEntry->Header;
                newNewEntry->Header.Next = &newEntry->Header;
                if(newEntry->Header.Next != NULL)
                {
                    newEntry->Header.Next->Prev = &newEntry->Header;
                }
                continue;
            }

            *newNewEntry = *newEntry;
            break;
        }

        if(newEntry == NULL)
        {
            newEntry = malloc(sizeof(HashEntry));
            assert(newEntry != NULL);
            *newEntry = (struct HashEntry){0};
        }

        *newEntry = *waiting;
        HashEntry* addTo = newBuffer + (table->Hash(waiting->Key) % newSize);
        waiting->Header.Next = addTo->Header.Next;
        waiting->Header.Prev = &addTo->Header;
        addTo->Header.Next = &waiting->Header;
        if(waiting->Header.Next != NULL)
        {
            waiting->Header.Next->Prev = &waiting->Header;
        }
    }

    for
        (
         HashEntry* freeEntry = (HashEntry*)listPopBackNode(&freeEntries);
         freeEntry != NULL;
         freeEntry = (HashEntry*)listPopBackNode(&freeEntries)
        )
        {
            HashEntry* newEntry = newBuffer + (table->Hash(freeEntry->Key) % newSize);
            if(newEntry->Key.TotalCharacters != 0)
            {
                freeEntry->Header.Next = newEntry->Header.Next;
                freeEntry->Header.Prev = &newEntry->Header;
                newEntry->Header.Next = &freeEntry->Header;
                if(freeEntry->Header.Next != NULL)
                {
                    freeEntry->Header.Next->Prev = &freeEntry->Header;
                }
                continue;
            }

            *newEntry = *freeEntry;
            free(freeEntry);
            break;
        }

    free(table->Buffer);
    table->Buffer = newBuffer;
    table->BufferSize = newSize;

}

bool containsHashTableKey(HashTable table, String key)
{
    return getHashTableKey(table, key) != NULL;
}

void finalizeHashTable(HashTable* table, void (*destroyData)(void* data))
{

    assert(table != NULL);
    assert(destroyData != NULL);

    for(size_t i = 0; i < table->BufferSize; ++i)
    {
        HashEntry* entry = table->Buffer + i;
        finalizeString(&entry->Key);
        destroyData(entry->Value);

        HashEntry* lastEntry = NULL;
        iterateListNodeForwardType(
                curEntry, (HashEntry*)entry->Header.Next, HashEntry, Header.Next
                )
        {
            finalizeString(&curEntry->Key);
            destroyData(curEntry->Value);

            free(lastEntry);
            lastEntry = curEntry;
        }

        free(lastEntry);
    }

    free(table->Buffer);
    *table = (struct HashTable){0};

}

