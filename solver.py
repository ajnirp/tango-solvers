'''
LinkedIn Tango solver. Uses DFS. Quite unoptimized, favoring readability via a
minimal implementation.
'''

from board import Board
from constants import *

class Solver():
    # Returns a boolean indicating whether it filled at least one cell.
    def next_to_constraint(self, board):
        result = False
        for row in range(SIDE):
            for col in range(SIDE):
                if board.at(row, col) == 2: continue
                for constraint in board.constraints[(row, col)]:
                    nr, nc, k = constraint
                    val = board.at(row, col) if k == "=" else board.other(row, col)
                    board.set(nr, nc, val)
                    result = True
        return result

    # Update |board| by performing a set of deductions.
    # Returns True if any of the deductions made progress.
    def step(self, board):
        return self.next_to_constraint(board)
    
    def dfs_solve(self, board):
        if not board.is_valid(): return None
        if board.is_all_filled_in(): return board
        VALS = [0, 1]
        for row in range(SIDE):
            for col in range(SIDE):
                if board.at(row, col) != 2:
                    continue  # already set
                for val in VALS:
                    copy = board.copy_and_set(row, col, val)
                    if not copy.is_valid(): continue
                    self.deduce_until_no_longer_possible(board=copy)
                    recur = self.dfs_solve(copy)
                    if recur is not None:
                        return recur
                    
    # Destructively updates |board|.
    def deduce_until_no_longer_possible(self, board):
        for _ in range(board.num_empty_cells):
            # Return early if no further progress is possible
            if not self.step(board):
                return

    def solve(self, board):
        # Run heuristics until the board is solved.
        # How many iterations do we need? Define an epoch as a run of every heuristic in sequence.
        # We expect that in each epoch at least one new cell will be filled with certainty.
        # So the upper bound on the number of epochs is the number of empty cells.
        # Returns True if it was possible to solve the board.
        num_epochs = board.num_empty_cells
        for _ in range(num_epochs):
            if not self.step(board):
                return False
        return True
