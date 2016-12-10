#include <pebble.h>

#ifndef PEBBLE_TIMELORD_TYPES_H
#define PEBBLE_TIMELORD_TYPES_H

typedef struct {
    char *name;
    char *description;
    unsigned int start;
    unsigned int finish;
} show;

#endif //PEBBLE_TIMELORD_TYPES_H