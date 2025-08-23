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
    for r in 0usize..6 {
        for c in 0usize..6 {
            let i = r*6 + c;
            print!("{}", board[i]);
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
    for v in 0..1 {
        let dr = drs[v];
        let dc = dcs[v];
        // let dc = dcs[v];
        let mut num_existing = 0;
        let mut _r = r*dc;
        let mut _c = c*dr;
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
    for v in 0..1 {
        let dr = drs[v];
        let dc = dcs[v];
        let mut _r = r;
        let mut _c = 0;
        if dr == 1 { _r = 0; _c = c; }
        let mut streak = 1;
        let mut prev = 255u8;
        for _ in 0..6 {
            if inside(_r, _c) {
                let curr = at(board, _r, _c);
                if curr == 2 || curr != prev { streak = 1; }
                else { streak += 1; }
                if streak == 3 { return false; }
                prev = curr;
            }
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
        if _nr <= 0 || _nc <= 0 { continue; }
        let nr = _nr as usize;
        let nc = _nc as usize;
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
    for new in 0u8..2 {
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
    for constraint in constraints.iter() {
        let adjacency = adjacent(constraint.from, constraint.to);
        if adjacency == 0 { continue; }
        let adjacencies = [1i8, -1, 6, -6];
        let from_shifts = [3u8, 1, 0, 2];
        let to_shifts = [1u8, 3, 2, 0];
        let index = adjacencies.iter().position(|&x| x == adjacency).unwrap_or(0);
        if index == 0 { continue; }
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
    let mut i = 0usize;
    for c in input.chars() {
        output[i] = c.to_digit(10).unwrap() as u8;
        i += 1;
    }
}

fn identical(board1: &[u8; 36], board2: &[u8; 36]) -> bool {
    for i in 0usize..36 {
        if board1[i] != board2[i] {
            return false;
        }
    }
    true
}

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
        }
    ];

    for (index, testcase) in testcases.iter().enumerate() {
        let mut constraints = [0u8; 36];
        parse_constraints(&testcase.constraints, &mut constraints);
        let mut board = [0u8; 36];
        let mut solution = [0u8; 36];
        parse_board(&testcase.board, &mut board);
        parse_board(&testcase.solution, &mut solution);

        print!("Testcase {}\n\nInput:\n", index+1);
        print(&board);
        println!("\nExpected:");
        print(&solution);
        println!("");

        if !solve(&mut board, &constraints) {
            println!("Failure: could not solve");
        } else if identical(&board, &solution) {
            println!("Success");
        } else {
            println!("Failure. Got:");
            print(&board);
        }
        println!("");
    }
}
