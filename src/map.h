#ifndef OPAL_MAP_H
#define OPAL_MAP_H

#include "vector.h"

#define MAP_EACH(map) int i = 0; i < VECTOR_SIZE(map->keys); i++
#define MAP_GET_INDEX(map, index) VECTOR_GET(map->values, index)

typedef struct {
    Vector* keys;
    Vector* values;
} Map;

Map* newMap();
void freeMap(Map* map);
void setMap(Map* map, char* key, void* value);
void* getMap(Map* map, char* key);

#endif
