#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <my_std.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "main_type.h"
#include "global_variable.h"
#include "engine.h"
#include "error.h"

i32 main(i32 argc, char** vv_argv)
{
        i32 returnValue = 0;

        fn_manageMainArg(argc, vv_argv);

        glfwSetErrorCallback(fn_glfwErrorCallback);
        if(!glfwInit())
        {
                fprintf(stderr, "glfw cannot be init\n");
                returnValue = -1;
                goto GO_END_GLFW_NOT_INIT;
        }

        E_main e_state = ENGINE;
        bool running = true;
        while(running)
        {
                switch(e_state)
                {
                        case ENGINE:
                                e_state = fn_engineLoop();
                                break;
                        case END:
                                running = false;
                                break;
                        default:
                                if(fn_isMainStateFatal(e_state))
                                {
                                        running = false;
                                        returnValue = e_state;
                                        fprintf(stderr, ANSI_RED_TEXT("fatal error :"));
                                }

                                else
                                        fprintf(stderr, ANSI_PURPLE_TEXT("error :"));

                                fprintf(stderr, " %d \"%s\"\n", e_state, fn_getMainState(e_state));

                                e_state = fn_mainStateHandling(e_state);
                }
        }

        glfwTerminate();

GO_END_GLFW_NOT_INIT:
        return returnValue;
}
