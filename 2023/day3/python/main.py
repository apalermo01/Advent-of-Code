import argparse
import re
from typing import List, Dict
import math


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


def has_two_numbers(line_below, this_line, line_above, index):
    """
    1 2 3
    4 * 5
    6 7 8

    need to count the distinct number of numbers adjacent to the *.

    TODO: expand this function to include information about where the digit is located in the string
    """

    # get the number of characters that aren't digits:
    def _num_non_digit(line): return sum(
        [1 if not e.isdigit() else 0 for e in line])

    num_adjacent_numbers = 0
    num_positions = []

    if index > 0 and this_line[index-1].isdigit():
        num_adjacent_numbers += 1
        num_positions.append([4, 'end'])

    if index+1 < len(this_line) and this_line[index+1].isdigit():
        num_adjacent_numbers += 1
        num_positions.append([5, 'end'])

    # same logic for above and below
    for line in [line_below, line_above]:

        # pick out the exact segment we're working with
        # normally, this will be 3 characters, but 2 if we're at the beginning
        # or end of the line
        if index == 0:
            seg = line[index:index+2]
        elif index == len(this_line) - 1:
            seg = line[index-1:index+1]
        else:
            seg = line[index-1:index+2]

        num_non_digit = _num_non_digit(seg)

        if num_non_digit == 0:
            num_adjacent_numbers += 1
        elif num_non_digit == 1 and len(seg) == 2:
            num_adjacent_numbers += 1
        elif num_non_digit == 1 and not seg[1].isdigit():
            num_adjacent_numbers += 2
        elif num_non_digit == 1:
            num_adjacent_numbers += 1
        elif num_non_digit == 2 and len(seg) == 3:
            num_adjacent_numbers += 1
    return num_adjacent_numbers == 2


def find_number_in_line(line, position, index):
    """For a given line and position, parse out the number.

    position=beginning: line[0] is the start of a number 
    position=end: line[-1] is the end of a number 
    position=middle, line[index] is part of the number but we don't know how
        far out it goes in either direction.
    """

    if position == 'beginning':
        number_str = ''
        i = 0
        while line[i].isdigit() and i < len(line):
            number_str += line[i]
            i += 1

    elif position == 'end':
        number_str = ''*5
        i = -1
        while line[i].isdigit() and math.abs(i) < len(line) and i >= -5:
            number_str[i] = line[i]
            i -= 1

    else:
        number_str = ''*10
        i = 5

        # search backwards
        while i > 0 and index-i >= 0 and line[index-i].isdigit():
            number_str[i] = line[index-i]
            i -= 1

        i = 5

        # search forwards
        while i < 10 and index + i <= len(line) and line[index+i].isdigit():
            number_str[i] = line[index+i]
            i += 1

    print("number str = ", number_str)


def find_gear_ratios(line_below, this_line, line_above, index):
    """this_line[index] is a *, two of the positions 1-8 have a number:

    1 2 3
    4 * 5
    6 7 8

    The goal is to figure out the complete number from these positions
    """


def get_gear_ratios(line_below, this_line, line_above):
    """Look for all instances of '*' in this_line.
       if there are 2 numbers adjacent to the *, then multiply them together"""

    # loop through all characters in this line and look for a '*'.
    # (sub function) if one is found, check if there are 2 numbers next to it
    # (sub function) use regex to parse out what those full numbers are
    gear_ratios = []
    print("="*80)
    print("abv: ", line_above)
    print("ths: ", this_line)
    print("bel: ", line_below)
    for i, char in enumerate(this_line):
        if char == '*' and \
                has_two_numbers(line_below, this_line, line_above, i):
            gear_ratios.append(
                find_gear_ratios(line_above, line_below, this_line, i))
    return 0


def problem_2(lines):
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
        part_nums.append(get_gear_ratios(line_below, this_line, line_above))

    return sum(part_nums)


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
