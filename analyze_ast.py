import sys
import json


# 함수 개수 추출 (희진)
def parse_func_cnt(data):
    func_cnt = 0
    
    for i in data["ext"]:
        if i["_nodetype"]=="FuncDef":
            func_cnt+=1
    return func_cnt


# 함수 리턴 타입 추출 (하영)
def parse_func_ret(data):
    func_ret = list()
    
    # write your code
    
    return func_ret


# 함수 이름 추출 (소정)
def parse_func_name(data):
    func_name = list()

    for i in data["ext"]:
        if i["_nodetype"]=="FuncDef":
            func_name.append(i['decl']['name'])
    
    return func_name


# 파라미터 타입과 변수명 추출 (다인)
def parse_param(data):
    param_type = list()
    param_name = list()
    
    # write your code
    
    return param_type, param_name


# if 조건 개수 추출 (송현)
def parse_if_cnt(data):
    if_cnt = 0
    
    # write your code
    
    return if_cnt


def print_analysis(func_cnt, func_ret, func_name, param_type, param_name, if_cnt):

    for i in range(func_cnt):
        print("Function", i+1)
        print("- Name:", func_name[i])
        print("- Return Type:")
        print("- Parameter Type:")
        print("- Parameter Name:")
        print("- Number of IF Statement:")
        
        
        if i!=func_cnt-1:
            print("")


def main(argv):
    path = argv[1]
    
    with open(path, 'r', encoding='utf-8') as json_file:
        data = json.load(json_file)
    
    func_cnt = parse_func_cnt(data)
    func_ret = parse_func_ret(data)
    func_name = parse_func_name(data)
    param_type, param_name = parse_param(data)
    if_cnt = parse_if_cnt(data)
    
    print_analysis(func_cnt, func_ret, func_name, param_type, param_name, if_cnt)


if __name__=="__main__":
    argv = sys.argv
    main(argv)