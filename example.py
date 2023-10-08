# 2020044557 배소정

import random

def initialize_board_6x6():
    row0 = [1, 2, 3, 4, 5, 6]
    random.shuffle(row0)
    row1 = row0[3:6] + row0[0:3]
    row2 = [row0[2], row0[0], row0[1], row0[5], row0[3], row0[4]]
    row3 = row2[3:6] + row2[0:3]
    row4 = [row0[1], row0[2], row0[0], row0[4], row0[5], row0[3]]
    row5 = row4[3:6] + row4[0:3]
    return [row0, row1, row2, row3, row4, row5]

def shuffle_ribbons_vertical(board):
    top = board[:3]
    bottom = board[3:6]
    random.shuffle(top)
    random.shuffle(bottom)
    return top + bottom

def shuffle_ribbons_horizontal(board):
    top = board[:2]
    mid = board[2:4] 
    bottom = board[4:6]
    random.shuffle(top)
    random.shuffle(mid)
    random.shuffle(bottom)
    return top + mid + bottom

def transpose(board):
    transposed = []
    size = len(board)
    for _ in range(size):
        transposed.append([])
    for row in board:
        for i in range(size):
            transposed[i].append(row[i])
    return transposed

def create_solution_board_6x6():
    board = initialize_board_6x6()
    board = shuffle_ribbons_horizontal(board)
    board = transpose(board)
    board = shuffle_ribbons_vertical(board)
    board = transpose(board)
    return board

def sudoku_mini():
    solution_board = create_solution_board_6x6()
    puzzle_board = copy_board(solution_board)
    no_of_holes = get_level()
    puzzle_board = make_holes(puzzle_board, no_of_holes)
    show_board(puzzle_board)
    while no_of_holes > 0:
        i = get_integer("가로줄번호(1~6): ", 1, 6) - 1
        j = get_integer("세로줄번호(1~6): ", 1, 6) - 1
        if puzzle_board[i][j] != 0:
            print("빈칸이 아닙니다.")
            continue
        n = get_integer("숫자(1~6): ", 1, 6)
        if n == solution_board[i][j]:
            puzzle_board[i][j] = solution_board[i][j]
            show_board(puzzle_board)
            no_of_holes -= 1
        else:
            print(n, "가(이) 아닙니다. 다시해보세요.")
    print("잘 하셨습니다. 또 들려주세요.")

def make_holes(board, no_of_holes):
    while no_of_holes > 0:
        i = random.randint(0, 5)
        j = random.randint(0, 5)
        if board[i][j] == 0:
            no_of_holes += 1
        board[i][j] = 0
        no_of_holes -= 1
    return board

def copy_board(board):
    board_clone = []
    for row in board:
        board_clone.append(row[:])
    return board_clone

def get_level():
    level = input("난이도(초급 1 중급 2 고급 3)를 숫자로 입력해주세요 : ")
    while level not in ('1', '2', '3'):
        level = input("난이도(초급 1 중급 2 고급 3)를 숫자로 입력해주세요 : ")
    if level == '1':
        return 6
    elif level == '2':
        return 8
    else:
        return 10

def show_board(board):
    print('S |', 1, 2, 3, 4, 5, 6)
    print('- | - - - - - -')
    i = 1
    for row in board:
        print(i, end=' | ')
        for entry in row:
            if entry == 0:
                print('.', end=' ')
            else:
                print(entry, end=' ')
        print()
        i += 1

def get_integer(message, i, j):
    number = input(message)
    while not (number.isdigit() and i <= int(number) <= j):
        number = input(message)
    return int(number)
