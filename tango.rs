// rustc -C opt-level=3 tango.rs; ./tango

// Boards are stored as arrays of 36 ints.
// 0 = sun, 1 = moon, 2 = blank square.
// Constraints are stored as an array of 36 ints. Each element is an 8-bit
// bitmask ABCDEFGH where A through D are the bits representing a "x" constraint
// North, East, South, West, and E through H are the bits representing a "="
// constraint North, East, South, West.

struct Constraint {
    from: u8,
    to: u8,
    eq: bool,
}

struct Testcase {
    board: &'static str,
    constraints: Vec<Constraint>,
    solution: &'static str,
}

fn print(board: &[u8; 36]) {
    for r in 0..6 {
        for c in 0..6 {
            print!("{}", board[r*6 + c]);
        }
        println!("");
    }
}

fn at(board: &[u8; 36], r: usize, c: usize) -> u8 { board[r*6 + c] }
fn inside(r: usize, c: usize) -> bool { r*6 + c < 36 }

fn can_set(board: &[u8; 36], i: usize, constraints: &[u8; 36], new: u8) -> bool {
    let drs = [0usize, 1];
    let dcs = [1usize, 0];
    let r: usize = i/6;
    let c: usize = i%6;

    // no more than three in a row or column
    for v in 0..2 {
        let (dr, dc) = (drs[v], dcs[v]);
        let mut num_existing = 0;
        let (mut _r, mut _c) = (r*dc, c*dr);
        for _ in 0..6 {
            let curr = at(board, _r, _c);
            if !(_r == r && _c == c) && curr == new {
                num_existing += 1;
            }
            _r += dr;
            _c += dc;
        }
        if num_existing == 3 { return false; }
    }

    // three consecutive identical not allowed
    for v in 0..2 {
        let (dr, dc) = (drs[v], dcs[v]);
        let (mut _r, mut _c) = if dr == 0 { (r, 0) } else { (0, c)};
        let mut streak = 1;
        let mut prev = 255u8;
        for _ in 0..6 {
            let curr = at(board, _r, _c);
            streak = if curr != 2 && curr == prev { streak + 1 } else { 1 };
            if streak == 3 { return false; }
            prev = curr;
            _r += dr;
            _c += dc;
        }
    }

    // abide by constraints
    // north, east, south, west
    let _drs = [-1i16, 0, 1, 0];
    let _dcs = [0i16, 1, 0, -1];
    let constraint = at(constraints, r, c);
    for j in 0..8 {
        let _nr = (r as i16) + _drs[j%4];
        let _nc = (c as i16) + _dcs[j%4];
        if _nr < 0 || _nc < 0 { continue; }
        let (nr, nc) = (_nr as usize, _nc as usize);
        if !inside(nr, nc) { continue; }
        let rule = constraint & (1 << j);
        if rule == 0 { continue; }
        let nbr_val = at(board, nr, nc);
        if nbr_val == 2 { continue; }
        if j < 4 && nbr_val != new { return false; }  // equality
        if j >= 4 && nbr_val == new { return false; }  // inequality
    }

    true
}

fn helper(board: &mut [u8; 36], i: usize, constraints: &[u8; 36]) -> bool {
    if i == 36 {
        return true;
    }
    if board[i] != 2 {
        return helper(board, i+1, constraints);
    }
    for new in 0..2 {
        if can_set(board, i, constraints, new) {
            board[i] = new;
            if helper(board, i+1, constraints) {
                return true;
            }
            board[i] = 2;
        }
    }
    false
}

fn solve(board: &mut [u8; 36], constraints: &[u8; 36]) -> bool {
    helper(board, 0, constraints)
}

// 1: j is west of i -> i's shift is 3, j's is 1
// -1: j is east of i -> i's shift is 1, j's is 3
// 6: j is north of i -> i's shift is 0, j's is 2
// -6: j is south of i -> i's shift is 2, j's is 0
// any other number: not adjacent
fn adjacent(i: u8, j: u8) -> i8 {
    let mut result = 0i8;
    if i/6 == j/6 || i%6 == j%6 { result = (i as i8) - (j as i8); }  // same row or col
    if result == 1 || result == -1 || result == 6 || result == -6 {
        result
    } else {
        0  // not adjacent
    }
}

fn parse_constraints(constraints: &Vec<Constraint>, result: &mut[u8; 36]) {
    let adjacencies = [1i8, -1, 6, -6];
    let from_shifts = [3u8, 1, 0, 2];
    let to_shifts = [1u8, 3, 2, 0];
    for constraint in constraints.iter() {
        let adjacency = adjacent(constraint.from, constraint.to);
        if adjacency == 0 { continue; }
        let index = adjacencies.iter().position(|&x| x == adjacency).unwrap_or(255);
        if index == 255 { continue; }
        let mut from_shift = from_shifts[index as usize];
        let mut to_shift = to_shifts[index as usize];
        if !constraint.eq {
            from_shift += 4;
            to_shift += 4;
        }
        result[constraint.from as usize] |= 1 << from_shift;
        result[constraint.to as usize] |= 1 << to_shift;
    }
}

fn parse_board(input: &str, output: &mut [u8; 36]) {
    for (i, c) in input.chars().enumerate() {
        output[i] = c.to_digit(10).unwrap() as u8;
    }
}

// TODO: these should be tests not a main function
fn main() {
    let testcases = vec![
        Testcase {
            board: "222222222222220022221022221122220022",
            constraints: vec![
                Constraint { from: 2, to: 3, eq: false, },
                Constraint { from: 6, to: 7, eq: true, },
                Constraint { from: 10, to: 11, eq: false, },
                Constraint { from: 12, to: 18, eq: false, },
                Constraint { from: 17, to: 23, eq: true, },
                Constraint { from: 24, to: 30, eq: false, },
                Constraint { from: 29, to: 35, eq: false, },
            ],
            solution: "100101001101110010011010001101110010",
        },
        Testcase {
            board: "222222220022202212212202221122222222",
            constraints: vec![
                Constraint { from: 0, to: 1, eq: true, },
                Constraint { from: 4, to: 5, eq: false, },
                Constraint { from: 0, to: 6, eq: true, },
                Constraint { from: 5, to: 11, eq: true, },
                Constraint { from: 24, to: 30, eq: false, },
                Constraint { from: 30, to: 31, eq: true, },
                Constraint { from: 29, to: 35, eq: false, },
                Constraint { from: 34, to: 35, eq: false, },
            ],
            solution: "001101010011101010110100001101110010",
        },
        Testcase {
            board: "212222102222222222222222222210222212",
            constraints: vec![
                Constraint { from: 3, to: 9, eq: false, },
                Constraint { from: 14, to: 15, eq: false, },
                Constraint { from: 16, to: 17, eq: false, },
                Constraint { from: 18, to: 19, eq: false, },
                Constraint { from: 20, to: 21, eq: true, },
                Constraint { from: 26, to: 32, eq: true, },
            ],
            solution: "011001100110011001101100100110010011",
        },
        Testcase {
            board: "222222222222222202222120222020222212",
            constraints: vec![
                Constraint { from: 0, to: 1, eq: true, },
                Constraint { from: 2, to: 8, eq: true, },
                Constraint { from: 6, to: 12, eq: true, },
                Constraint { from: 14, to: 20, eq: false, },
                Constraint { from: 18, to: 19, eq: true, },
            ],
            solution: "110100010011001101110100101010001011",
        },
    ];

    for (index, testcase) in testcases.iter().enumerate() {
        let (mut board, mut solution, mut constraints) = (
            [0u8; 36], [0u8; 36], [0u8; 36],
        );
        parse_board(&testcase.board, &mut board);
        parse_board(&testcase.solution, &mut solution);
        parse_constraints(&testcase.constraints, &mut constraints);

        print!("Testcase {}: ", index+1);

        if !solve(&mut board, &constraints) {
            println!("Failure: could not solve");
        } else if board == solution {
            println!("Success");
        } else {
            println!("Failure. Expected:");
            print(&solution);
            println!("Got:");
            print(&board);
        }
        println!("...");
    }
}
