cmake_minimum_required(VERSION 3.15)
project(pierrot-le-fou-engine)

include_directories(include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SOURCE src/context.c
        src/global_variable.c
        src/main_type.c
        src/engine.c
        src/main.c
        src/vulkan_error.c)

set(BIN pierrot)

add_executable(${BIN} ${SOURCE})
if(DEBUG)
        target_compile_definitions(${BIN} PUBLIC DEBUG)
endif()

target_link_libraries(${BIN} glfw vulkan)
