"""
This script fills filters of different capacities to different levels, and measures the rate of
false positives for 1000000 lookups. The results are put in a `test.csv` file in the current
directory. The following table was for a filter using 12 bits of storage per item.

maximum entries, added entries, minimum, maximum, average, false rate
         503316,          2000,       1,      14,     7.2,   0.000007
         503316,          4000,       5,      26,    15.4,   0.000015
         503316,          8000,      19,      42,    29.0,   0.000029
         503316,         16000,      43,      79,    58.6,   0.000059
         503316,         32000,      88,     146,   119.5,   0.000119
         503316,         64000,     185,     283,   237.5,   0.000237
         503316,        128000,     419,     528,   475.9,   0.000476
         503316,        256000,     881,    1021,   954.9,   0.000955
         503316,        503316,    1745,    1973,  1872.1,   0.001872
        1006632,          2000,       0,       8,     3.5,   0.000004
        1006632,          4000,       1,      16,     7.5,   0.000008
        1006632,          8000,       5,      27,    15.7,   0.000016
        1006632,         16000,      19,      44,    29.8,   0.000030
        1006632,         32000,      38,      78,    60.4,   0.000060
        1006632,         64000,      99,     143,   118.8,   0.000119
        1006632,        128000,     197,     279,   239.0,   0.000239
        1006632,        256000,     434,     534,   476.8,   0.000477
        1006632,        512000,     892,    1031,   951.0,   0.000951
        1006632,       1006632,    1775,    1957,  1866.3,   0.001866
        2013265,          2000,       0,       6,     1.8,   0.000002
        2013265,          4000,       0,       9,     3.8,   0.000004
        2013265,          8000,       2,      15,     7.2,   0.000007
        2013265,         16000,       6,      31,    15.0,   0.000015
        2013265,         32000,      16,      46,    30.5,   0.000030
        2013265,         64000,      37,      85,    59.5,   0.000059
        2013265,        128000,      84,     148,   119.2,   0.000119
        2013265,        256000,     202,     284,   237.7,   0.000238
        2013265,        512000,     410,     518,   473.0,   0.000473
        2013265,       1024000,     897,    1044,   962.2,   0.000962
        2013265,       2013265,    1731,    1960,  1871.1,   0.001871
        4026531,          2000,       0,       5,     1.0,   0.000001
        4026531,          4000,       0,       6,     1.8,   0.000002
        4026531,          8000,       0,       9,     4.0,   0.000004
        4026531,         16000,       1,      15,     7.0,   0.000007
        4026531,         32000,       4,      23,    14.6,   0.000015
        4026531,         64000,      18,      41,    29.7,   0.000030
        4026531,        128000,      45,      83,    59.1,   0.000059
        4026531,        256000,      91,     147,   120.7,   0.000121
        4026531,        512000,     203,     269,   236.7,   0.000237
        4026531,       1024000,     420,     525,   478.6,   0.000479
        4026531,       2048000,     868,    1027,   953.9,   0.000954
        4026531,       4026531,    1756,    1983,  1865.2,   0.001865
"""

from collections import defaultdict
import os

import refcuckoo

LOOKUP_COUNT = 1000000
ITERATIONS = 100

with open("test.csv", "w") as f:
    print("maximum entries, added entries, minimum, maximum, average, false rate", file=f)

    for shift in range(19, 23):
        # Maximum entry usage can only be 96% of the capacity, above that the filter increases
        # the capacity level to the next power of two. So the maximum entry value here is the
        # most the filter advises adding for the given capacity level.
        maximum_entries = int((1 << shift) * 0.96)
        false_positive_map: dict[int, list[int]] = defaultdict(list)
        for i in range(ITERATIONS):
            print(f"{i}, ", end="", flush=True)
            filter = refcuckoo.CuckooFilter(maximum_entries)
            addition_count = 2000
            last_addition_count = 0
            while addition_count <= maximum_entries:
                for i in range(last_addition_count, addition_count):
                    k = os.urandom(32)
                    filter.add(k)

                false_positives = 0
                for j in range(LOOKUP_COUNT):
                    k = os.urandom(32)
                    if filter.contains(k) == 0:
                        false_positives += 1
                false_positive_map[addition_count].append(false_positives)
                last_addition_count = addition_count
                addition_count *= 2
                if last_addition_count < maximum_entries and addition_count > maximum_entries:
                    addition_count = maximum_entries

        print()

        for addition_count, false_positive_counts in false_positive_map.items():
            average_false_positives = sum(false_positive_counts)/ITERATIONS
            print(f"{maximum_entries:15}, {addition_count:13}, {min(false_positive_counts):7}, "
                f"{max(false_positive_counts):7}, {average_false_positives:7.1f}, "
                f"{average_false_positives/LOOKUP_COUNT:10.6f}",
                file=f)
