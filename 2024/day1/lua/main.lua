function main()
  local file = "sample_input_1.txt"
  local f = io.open(file, 'r')
  local lines = {}
  for line in io.lines(file) do
    print(line)
  end
end

main()
