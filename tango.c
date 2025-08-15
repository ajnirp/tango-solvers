// On Windows with MinGW: cc tango.c -std=c23 && ./a.exe

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BLANK '2'
#define NUM_TESTCASES 1

// Boards are stored as 37-character strings. The 37th character is '\0'.
// '0' represents a sun, '1' represents a moon, 2 represents a blank square.
// Constraints are stored as a 6x6 array. Each element is an 8-bit bitmask
// ABCDEFGH where A through D are the bits representing a "x" constraint
// North, East, South, West, and E through H are the bits representing a "="
// constraint North, East, South, West.

char at(char board[37], const int r, const int c) { return board[r*6 + c]; }
bool inside(const int r, const int c) {
    const int i = r*6 + c;
    return 0 <= i && i < 36;
}

// bool too_many()

bool can_set(char board[37], const int i, const int constraints[36], char new) {
    const int drs[2] = {0,1};
    const int dcs[2] = {1,0};

    // no more than three in a row or column
    const int r = i/6, c = i/6;
    for (int v = 0; v < 2; v++) {
        const int dr = drs[v], dc = dcs[v];
        int num_existing = 0;
        int _r = r*dr, _c = c*dc;
        for (int j = 0; j < 6; j++) {
            _r += dr;
            _c += dc;
            if (_r == r && _c == c) { continue; }
            if (at(board, _r, _c) == new) { num_existing++; }
        }
        if (num_existing == 3) { return false; }
    }

    // three consecutive identical not allowed
    for (int v = 0; v < 2; v++) {
        const int dr = drs[v], dc = dcs[v];
        int num_consecutive = 0;
        int _r = (dr == 0) ? r : r - 2;
        int _c = (dc == 0) ? c : c - 2;
        char prev = at(board, _r, _c);
        for (int j = 1; j < 5; j++) {
            char curr = at(board, _r+j*dr, _c+j*dc);
            if (prev == new) {
                if (curr == prev) {
                    num_consecutive++;
                } else {
                    num_consecutive = 0;
                }
            } else if (curr == new) {
                num_consecutive = 1;
            }
            prev = curr;
        }
        if (num_consecutive >= 3) { return false; }
    }

    // abide by constraints
    // north, east, south, west
    const int _drs[4] = {-1, 0, 1, 0};
    const int _dcs[4] = {0, 1, 0, -1};
    const int constraint = constraints[r*6+c];
    for (int j = 0; j < 8; j++) {
        int nr = r + _drs[j%4], nc = c + _dcs[j%4]; // neighbor
        if (!inside(nr, nc)) { continue; }
        int rule = (constraint & (1 << j)) >> j;
        if (rule == 0) { continue; }
        if (rule == 1) {
            const char nbr_val = at(board, nr, nc);
            // equality
            if (j < 4 && nbr_val != new) { return false; }
            else if (j >= 4 && nbr_val == new) { return false; }
        }
    }

    return true;
}

bool helper(char board[37], const int i, const int constraints[36]) {
    if (i == 37) {
        return true;
    }
    if (board[i] != '2') {
        return helper(board, i+1, constraints);
    }
    for (char new = '0'; new <= '1'; new++) {
        if (can_set(board, i, constraints, new)) {
            board[i] = new;
            if (helper(board, i+1, constraints)) {
                return true;
            }
            board[i] = BLANK;
        }
    }
    return false;
}

bool solve(char board[37], const int constraints[36]) {
    return helper(board, 0, constraints);
}

struct Constraint {
    int from;  // index
    int to;  // index
    bool equality;
};

struct Game {
    char board[37];
    struct Constraint constraints[4*36];
    const int num_constraints;
};

// 1: j is west of i -> i's shift is 3, j's is 1
// -1: j is east of i -> i's shift is 1, j's is 3
// 6: j is north of i -> i's shift is 0, j's is 2
// -6: j is south of i -> i's shift is 2, j's is 0
// any other number: not adjacent
int adjacent(const int i, const int j) {
    int result;
    if (i/6 == j/6 || i%6 == j%6) { result = i-j; }  // same row or col
    return
        (result == 1 || result == -1 || result == 6 || result == -6)
        ? result : 0;  // 0 if not adjacent
}
// 0: not col adjacent

int find(const int haystack[], const int n, const int needle) {
    for (int i = 0; i < n; i++) {
        if (haystack[i] == needle) {
            return i;
        }
    }
    return -1;
}

void parse_constraints(const struct Constraint constraints[], const int n, int result[36]) {
    for (int i = 0; i < n; i++) {
        const struct Constraint constraint = constraints[i];
        const int adjacency = adjacent(constraint.from, constraint.to);
        if (adjacency == 0) { continue; }
        const int adjacencies[4] = {1, -1, 6, -6};
        const int from_shifts[4] = {3, 1, 0, 2};
        const int to_shifts[4] = {1, 3, 2, 0};
        const int index = find(adjacencies, 4, adjacency);
        int from_shift = from_shifts[index], to_shift = to_shifts[index];
        if (!constraint.equality) {
            from_shift += 4;
            to_shift += 4;
        }
        result[constraint.from] |= (1 << from_shift);
        result[constraint.to] |= (1 << to_shift);
    }
}

int main(int argc, [[maybe_unused]] char* argv[argc+1]) {
    struct Game games[NUM_TESTCASES] = {
        {
            .board = "222222222222220022221022221122220022",
            .constraints = {
                { .from = 2, .to = 3, .equality = false, },
                { .from = 6, .to = 7, .equality = true, },
                { .from = 10, .to = 11, .equality = false, },
                { .from = 12, .to = 18, .equality = false, },
                { .from = 17, .to = 23, .equality = true, },
                { .from = 24, .to = 30, .equality = false, },
                { .from = 29, .to = 35, .equality = false, },
            },
            .num_constraints = 6,
        },
    };

    for (int i = 0; i < NUM_TESTCASES; i++) {
        struct Game game = games[i];

        int constraints[36];
        parse_constraints(game.constraints, game.num_constraints, constraints);
        char board[37];
        strncpy(board, game.board, 36);

        if (!solve(board, constraints)) {
            printf("Failure: could not solve\n");
        } else {
            printf("Solved: %s\n", board);
        }
    }

    return EXIT_SUCCESS;
}
