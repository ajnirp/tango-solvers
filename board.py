from collections import defaultdict
from constants import *

class Board:
    # cells = 2D square array
    def __init__(self, cells, constraints):
        self.cells = cells
        self.constraints = self.parse_constraints(constraints)

    def num_empty_cells(self):
        return sum(cell == 2 for row in self.cells for cell in row)

    def inside(self, r, c): return 0 <= r < SIDE and 0 <= c < SIDE
    def at(self, r, c): return self.cells[r][c]  # does not check bounds
    def set(self, r, c, val):
        if self.cells[r][c] == 2:
            self.cells[r][c] = val # does not check bounds
    def other(self, r, c):  # does not check bounds
        return 1 - self.cells[r][c]

    # assumes constraints are valid
    def parse_constraints(self, constraints):
        result = defaultdict(set)  # map of cell index to a set of (neighbor, constraint_type)
        for cell1, cell2, k in constraints:
            result[cell1].add((*cell2, k))
            result[cell2].add((*cell1, k))
        return result

    def print(self):
        for row in range(SIDE):
            for col in range(SIDE):
                print(self.at(row, col), end=" ")
            print()

    def fill_empty_row(self, row, val):
        for col in range(SIDE):
            self.set(row, col, val)

    def fill_empty_col(self, col, val):
        for row in range(SIDE):
            self.set(row, col, val)