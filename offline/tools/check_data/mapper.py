#!/usr/bin/env python
"""mapper.py"""

import sys
col_value={}
pairs=sys.argv[1].split(',')
for pair in pairs:
    col, value=pair.split('=')
    col_value[int(col)]=value
# input comes from STDIN (standard input)
for line in sys.stdin:
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into words
    words = line.split('\x01')
    # increase counters
    count = 0
    for i in range(len(words)-1):
        # write the results to STDOUT (standard output);
        # what we output here will be the input for the
        # Reduce step, i.e. the input for reducer.py
        #
        # tab-delimited; the trivial word count is 1
        if i in col_value and words[i] == col_value[i]:
            count += 1
    if count == len(col_value):
        for col in col_value:
            print '%s\t%s' % (words[col], 1)
