#ifndef CUSTOM_TYPE_H
#define CUSTOM_TYPE_H

#include <stddef.h>

#include "error.h"

typedef struct
{
        void* buffer;
        size_t size;
        Error_enum error;
} Buffer;

#endif
