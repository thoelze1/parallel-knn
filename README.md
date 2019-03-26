# Parallel k-NN

## Assumptions
- Number of dimensions is a positive integer
- Training file and query file are well formed files that match in dimension
- Requested k is not less than total number of training points

## To Do
- Store points based on Ryan's idea
- Improve testing suite with test-truth.sh and test-time.sh
- Pass Vector to recursive query calls instead of "priority queue"
- KDNode as subclass of KDTree
- Add training function to KDTree (for easier timing in main - right now constructor copies over points too, which takes some time)
- Tune parameters:
    - Number of points per leaf cell (per hypercube)
    - Number of points to sample for median
    - Number of children per node in k-d tree (ternary tree? quaternary?)
- Implement this bad boy in rust
- Implement multithreaded single query
- Implement special case code for n = 1 (Queries could be O(k)...)

## Expectations
- Linear speedup of queries
- ~Linear speedup of training up to 4 cores

## Notes-to-self
- PROT_PRIVATE (lots of page faults)
- Copy all floats (sequential is fast)
- Indices of all floats (n/d indices as opposed to n floats)
- Edge case: more dimensions than points?
- Edge case: num points < k?
- Special case code for n = 1? Queries could be O(k)...
- mmap the results file (eliminates locking/concurrency issues)
- Join all training threads before starting querying
- File I/O Speed
    - Options: in place in mmap; copy to new array; sort array of offsets; copy to vector
    - mmap with private flag: 12-13 seconds for 1,000,000 pts
    - copy array of floats to new array of floats: 11-12 seconds for 1,000,000 pts
- Parallelization
    - Copy from mmap in parallel
    - Build disjoint training subtrees in parallel
    - Compute different queries at the same time
    - Make one query faster by searching disjoint subtrees in parallel
