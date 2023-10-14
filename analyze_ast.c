#include <stdio.h>
#include "json_c.c"
#include <string.h>

#define MAX 1000000

void parse_func_name(json_value decl){
    char* func_name = json_get_string(json_get(decl, "name"));

    printf("- Name: %s\n", strtok(func_name, "\""));
}


void parse_ret_type(json_value decl) {
    json_value type = json_get(decl, "type");
    json_value node = json_get(type, "_nodetype");

    if(strcmp(json_get_string(node), "TypeDecl")!=0) type = json_get(type, "type");

    json_value type2 = json_get(type, "type");
    json_value rets = json_get(type2, "names");

    //"PtrDecl"

    printf("- Return Type: ");
    for(int i=0; i<json_len(rets); i++){
        if(i==2) json_print(type2);
        json_value ret = json_get(rets, i);
        printf("%s ", strtok(json_get_string(ret), "\""));
    }
    
    if(strcmp(json_get_string(node), "PtrDecl")==0) putchar('*');
    putchar('\n');
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

int countSubstring(char *str, char *substr) {
    int cnt = 0;
    int sub_len = strlen(substr);

    while (*str) {
        if (strncmp(str, substr, sub_len) == 0) {
            cnt++;
            str += sub_len - 1;
        }
        str++;
    }

    return cnt;
}

void count_if_statements(json_value body) {
    FILE* fp = fopen("./func_body.txt", "w");  
    json_fprint(fp, body);
    fclose(fp); 
    FILE* fp2 = fopen("./func_body.txt", "r"); 
    char body_str[MAX] = {0, }; 
    fread(body_str, 1, MAX, fp); 
    fclose(fp2); 

    int result = countSubstring(body_str, "\"_nodetype\": \"If\"");

    printf("- Number of IF Statement: %d\n", result);
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
            parse_ret_type(json_get(decl, "type"));
            parse_func_var(decl);
            count_if_statements(body);

            putchar('\n');
        }
    }

    printf("Total Number of Function: %d\n", func_cnt);

    json_free(data);

    return 0;
}
