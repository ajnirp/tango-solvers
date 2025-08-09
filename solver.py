'''
LinkedIn Tango solver. Uses a bag of heuristics. Fairly unoptimized.

Heuristics used:

    Three of one type in the same line
    Two identical adjacent
    Two identical opposite ends
    Two identical two-apart
    One next to =
    One next to x
    One one-apart from =
    Count x's to determine what the final cell should be
    Count ='s to determine what the final cell should be
    = at the far end from a single filled val

More to add: identical at opposite ends
there are two of a kind and somewhere else in the line is an =
'''

from board import Board
from constants import *

class Solver():
    def __init__(self, board):
        self.board = board

    def at(self, *args, **kwargs): return self.board.at(*args, **kwargs)
    def inside(self, *args, **kwargs): return self.board.inside(*args, **kwargs)
    def other(self, *args, **kwargs): return self.board.other(*args, **kwargs)
    def set(self, *args, **kwargs): return self.board.set(*args, **kwargs)
    def fill_empty_row(self, *args, **kwargs): return self.board.fill_empty_row(*args, **kwargs)
    def fill_empty_col(self, *args, **kwargs): return self.board.fill_empty_col(*args, **kwargs)

    ### BEGIN: heuristics
    # Every heuristic returns a boolean indicating whether it filled at least
    # one cell.

    def three_identical(self):
        result = False
        for row in range(SIDE):
            num_suns = num_moons = 0
            for col in range(SIDE):
                empty_cells = []
                match self.at(row, col):
                    case 0: num_suns += 1
                    case 1: num_moons += 1
                    case 2: empty_cells.append((row, col))
            if num_suns == 3:
                self.fill_empty_row(row=row, val=1)
                result = True
            elif num_moons == 3:
                self.fill_empty_row(row=row, val=0)
                result = True
        for col in range(SIDE):
            num_suns = num_moons = 0
            for row in range(SIDE):
                empty_cells = []
                match self.at(row, col):
                    case 0: num_suns += 1
                    case 1: num_moons += 1
                    case 2: empty_cells.append((row, col))
            if num_suns == 3:
                self.fill_empty_col(col=col, val=1)
                result = True
            elif num_moons == 3:
                self.fill_empty_col(col=col, val=0)
                result = True
        return result

    def identical_adjacent(self):
        result = False
        for row in range(SIDE):
            for col in range(SIDE-1):
                val = self.at(row, col)
                if val == 2: continue
                if self.at(row, col+1) == val:
                    if self.inside(row, col+2):
                        self.set(row, col+2, 1-val)
                        result = True
                    if self.inside(row, col-1):
                        self.set(row, col-1, 1-val)
                        result = True
        for col in range(SIDE):
            for row in range(SIDE-1):
                val = self.at(row, col)
                if val == 2: continue
                if self.at(row+1, col) == val:
                    if self.inside(row+2, col):
                        self.set(row+2, col, 1-val)
                        result = True
                    if self.inside(row-1, col):
                        self.set(row-1, col, 1-val)
                        result = True
        return result
                        
    def two_identical_at_an_end(self):
        result = False
        for row in range(SIDE):
            if self.at(row, 1) == self.at(row, 0) != 2:
                self.set(row, SIDE-1, self.other(row, 0))
                result = True
            if self.at(row, SIDE-2) == self.at(row, SIDE-1) != 2:
                self.set(row, 0, self.other(row, SIDE-1))
                result = True
        for col in range(SIDE):
            if self.at(1, col) == self.at(0, col) != 2:
                self.set(SIDE-1, col, self.other(0, col))
                result = True
            if self.at(SIDE-2, col) == self.at(SIDE-1, col) != 2:
                self.set(0, col, self.other(SIDE-1, col))
                result = True
        return result

    def identical_opposite_ends(self):
        result = False
        for row in range(SIDE):
            if self.at(row, 0) == self.at(row, SIDE-1) != 2:
                self.set(row, 1, self.other(row, 0))
                self.set(row, SIDE-2, self.other(row, 0))
                result = True
        for col in range(SIDE):
            if self.at(0, col) == self.at(SIDE-1, col) != 2:
                self.set(1, col, self.other(0, col))
                self.set(SIDE-2, col, self.other(0, col))
                result = True
        return result

    def identical_two_apart(self):
        result = False
        for dv in DVS:
            dr, dc = dv
            for row in range(SIDE):
                for col in range(SIDE):
                    one_over = (row + dr, col + dc)
                    two_over = (row + 2*dr, col + 2*dc)
                    if not self.inside(*one_over) or not self.inside(*two_over):
                        continue
                    if self.at(row, col) == self.at(*two_over) != 2:
                        self.set(*one_over, self.other(row, col))
                        result = True
        return result

    def next_to_constraint(self):
        result = False
        for row in range(SIDE):
            for col in range(SIDE):
                if self.at(row, col) == 2: continue
                for constraint in self.board.constraints[(row, col)]:
                    nr, nc, k = constraint
                    val = self.at(row, col) if k == "=" else self.other(row, col)
                    self.set(nr, nc, val)
                    result = True
        return result

    def one_over_from_equals(self):
        result = False
        for dv in DVS:
            dr, dc = dv
            for row in range(SIDE):
                for col in range(SIDE):
                    if self.at(row, col) == 2: continue
                    one_over = (row + dr, col + dc)
                    two_over = (row + 2*dr, col + 2*dc)
                    if not self.inside(*one_over) or not self.inside(*two_over):
                        continue
                    if (*two_over, "=") in self.board.constraints[one_over]:
                        self.set(*one_over, self.other(row, col))
                        self.set(*two_over, self.other(row, col))
                        result = True
        return result

    # TODO: replace identical adjacent, two identical at an end, identical opposite ends
    # with a heuristic that looks at each empty cell in the row or col and asks
    # if this were a sun (or a moon), would it violate the property of "no three in a row"
    # or "more than three in a row"

    ### END: heuristics

    # Returns True if any progress was made in the step.
    def step(self):
        return any(
            [
                self.three_identical(),
                self.identical_adjacent(),
                self.two_identical_at_an_end(),
                self.identical_opposite_ends(),
                self.identical_two_apart(),
                self.next_to_constraint(),
                self.one_over_from_equals(),
            ]
        )

    def solve(self):
        # Run heuristics until the board is solved.
        # How many iterations do we need? Define an epoch as a run of every heuristic in sequence.
        # We expect that in each epoch at least one new cell will be filled with certainty.
        # So the upper bound on the number of epochs is the number of empty cells.
        num_epochs = self.board.num_empty_cells()
        for _ in range(num_epochs):
            if not self.step():
                print("Can't make progress!")
                break
