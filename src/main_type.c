#include <stdio.h>

#include <main_type.h>

bool fn_isMainStateFatal(E_main e_state)
{
        return true;
}

char* fn_getMainState(E_main e_state)
{
        switch(e_state)
        {
                case ENGINE:
                        return "engine loop";
                case END:
                        return "end program";
        }
        return "undefine state enum";
}

E_main fn_mainStateHandling(E_main e_state)
{
        return END;
}
