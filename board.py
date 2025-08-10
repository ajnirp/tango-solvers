from collections import defaultdict
from constants import *
from copy import deepcopy

class Board:
    # cells = 2D square array
    def __init__(self, grid, constraints):
        self.grid = grid
        self.constraint_list = constraints
        self.constraints = self.parse_constraints(constraints)
        self.num_empty_cells = sum(cell == 2 for row in self.grid for cell in row)

    def is_all_filled_in(self): return self.num_empty_cells == 0
    def is_solved(self): return self.is_all_filled_in() and self.is_valid()

    # Returns True if all these conditions hold:
    # - the board has no triples
    # - each row and column has <= 3 suns and <= 3 moons.
    # - all constraints are met
    def is_valid(self):
        for r in range(SIDE):
            suns_valid = sum(self.at(r, c) == 0 for c in range(SIDE)) <= SIDE//2
            moons_valid = sum(self.at(r, c) == 1 for c in range(SIDE)) <= SIDE//2
            triples_exist = any(self.at(r, c) == self.at(r, c+1) == self.at(r, c+2) != 2 for c in range(SIDE-2))
            if (not suns_valid) or (not moons_valid) or triples_exist:
                return False
        for c in range(SIDE):
            suns_valid = sum(self.at(r, c) == 0 for r in range(SIDE)) <= SIDE//2
            moons_valid = sum(self.at(r, c) == 1 for r in range(SIDE)) <= SIDE//2
            triples_exist = any(self.at(r, c) == self.at(r+1, c) == self.at(r+2, c) != 2 for r in range(SIDE-2))
            if (not suns_valid) or (not moons_valid) or triples_exist:
                return False
        for cell1, cell2, operator in self.constraint_list:
            if operator == "=" and self.at(*cell1) != self.at(*cell2):
                return False
            elif operator == "*" and self.at(*cell1) == self.at(*cell2):
                return False
        return True

    def copy_and_set(self, r, c, val):
        copy = deepcopy(self)
        copy.set(r, c, val)
        return copy

    def is_inside(self, r, c): return 0 <= r < SIDE and 0 <= c < SIDE
    def at(self, r, c): return self.grid[r][c]  # does not check bounds
    def set(self, r, c, val):  # does not check that val != 2
        if self.is_inside(r, c) and self.grid[r][c] == 2:
            self.grid[r][c] = val
            self.num_empty_cells -= 1
    # Does not check bounds. Does not check that self.at(r, c) is not 2
    def other(self, r, c):
        return 1 - self.grid[r][c]

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