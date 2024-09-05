import argparse
import re
from typing import List


def parse_line_1(line):
    pass


def parse_line_2(line):
    pass


def generate_matches(line):
    """Gets all the numbers out of a line"""

    matches = []
    s = True
    index = 0

    while s is not None:

        s = re.search(".?\d+.?", line)
        if s is not None:
            this_match = s.group(0)
            matches.append(
                {'match': this_match,
                 'value': int(re.findall("\d+", this_match)[0]),
                 'start_idx': s.start() + index,
                 'end_idx': s.end() + index - 1,
                 })
            old_line = line

            if this_match[-1].isnumeric():
                forward = len(this_match)
            else:
                forward = len(this_match)-1

            index = line.index(this_match) + forward

            line = line[index:]
            if line == old_line:
                raise ValueError("bad iteration!")
    print("matches = ", matches)
    return matches


def get_part_nums(line_below, this_line, line_above):

    matches = generate_matches(this_line)
    return 0


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


# def problem_1(lines):
#
#     LEN_LINES = len(lines)
#
#     part_nums = []
#     count = min(len(lines), 5)
#     for row_idx in range(len(lines)):
#         line = lines[row_idx]
#         match_strings = generate_matches(line)
#         nums_this_line = []
#         for match in match_strings:
#             surrounding_chars = match
#             index_start = line.index(match)
#
#             if row_idx > 0:
#                 line_above = lines[row_idx -
#                                    1][index_start:index_start+len(match)]
#                 surrounding_chars += line_above
#
#             if row_idx < LEN_LINES-1:
#                 line_below = lines[row_idx +
#                                    1][index_start:index_start+len(match)]
#                 surrounding_chars += line_below
#
#             mask = [not (c.isnumeric() or c == '.' or c == '\n')
#                     for c in surrounding_chars]
#             if any(mask):
#                 nums_this_line.append(int(re.findall("\d+", match)[0]))
#         part_nums += nums_this_line
#     return sum(part_nums)


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
