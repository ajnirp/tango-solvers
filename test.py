from board import Board
from solver import Solver

testcases = [
    (
        [
            [2,2,2,2,2,2],
            [2,2,2,2,2,2],
            [2,2,0,0,2,2],
            [2,2,1,0,2,2],
            [2,2,1,1,2,2],
            [2,2,0,0,2,2],
        ],
        [
            ((0, 2), (0, 3), "x"),
            ((1, 0), (1, 1), "="),
            ((1, 4), (1, 5), "x"),
            ((2, 0), (3, 0), "x"),
            ((2, 5), (3, 5), "="),
            ((4, 0), (5, 0), "x"),
            ((4, 5), (5, 5), "x"),
        ],
    ),
    (
        [
            [2,2,2,2,2,2],
            [2,2,0,0,2,2],
            [2,0,2,2,1,2],
            [2,1,2,2,0,2],
            [2,2,1,1,2,2],
            [2,2,2,2,2,2],
        ],
        [
            ((0, 0), (0, 1), "="),
            ((0, 0), (1, 0), "="),
            ((0, 5), (0, 4), "x"),
            ((0, 5), (1, 5), "="),
            ((5, 5), (5, 4), "x"),
            ((5, 5), (4, 5), "x"),
            ((5, 0), (4, 0), "x"),
            ((5, 0), (5, 1), "="),
        ],
    ),
]

for idx, testcase in enumerate(testcases):
    cells, constraints = testcase
    board = Board(cells=cells, constraints=constraints)
    solver = Solver(board=board)
    solver.solve()
    print(f"Testcase {idx+1}")
    board.print()
    print()
