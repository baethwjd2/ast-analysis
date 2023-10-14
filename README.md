# AST 분석 프로젝트

### Generate AST
- SAMPLE 1: `python3 generate_ast.py ./sample/sample1.c > ./sample/sample1.json` 
- SAMPLE 2: `python3 generate_ast.py ./sample/sample2.c > ./sample/sample2.json` 

### Compile
`gcc analyze_ast.c -o analyze_ast -w`

### Run
- TEST 1: `./analyze_ast ./sample/sample1.json` 
- TEST 2: `./analyze_ast ./sample/sample2.json` 

### Output
[](!result.png)
