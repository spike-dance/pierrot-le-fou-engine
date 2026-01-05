#include <GLFW/glfw3.h>

#include <my_std.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void fn_glfwErrorCallback(int errorCode, const char* v_errorString)
{
        fprintf(stderr, ANSI_RED_TEXT("GLFW error [%d] : ") "%s\n", errorCode, v_errorString);
}
