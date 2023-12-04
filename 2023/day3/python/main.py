import argparse
import re

def parse_line_1(line):
    pass

def parse_line_2(line):
    pass

def generate_matches(line):
    matches = []
    s = True

    while s is not None:
        s = re.search(".?\d+.?", line)

        if s is not None:
            this_match = s.group(0)
            matches.append(this_match)
            old_line = line

            if this_match[-1].isnumeric():
                forward = len(this_match)
            else:
                forward = len(this_match)-1

            line = line[line.index(this_match) + forward:]
            if line == old_line:
                raise ValueError("bad iteration!")

    return matches


def problem_1(lines):
    LEN_LINES = len(lines)

    part_nums = []
    count = min(len(lines), 5)
    for row_idx in range(len(lines)):
        line = lines[row_idx]
        match_strings = generate_matches(line)
        nums_this_line = []
        for match in match_strings:
            surrounding_chars = match
            index_start = line.index(match)

            if row_idx > 0:
                line_above = lines[row_idx-1][index_start:index_start+len(match)]
                surrounding_chars += line_above

            if row_idx < LEN_LINES-1:
                line_below = lines[row_idx+1][index_start:index_start+len(match)]
                surrounding_chars += line_below

            mask = [not (c.isnumeric() or c == '.' or c == '\n')  for c in surrounding_chars]
            if any(mask):
                nums_this_line.append(int(re.findall("\d+", match)[0]))
        part_nums += nums_this_line
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
    parser.add_argument('--method', choices = ['1', '2'],
                        default = 1)
    args = parser.parse_args()
    return args

if __name__ == '__main__':
    main()
