#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "cJSON.c"

void parse_func_return_type(char *jsonString, long fileSize) {
    cJSON *root = cJSON_Parse(jsonString);
    if (root == NULL) {
        printf("Error: Unable to parse the JSON root\n");
        free(jsonString);
    } else {
        cJSON *ext = cJSON_GetObjectItem(root, "ext");
        if (ext == NULL) {
            printf("Error: 'ext' object not found in JSON\n");
            cJSON_Delete(root);
        }
        long arraySize = cJSON_GetArraySize(ext);

        for (long idx = 0; idx < arraySize; idx++) {
            cJSON *idxJSON = cJSON_GetArrayItem(ext, idx);
            if (idxJSON == NULL) {
                printf("Error: Unable to retrieve object at index %ld\n", idx);
                cJSON_Delete(root);
            }

            cJSON *nodeType = cJSON_GetObjectItem(idxJSON, "_nodetype");
            if (strcmp(nodeType->valuestring, "FuncDef") == 0) {
                cJSON *declaration = cJSON_GetObjectItem(idxJSON, "decl");
                cJSON *type = cJSON_GetObjectItem(declaration, "type");

                cJSON *name = cJSON_GetObjectItem(declaration, "name");
                cJSON *returnType = cJSON_GetObjectItem(type, "type");

                cJSON *returnTypeName = cJSON_GetObjectItem(returnType, "type");
                if (returnTypeName) {
                    cJSON *nameValue = cJSON_GetArrayItem(returnTypeName, 0);
                    printf("Function name: %s\n", cJSON_PrintUnformatted(name));
                    printf("Return type: %s\n", cJSON_PrintUnformatted(nameValue));
                }
            }
        }

        cJSON_Delete(root);
    }
}

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error: Failed to open the JSON file\n");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *jsonString = (char *)malloc(fileSize + 1);
    if (jsonString == NULL) {
        fclose(file);
        printf("Error: Memory allocation failed\n");
        return 0;
    }

    fread(jsonString, 1, fileSize, file);
    jsonString[fileSize] = '\0';
    fclose(file);

    parse_func_return_type(jsonString, fileSize);

    free(jsonString);

    return 0;
}
