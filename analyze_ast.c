#include <stdio.h>
#include "json_c.c"
#include <string.h>

#define MAX 1000000


void parse_func_name(json_value decl){
    char* func_name = json_get_string(json_get(decl, "name"));

    printf("- Name: %s\n", strtok(func_name, "\""));
}


void parse_func_var(json_value decl){
    char* var_name="", var_type="";

    json_value type = json_get(decl, "type");
    json_value args = json_get(type, "args");
    
    if(!json_is_null(args)){
        json_value params = json_get(args, "params");

        printf("- Parameter Name: "); 

        for(int i=0; i<json_len(params); i++){
            json_value name = json_get(args, "params", i, "name");
            if(json_is_null(name)) continue;

            printf("%s", strtok(json_get_string(name), "\""));

            if(i!=json_len(params)-1) printf(", ");
        } putchar('\n'); 

        printf("- Parameter Type: "); 
        for(int i=0; i<json_len(params); i++){
            json_value name = json_get(args, "params", i, "name");
            if(json_is_null(name)) continue;

            json_value param = json_get(args, "params", i, "type");
            json_value type = json_get(param, "type");
            json_value node = json_get(type, "_nodetype");
            
            if (strcmp(json_get_string(node), "TypeDecl")==0){   
                json_value type2 = json_get(type, "type");
                json_value var_type = json_get(type2, "names", 0);
                printf("%s", json_get_string(var_type));
            }else{
                json_value var_type = json_get(type, "names", 0);
                printf("%s", json_get_string(var_type));
            }

            json_value node2 = json_get(param, "_nodetype");
            
            if (strcmp(json_get_string(node2), "PtrDecl")==0) printf(" *");

            if(i!=json_len(params)-1) printf(", ");
        } 
    } else {
        printf("- Parameter Name: NULL\n");
        printf("- Parameter Type: NULL");
    }
    
    putchar('\n');
}

int recursive_count_if(json_value body) {
    int cnt_if = 0;

    json_value block_items = json_get(body, "block_items");    

    for(int i=0; i<json_len(block_items); i++) {
        json_value node = json_get(body, "block_items", i, "_nodetype");
        char* _nodetype = json_get_string(node);

        if (strcmp(_nodetype, "If")==0) {
            cnt_if++;

            json_value iftrue = json_get(body, "block_items", i, "iftrue");
            json_value iffalse = json_get(body, "block_items", i, "iffalse");

            if(json_is_null(iftrue) == 0) {
                // cnt_if += recursive_count_if(iftrue);
            }

            if(json_is_null(iffalse) == 0) {
                // cnt_if += recursive_count_if(iffalse);
            }

        }
    }
    return cnt_if;    
}

void count_if_statements(json_value body) {
    printf("- Number of IF Statement: %d\n", recursive_count_if(body));    
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
            parse_func_var(decl);
            count_if_statements(body);

            putchar('\n');
        }
    }

    printf("Total Number of Function: %d\n", func_cnt);

    json_free(data);

    return 0;
}