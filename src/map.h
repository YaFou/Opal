#ifndef OPAL_MAP_H
#define OPAL_MAP_H

#include "vector.h"

typedef struct {
    Vector* keys;
    Vector* values;
} Map;

Map* newMap();
void freeMap(Map* map);
void setMap(Map* map, char* key, void* value);
void* getMap(Map* map, char* key);

#endif
