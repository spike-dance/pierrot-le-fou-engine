#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "main_type.h"
#include "global_variable.h"
#include "engine.h"

int main(int argc, char** argv)
{
        int return_value = 0;

        manage_main_arg(argc, argv);

        if(!glfwInit())
        {
                fprintf(stderr, "glfw cannot be init\n");
                return_value = -1;
                goto GLFW_NOT_INIT;
        }

        Main_state state = ENGINE;
        bool running = true;
        while(running)
        {
                switch(state)
                {
                        case ENGINE:
                                state = engine_loop();
                                break;
                        case END:
                                running = false;
                                break;
                        default:
                                if(is_main_state_fatal(state))
                                {
                                        running = false;
                                        return_value = state;
                                        fprintf(stderr, "fatal error :");
                                }

                                else
                                        fprintf(stderr, "error :");

                                fprintf(stderr, " %d \"%s\"\n", state, get_main_state(state));

                                state = main_state_handling(state);
                }
        }

        glfwTerminate();

GLFW_NOT_INIT:

        return return_value;
}
