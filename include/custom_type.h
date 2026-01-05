#ifndef CUSTOM_TYPE_H
#define CUSTOM_TYPE_H

#include <my_std.h>

#include "error.h"

typedef struct
{
        void* buffer;
        size_t size;
        E_error error;
} Buffer;

#endif
