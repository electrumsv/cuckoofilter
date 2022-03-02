Cuckoo Filter
============

[![PyPI version](https://badge.fury.io/py/refcuckoo.svg)](https://badge.fury.io/py/refcuckoo)

Overview
--------
Cuckoo filter is a Bloom filter replacement for approximated set-membership queries. While Bloom filters are well-known space-efficient data structures to serve queries like "if item x is in a set?", they do not support deletion. Their variances to enable deletion (like counting Bloom filters) usually require much more space.

Cuckoo filters provide the ﬂexibility to add and remove items dynamically. A cuckoo filter is based on cuckoo hashing (and therefore named as cuckoo filter).  It is essentially a cuckoo hash table storing each key's fingerprint. Cuckoo hash tables can be highly compact, thus a cuckoo filter could use less space than conventional Bloom filters, for applications that require low false positive rates (< 3%).

For details about the algorithm and citations please use:

["Cuckoo Filter: Practically Better Than Bloom"](http://www.cs.cmu.edu/~binfan/papers/conext14_cuckoofilter.pdf) in proceedings of ACM CoNEXT 2014 by Bin Fan, Dave Andersen and Michael Kaminsky

API
---

The filter is wrapped in a Python extension and packaged for cross-platform installation.
`CuckooFilter` objects support the following operations:

*  `f.add(item) -> int`: insert an item to the filter
*  `f.contains(item) -> int`: return if item is already in the filter. Note that this method may return false positive results like Bloom filters
*  `f.remove(item) -> int`: delete the given item from the filter. Note that to use this method, it must be ensured that this item is in the filter (e.g., based on records on external storage); otherwise, a false item may be deleted.
*  `f.item_count -> int`: return the total number of items currently in the filter
*  `f.memory_size -> int`: return the filter size in bytes
*  `f.victim -> Optional[tuple[int, int]]`: return metadata about any victim

Usage
-----

There are two factors involved in calculating the size of the table used by the filter. Both are
based on the maximum key count that the application gives when creating the filter. The paper
states that it is not worth allocating space that will be filled above 95% as the filter
will be highly occupied and attempting to fit in extra items will fail. Capacity sizes are also
powers of two. This means that if the required space goes above 95% of the power of two size
for the given maximum key count, then the actual capacity allocated will be the next power of two.

The table below illustrates the rate of false positives for a table of a given capacity (Maximum)
used to a given extent (Added).

Key:

* Maximum: The highest maximum key count for each power of two.
* Added: The number of entries added.
* Minimum: The minimum number of false positives for 100 iterations of 1000000 random keys.
* Maximum: The maximum number of false positives for 100 iterations of 1000000 random keys.
* Maximum: The average number of false positives for 100 iterations of 1000000 random keys.
* Rate: The percentage of 1000000 lookups that were false positives.


| Maximum  | Added    | Minimum  | Maximum  | Average  | Rate        |
| ---------|--------- | -------- | -------- | -------- | ----------- |
|   503316 |     2000 |        1 |       14 |      7.2 |    0.000007 |
|   503316 |     4000 |        5 |       26 |     15.4 |    0.000015 |
|   503316 |     8000 |       19 |       42 |     29.0 |    0.000029 |
|   503316 |    16000 |       43 |       79 |     58.6 |    0.000059 |
|   503316 |    32000 |       88 |      146 |    119.5 |    0.000119 |
|   503316 |    64000 |      185 |      283 |    237.5 |    0.000237 |
|   503316 |   128000 |      419 |      528 |    475.9 |    0.000476 |
|   503316 |   256000 |      881 |     1021 |    954.9 |    0.000955 |
|   503316 |   503316 |     1745 |     1973 |   1872.1 |    0.001872 |
|  1006632 |     2000 |        0 |        8 |      3.5 |    0.000004 |
|  1006632 |     4000 |        1 |       16 |      7.5 |    0.000008 |
|  1006632 |     8000 |        5 |       27 |     15.7 |    0.000016 |
|  1006632 |    16000 |       19 |       44 |     29.8 |    0.000030 |
|  1006632 |    32000 |       38 |       78 |     60.4 |    0.000060 |
|  1006632 |    64000 |       99 |      143 |    118.8 |    0.000119 |
|  1006632 |   128000 |      197 |      279 |    239.0 |    0.000239 |
|  1006632 |   256000 |      434 |      534 |    476.8 |    0.000477 |
|  1006632 |   512000 |      892 |     1031 |    951.0 |    0.000951 |
|  1006632 |  1006632 |     1775 |     1957 |   1866.3 |    0.001866 |
|  2013265 |     2000 |        0 |        6 |      1.8 |    0.000002 |
|  2013265 |     4000 |        0 |        9 |      3.8 |    0.000004 |
|  2013265 |     8000 |        2 |       15 |      7.2 |    0.000007 |
|  2013265 |    16000 |        6 |       31 |     15.0 |    0.000015 |
|  2013265 |    32000 |       16 |       46 |     30.5 |    0.000030 |
|  2013265 |    64000 |       37 |       85 |     59.5 |    0.000059 |
|  2013265 |   128000 |       84 |      148 |    119.2 |    0.000119 |
|  2013265 |   256000 |      202 |      284 |    237.7 |    0.000238 |
|  2013265 |   512000 |      410 |      518 |    473.0 |    0.000473 |
|  2013265 |  1024000 |      897 |     1044 |    962.2 |    0.000962 |
|  2013265 |  2013265 |     1731 |     1960 |   1871.1 |    0.001871 |
|  4026531 |     2000 |        0 |        5 |      1.0 |    0.000001 |
|  4026531 |     4000 |        0 |        6 |      1.8 |    0.000002 |
|  4026531 |     8000 |        0 |        9 |      4.0 |    0.000004 |
|  4026531 |    16000 |        1 |       15 |      7.0 |    0.000007 |
|  4026531 |    32000 |        4 |       23 |     14.6 |    0.000015 |
|  4026531 |    64000 |       18 |       41 |     29.7 |    0.000030 |
|  4026531 |   128000 |       45 |       83 |     59.1 |    0.000059 |
|  4026531 |   256000 |       91 |      147 |    120.7 |    0.000121 |
|  4026531 |   512000 |      203 |      269 |    236.7 |    0.000237 |
|  4026531 |  1024000 |      420 |      525 |    478.6 |    0.000479 |
|  4026531 |  2048000 |      868 |     1027 |    953.9 |    0.000954 |
|  4026531 |  4026531 |     1756 |     1983 |   1865.2 |    0.001865 |


Filter implementation authors
-----------------------------
- Bin Fan <binfan@cs.cmu.edu>
- David G. Andersen <dga@cs.cmu.edu>
- Michael Kaminsky <michael.e.kaminsky@intel.com>
