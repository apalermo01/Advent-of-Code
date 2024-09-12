import argparse
import re
from typing import List, Dict
import logging

logging.basicConfig(level=logging.WARNING)
logger = logging.getLogger(__name__)


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


def find_number_in_line(line, position, index=None):
    """For a given line and position, parse out the number.

    position=start: line[0] is the start of a number
    position=end: line[-1] is the end of a number
    position=middle, line[index] is part of the number but we don't know how
        far out it goes in either direction.
    """
    # logger.info("running find number in line")
    if position == 'start':
        number_str = ''
        i = 0
        while line[i].isdigit() and i < len(line):
            number_str += line[i]
            i += 1

    elif position == 'end':
        number_str = ['']*5
        i = -1
        while line[i].isdigit() and abs(i) < len(line) and i >= -5:
            number_str[i] = line[i]
            i -= 1

    else:
        number_str = ['']*10
        i = 5
        offset_from_center = 0

        # search backwards
        # start from line @ index and i = 5 (middle of string to write to)
        # at each iteration, increase the offset
        # read from line @ index - offset to number_str @ i - offset
        # the first condition in the where statement checks that we haven't
        # hit the start of the string to write to
        # the second condition checks that we haven't hit the start of the
        # input string.
        while i-offset_from_center > 0 and \
                index-offset_from_center >= 0 and \
                line[index-offset_from_center].isdigit():
            number_str[i-offset_from_center] = line[index-offset_from_center]
            offset_from_center += 1

        # reset counters
        i = 5
        offset_from_center = 0

        # search forwards
        while i+offset_from_center < 10 and \
                index + offset_from_center <= len(line) and \
                line[index+offset_from_center].isdigit():
            number_str[i+offset_from_center] = line[index+offset_from_center]
            offset_from_center += 1

    number_str = ''.join(number_str).strip()
    number = int(number_str)
    return number


def get_numbers(line_below, this_line, line_above, index):
    """
    1 2 3
    4 * 5
    6 7 8

    need to count the distinct number of numbers adjacent to the *.

    Handle this in a series of cases. Look at positions 1-5 only, positions
    6,7,8 has the same logic as 1,2,3

    1. 1 only
    2. 1 2
    3. 1 2 3
    4. 2 only
    5. 2 3
    6. 3 only
    7. 1 and 3 only
    8. 4 only
    9. 5 only
    """

    def which_digits(line): return [
        line[0].isdigit(), line[1].isdigit(), line[2].isdigit()]

    numbers = []

    # same logic for above and below
    for line in [line_below, line_above]:
        seg = line[index-1:index+2]

        match which_digits(seg):

            case [True, False, False]:
                numbers.append(find_number_in_line(
                    line[:index], 'end', None))

            case [True, True, False]:
                numbers.append(find_number_in_line(
                    line[:index+1], 'end', None))

            case [True, True, True]:
                numbers.append(find_number_in_line(
                    line, 'middle', index))

            case [False, True, False]:
                numbers.append(int(seg[1]))

            case [False, True, True]:
                numbers.append(find_number_in_line(
                    line[index:], 'start', None))

            case [False, False, True]:
                numbers.append(find_number_in_line(
                    line[index+1:], 'start', None))

            case [True, False, True]:
                numbers.append(find_number_in_line(
                    line[index+1:], 'start', None))
                numbers.append(find_number_in_line(
                    line[:index], 'end', None))

    # position 4
    if this_line[index-1].isdigit():
        numbers.append(find_number_in_line(this_line[:index], 'end', None))

    # position 5
    if this_line[index+1].isdigit():
        numbers.append(find_number_in_line(this_line[index+1:], 'start', None))

    return numbers


def get_gear_ratios(line_below, this_line, line_above):
    """Look for all instances of '*' in this_line.
       if there are 2 numbers adjacent to the *, then multiply them together"""

    # loop through all characters in this line and look for a '*'.
    gear_ratios = []

    # pad the lines
    # all lines are terminated wtih \n, so skip that too
    line_above = '.' + line_above[:-1] + '.'
    this_line = '.' + this_line[:-1] + '.'
    line_below = '.' + line_below[:-1] + '.'

    logger.info("="*80)
    logger.info("abv: " + line_above)
    logger.info("ths: " + this_line)
    logger.info("bel: " + line_below)
    for i in range(1, len(this_line) - 1):
        char = this_line[i]
        if char == '*':
            numbers = get_numbers(line_below, this_line, line_above, i)
            logger.info("numbers = " + str(numbers))
            if len(numbers) == 2:
                gear_ratios.append(numbers[0] * numbers[1])

    return sum(gear_ratios)


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
