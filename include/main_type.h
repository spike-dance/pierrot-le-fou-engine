#ifndef MAIN_TYPE_H
#define MAIN_TYPE_H

#include <stdbool.h>

typedef enum
{
        ENGINE,
        END
} E_main;

bool fn_isMainStateFatal(E_main e_state);

char* fn_getMainState(E_main e_state);

E_main fn_mainStateHandling(E_main e_state);

#endif
