#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "engine.h"
#include "context.h"

Main_state engine_loop()
{
        Main_state return_value = END;

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        GLFWwindow* window = glfwCreateWindow(800, 500, "Scene", NULL, NULL);

        Vulkan_context context = create_context(window);

        bool running = true;
        while(!running)
        {}

        clear_context(context);
        glfwDestroyWindow(window);

        return return_value;
}
