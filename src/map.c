#include "map.h"
#include "util.h"
#include <string.h>

Map* newMap()
{
    Map* map = safeMalloc(sizeof(Map));
    map->keys = newVector();
    map->values = newVector();

    return map;
}

void freeMap(Map* map)
{
    freeVector(map->keys);
    freeVector(map->values);
    free(map);
}

void setMap(Map* map, char* key, void* value)
{
    pushVector(map->keys, key);
    pushVector(map->values, value);
}

void* getMap(Map* map, char* key)
{
    for (VECTOR_EACH(map->keys)) {
        if (!strcmp(VECTOR_GET(map->keys, i), key)) {
            return VECTOR_GET(map->values, i);
        }
    }

    return NULL;
}
