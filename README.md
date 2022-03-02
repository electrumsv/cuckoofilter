Cuckoo Filter
============

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

Filter implementation authors
-----------------------------
- Bin Fan <binfan@cs.cmu.edu>
- David G. Andersen <dga@cs.cmu.edu>
- Michael Kaminsky <michael.e.kaminsky@intel.com>
