#include <stdio.h>

#include <main_type.h>

bool is_main_state_fatal(Main_state state)
{
        return true;
}

char* get_main_state(Main_state state)
{
        switch(state)
        {
                case ENGINE:
                        return "engine loop";
                case END:
                        return "end program";
        }
        return "undefine state enum";
}

Main_state main_state_handling(Main_state state)
{
        return END;
}
