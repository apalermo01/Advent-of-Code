import argparse
import re
from functools import reduce

def parse_line_1(line):

    true_nums = {
            'red': 12,
            'green': 13,
            'blue': 14
            }

    line = line[:-1]
    game_id = int(re.search("\d+(?=:)", line).group(0))

    rounds = ''.join(line.split(':')[1:]).split(';')
    for r in rounds:

        for color in true_nums:
            num_color = re.search(f"\d+(?= {color})", r)
            if num_color is not None and int(num_color.group(0)) > true_nums[color]:
                return game_id, False


    return game_id, True

def parse_line_2(line):
    min_cubes_needed = []
    for c in ['red', 'green', 'blue']:
        match = re.findall(f"\d+(?= {c})", line)
        max_cubes = max([int(e) for e in match])
        min_cubes_needed.append(max_cubes)

    prod = reduce(lambda x, y: x*y, min_cubes_needed)
    return prod

def problem_1(lines):
    possible_ids = []
    for line in lines:
        game_id, possible = parse_line_1(line)
        if possible:
            possible_ids.append(game_id)
    return sum(possible_ids)

def problem_2(lines):
    powers = []
    for line in lines:
        powers.append(parse_line_2(line))
    return sum(powers)

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
