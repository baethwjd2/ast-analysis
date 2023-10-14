// Mac : brew install jansson
// Ubuntu : sudo apt-get install libjansson-dev
// gcc -o count_if_statements count_if_statements.c -ljansson -w 

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <string.h>

#define MAX 1000000

int count_if_statements(json_t *node) {
    int if_cnt = 0;
    json_t *nodetype = json_object_get(node, "_nodetype");

    if(json_is_string(nodetype)) {
        char *_nodetype = json_string_value(nodetype);
        if(strcmp(_nodetype, "If") == 0) {
            if_cnt++;
        }
    }

    if (json_is_array(node)) {
        size_t size = json_array_size(node);
        for (size_t i = 0; i < size; i++) {
            json_t *sub_node = json_array_get(node, i);
            if_cnt += count_if_statements(sub_node);
        }
    } 
    else if (json_is_object(node)) {
        const char *k;
        json_t *value;

        json_object_foreach(node, k, value) {
            if_cnt += count_if_statements(value);
        }
    }

    return if_cnt;
}

int main(int argc, char* argv[]) {
    char input_file[256];
    
    strcpy(input_file, argv[1]); 

    json_error_t error;
    json_t *root = json_load_file(input_file, 0, &error);
    printf("Total Number of IF Statement: %d\n", count_if_statements(root));

    json_decref(root);
    return 0;
}
