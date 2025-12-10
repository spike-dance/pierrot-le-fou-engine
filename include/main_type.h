#ifndef MAIN_TYPE_H
#define MAIN_TYPE_H

#include <stdbool.h>

typedef enum
{
        ENGINE,
        END
} Main_state;

bool is_main_state_fatal(Main_state state);

char* get_main_state(Main_state state);

Main_state main_state_handling(Main_state state);

#endif
