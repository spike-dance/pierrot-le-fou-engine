#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "error.h"
#include "custom_type.h"

Buffer read_file(char* file_path)
{
        Buffer file_content = {0};

        FILE* file = fopen(file_path, "r");
        if(file == NULL)
        {
                fprintf(stderr, "Error cannot open \"%s\"\n", file_path);
                file_content.error = ACCESS_FAILED;
                return file_content;
        }

        fseek(file, 0, SEEK_END);
        file_content.size = ftell(file);
        fseek(file, 0, SEEK_SET);

        if(file_content.size == 0)
        {
                fprintf(stderr, "Warning \"%s\" is empty\n", file_path);
                return file_content;
        }

        file_content.buffer = malloc(file_content.size);
        if(file_content.buffer == NULL)
        {
                fprintf(stderr, "Error allocation failed\n");
                file_content.error = ALLOCATION_FAILED;
                return file_content;
        }

        fread(file_content.buffer, 1, file_content.size, file);

        fclose(file);
        return file_content;
}
