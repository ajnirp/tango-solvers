// On Windows with MinGW: cc tango.c -std=c23 && ./a.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_TESTCASES 4

// Boards are stored as arrays of 36 ints.
// 0 = sun, 1 = moon, 2 = blank square.
// Constraints are stored as an array of 36 ints. Each element is an 8-bit
// bitmask ABCDEFGH where A through D are the bits representing a "x" constraint
// North, East, South, West, and E through H are the bits representing a "="
// constraint North, East, South, West.

struct Constraint {
    int from;  // index
    int to;  // index
    bool equality;
};

struct Testcase {
    char board[37];
    struct Constraint constraints[4*36];
    const int num_constraints;
    char solution[37];
};

void print(const int board[36]) {
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 6; c++) {
            const int i = r*6 + c;
            printf("%d", board[i]);
        }
        printf("\n");
    }
}

int at(const int board[36], const int r, const int c) { return board[r*6 + c]; }
bool inside(const int r, const int c) {
    const int i = r*6 + c;
    return 0 <= i && i < 36;
}

bool can_set(const int board[36], const int i, const int constraints[36], const int new) {
    const int drs[2] = {0,1};
    const int dcs[2] = {1,0};
    const int r = i/6, c = i%6;

    // no more than three in a row or column
    for (int v = 0; v < 2; v++) {
        const int dr = drs[v], dc = dcs[v];
        int num_existing = 0;
        int _r = r*dc, _c = c*dr;
        for (int j = 0; j < 6; j++, _r += dr, _c += dc) {
            int curr = at(board, _r, _c);
            if (_r == r && _c == c) { continue; }
            if (at(board, _r, _c) == new) { num_existing++; }
        }
        if (num_existing == 3) { return false; }
    }

    // three consecutive identical not allowed
    for (int v = 0; v < 2; v++) {
        const int dr = drs[v], dc = dcs[v];
        int _r = r, _c = 0;
        if (dr == 1) { _r = 0; _c = c; }
        int streak = 1, prev = -1;
        for (int j = 0; j < 6; j++, _r += dr, _c += dc) {
            if (!inside(_r, _c)) { continue; }
            int curr = at(board, _r, _c);
            if (curr == 2 || curr != prev) { streak = 1; }
            else { streak++; }
            if (streak == 3) { return false; }
            prev = curr;
        }
    }

    // abide by constraints
    // north, east, south, west
    const int _drs[4] = {-1, 0, 1, 0};
    const int _dcs[4] = {0, 1, 0, -1};
    const int constraint = at(constraints, r, c);
    for (int j = 0; j < 8; j++) {
        const int nr = r + _drs[j%4], nc = c + _dcs[j%4];  // neighbor
        if (!inside(nr, nc)) { continue; }
        const int rule = constraint & (1 << j);
        if (rule == 0) { continue; }
        const int nbr_val = at(board, nr, nc);
        if (nbr_val == 2) { continue; }
        if (j < 4 && nbr_val != new) { return false; }  // equality
        if (j >= 4 && nbr_val == new) { return false; }  // inequality
    }

    return true;
}

bool helper(int board[36], const int i, const int constraints[36]) {
    if (i == 36) {
        return true;
    }
    if (board[i] != 2) {
        return helper(board, i+1, constraints);
    }
    for (int new = 0; new <= 1; new++) {
        if (can_set(board, i, constraints, new)) {
            board[i] = new;
            if (helper(board, i+1, constraints)) {
                return true;
            }
            board[i] = 2;
        }
    }
    return false;
}

bool solve(int board[36], const int constraints[36]) {
    return helper(board, 0, constraints);
}

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

void parse_board(const char input[37], int output[36]) {
    for (int i = 0; i < 36; i++) {
        output[i] = input[i] - '0';
    }
}

bool identical(const int board1[36], const int board2[36]) {
    for (int i = 0; i < 36; i++) {
        if (board1[i] != board2[i]) {
            return false;
        }
    }
    return true;
}

int main(int argc, [[maybe_unused]] char* argv[argc+1]) {
    struct Testcase testcases[NUM_TESTCASES] = {
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
            .solution = "100101001101110010011010001101110010",
        },
        {
            .board = "222222220022202212212202221122222222",
            .constraints = {
                { .from = 0, .to = 1, .equality = true, },
                { .from = 4, .to = 5, .equality = false, },
                { .from = 0, .to = 6, .equality = true, },
                { .from = 5, .to = 11, .equality = true, },
                { .from = 24, .to = 30, .equality = false, },
                { .from = 30, .to = 31, .equality = true, },
                { .from = 29, .to = 35, .equality = false, },
                { .from = 34, .to = 35, .equality = false, },
            },
            .num_constraints = 8,
            .solution = "001101010011101010110100001101110010",
        },
        {
            .board = "212222102222222222222222222210222212",
            .constraints = {
                { .from = 3, .to = 9, .equality = false, },
                { .from = 14, .to = 15, .equality = false, },
                { .from = 16, .to = 17, .equality = false, },
                { .from = 18, .to = 19, .equality = false, },
                { .from = 20, .to = 21, .equality = true, },
                { .from = 26, .to = 32, .equality = true, },
            },
            .num_constraints = 6,
            .solution = "011001100110011001101100100110010011",
        },
        {
            .board = "222222222222222202222120222020222212",
            .constraints = {
                { .from = 0, .to = 1, .equality = true, },
                { .from = 2, .to = 8, .equality = true, },
                { .from = 6, .to = 12, .equality = true, },
                { .from = 14, .to = 20, .equality = false, },
                { .from = 18, .to = 19, .equality = true, },
            },
            .num_constraints = 5,
            .solution = "110100010011001101110100101010001011",
        },
    };

    for (int i = 0; i < NUM_TESTCASES; i++) {
        const struct Testcase* testcase = &testcases[i];

        int constraints[36] = {0};
        parse_constraints(testcase->constraints, testcase->num_constraints, constraints);
        int board[36], solution[36];
        parse_board(testcase->board, board);
        parse_board(testcase->solution, solution);

        printf("Testcase %d:\n\nInput:\n", i+1);
        print(board);
        printf("\nExpected:\n");
        print(solution);
        printf("\n");

        if (!solve(board, constraints)) {
            printf("Failure: could not solve\n");
        } else if (identical(board, solution)) {
            printf("Success\n");
        } else {
            printf("Failure. Got:\n");
            print(board);
        }
        printf("...\n");
    }
    return EXIT_SUCCESS;
}
