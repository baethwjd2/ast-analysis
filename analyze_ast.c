#include <stdio.h>
#include "json_c.c"
#include <string.h>
#define MAX 1000000


char* parse_func_name(json_value decl){
    char* func_name = "";

    json_value name = json_get(decl, "name");
    printf("- Name: "); json_print(name); putchar("\n");

    return func_name;
}

int main(int argc, char* argv[]){
    FILE* fp = fopen(argv[1], "r");  
    char file[MAX] = {0, };
    int func_cnt = 0;

    fread(file, 1, MAX, fp); 
    fclose(fp); 
    
    json_value data = json_create(file);
    json_value ext = json_get(data, "ext");

    for(int i=0; i<json_len(ext); i++){
        json_value node = json_get(data, "ext", i, "_nodetype");
        char* _nodetype = json_get_string(node);

        if (strcmp(_nodetype, "FuncDef")==0){
            func_cnt++;
            printf("Function %d:\n", func_cnt);
            
            json_value body = json_get(data, "ext", i, "body");
            json_value decl = json_get(data, "ext", i, "decl");
            parse_func_name(decl);
        }
    }
    
    printf("Total Number of Function: %d\n", func_cnt);

    json_free(data);

    return 0;
}
    
