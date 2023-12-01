import argparse

def parse_lines(lines):
    """First puzzle solution"""
    nums = []

    for line in lines:
        nums_in_line = [c for c in line if c.isnumeric()]
        num_this_line = int(nums_in_line[0] + nums_in_line[-1])
        nums.append(num_this_line)

    return sum(nums)

def parse_lines_2(lines):
    """Second puzzle solution"""

    nums = []
    digits = {'one': 1,
              'two': 2,
              'three': 3,
              'four': 4,
              'five': 5,
              'six': 6,
              'seven': 7,
              'eight': 8,
              'nine': 9}

    for line in lines:
        nums_in_line = []
        while len(line) > 0:
            found_numeric_word = False
            for key in digits:
                if len(line) >= len(key) and line[:len(key )] == key:
                    nums_in_line.append(str(digits[key]))
                    line = line[len(key)-1:]
                    found_numeric_word = True
                    break
            if line[0].isnumeric():
                nums_in_line.append(str(line[0]))
            if not found_numeric_word:
                line = line[1:]
        num = int(nums_in_line[0] + nums_in_line[-1])
        nums.append(int(nums_in_line[0] + nums_in_line[-1]))
        sample += 1
    return sum(nums)

def main():
    args = parse_args()
    path = args.path
    with open(path, "r") as f:
        lines = f.readlines()

    if args.method == '1':
        answer = parse_lines(lines)
        print("answer is: ", answer)
    elif args.method == '2':
        answer = parse_lines_2(lines)
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
