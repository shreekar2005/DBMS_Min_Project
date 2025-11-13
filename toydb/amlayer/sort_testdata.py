#!usr/bin/python

# This python script is used to create testdata.txt where rows will be sorted wrt first number of each row.
# Then we will use ../splayer/build/testcovnert to convert testdata.txt to testdata.tdb
# testdata.tdb is usefull for test_objective3.c

with open("testdata_unsorted.txt", "r") as f:
    lines = f.readlines()

lines = [line for line in lines if line.strip()]
lines.sort(key=lambda line: int(line.split(';')[0]))

with open("testdata_sorted.txt", "w") as f:
    f.writelines(lines)

print("Sorted data has been written to testdata_sorted.txt")