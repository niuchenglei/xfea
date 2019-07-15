#!/usr/bin/env python
"""mapper.py"""

import sys
col_value={}
pairs=sys.argv[1].split(',')
for pair in pairs:
    col_value[pair]=1
# input comes from STDIN (standard input)
for line in sys.stdin:
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into words
    words = line.split('\t')
    # increase counters
    count = 0
    for i in range(len(words)-1):
        # write the results to STDOUT (standard output);
        # what we output here will be the input for the
        # Reduce step, i.e. the input for reducer.py
        #
        # tab-delimited; the trivial word count is 1
        for item in col_value:
            if words[i].find(item) != -1:
                print '%s\t%s' % (words[i], 1)
