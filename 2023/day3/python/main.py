import argparse
import re
from typing import List, Dict


def parse_line_1(line):
    pass


def parse_line_2(line):
    pass


def generate_matches(line):
    """Gets all the numbers out of a line"""

    matches = []
    s = True
    curr_idx = 0
    orig_line = line
    while s is not None:

        s = re.search(".?\d+.?", line)
        if s is not None:
            this_match = s.group(0)
            matches.append(
                {'match': this_match,
                 'value': int(re.findall("\d+", this_match)[0]),
                 'start_idx': s.start() + curr_idx,
                 'end_idx': s.end() + curr_idx,
                 })

            # now move the line forward past the found value
            curr_idx += s.end()
            if orig_line[curr_idx].isdigit():
                curr_idx -= 1
            line = orig_line[curr_idx:]
    return matches


def symbol_nearby(match: Dict,
                  line_below: str,
                  this_line: str,
                  line_above: str):
    line_below_chunk = line_below[
        match['start_idx']:match['end_idx']
    ]
    this_line_chunk = this_line[
        match['start_idx']:match['end_idx']
    ]
    line_above_chunk = line_above[
        match['start_idx']:match['end_idx']
    ]
    for line in [line_below_chunk, this_line_chunk, line_above_chunk]:
        for char in range(len(line)):
            if (not line[char].isdigit() and line[char] != '.'):
                return True

    return False


def get_part_nums(line_below, this_line, line_above):
    part_nums = []

    # grab matches
    matches = generate_matches(this_line)

    for match in matches:
        if (symbol_nearby(match, line_below, this_line, line_above)):
            part_nums.append(match['value'])

    return sum(part_nums)


def problem_1(lines: List[str]) -> int:

    part_nums = []

    for row_idx in range(len(lines)):
        # grab this line and the one above / below
        this_line = lines[row_idx]
        if row_idx == 0:
            line_above = '.'*len(this_line)
        else:
            line_above = lines[row_idx-1]

        if row_idx == len(lines) - 1:
            line_below = '.'*len(this_line)
        else:
            line_below = lines[row_idx+1]

        # now get all the part numbers from this line
        part_nums.append(get_part_nums(line_below, this_line, line_above))

    return sum(part_nums)


def problem_2(lines):
    pass


def main():
    args = parse_args()
    path = args.path
    with open(path, "r") as f:
        lines = f.readlines()

    if args.method == '1':
        answer = problem_1(lines)
        print("answer is: ", answer)

    elif args.method == '2':
        answer = problem_2(lines)
        print("the second answer is: ", answer)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--path')
    parser.add_argument('--method', choices=['1', '2'],
                        default=1)
    args = parser.parse_args()
    return args


if __name__ == '__main__':
    main()
