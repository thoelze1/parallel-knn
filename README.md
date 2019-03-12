# Parallel k-NN

## Expectations
- Linear speedup of queries
- ~Linear speedup of training up to 4 cores

## Assumptions
- Number of dimensions is a positive integer
- Training file and query file are well formed files that match in dimension
- Requested k is not less than total number of training points

## To Do
- Implement this bad boy in rust
- Implement multithreaded single query
- Play around with parameters to improve timing
- Change main file to use struct initialization (packed structs or whatever)
- Pass Vector to recursive query calls instead of "priority queue"
- Implement advanced pruning and compare with simple pruning
- KDNode as subclass of KDTree
- struct instead of union
- uint32_t instead of uint64_t where possible
- add helper to buildTree that doesn't track parameters
- add training function to KDTree (for easier timing in main - right now constructor copies over points too, which takes some time)
- Test whether NaN comparison is better than bool field
- Add parallelization for single queries
- Tune parameters:
    - Number of points per leaf cell (per hypercube)
    - Number of points to sample for median
    - Number of children per node in k-d tree (ternary tree? quaternary?)

## Notes
- PROT_PRIVATE (lots of page faults)
- Copy all floats (sequential is fast)
- Indices of all floats (n/d indices as opposed to n floats)
- Edge case: more dimensions than points?
- Edge case: num points < k?
- Special case code for n = 1? Queries could be O(k)...
- mmap the results file (eliminates locking/concurrency issues)
- Root of tree should also have bounding hypercube and pointer to points
- If tree is less-than-balanced, one thread working on training will finish before another, and should help unfinished threads continue training
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

## Tree Construction with Random Sampling and Smart Querying
- Training runtime: nlog(n)
- Query runtime best case: log(n)
- Query runtime worst case: n
```
needMore(node, cube, queryPoint, worstNeighbor, in)
    if in:
        return len(nn) < k or nn.min > distanceFromQueryToMedian
    if out:
        return len(nn) < k or nn.min > distanceToFurtherCube:

getBetterChild(node, cube, queryPoint, in)
    if in:
        return = (queryPoint[node.d] < node.median)? node.left : node.right
    else:
        leftDistance = min(distance to closest corner of node.left, distance to closest wall of node.left)
        rightDistance = min(distance to closest corner of node.left, distance to closest wall of node.left)
        if leftDistance < rightDistance
            return node.left
        else
            return node.right

getnn(node, cube, queryPoint, k, inCube)
    # Base case
    if node is leaf
        return k nn in leaf (or as many as you can)
    # Determine which subtree is better
    better = getBetterChild(node, cube, queryPoint, inCube)
    # Search better subtree
    newCube = cube
    if better == node.left
        newCube[dim][1] = node.median
    else
        newCube[dim][0] = node.median
    nn = getnn(better, newCube, queryPoint, k, in)
    # If necessary, search worse subtree
    if needMore(node, cube, queryPoint, in, nn[worst])
        newCube = cube
        newk = in? k-(number of nn closer to query than query is to wall) : k-(number of nn closer to query than query is to further cube)
        if better == node.left
            newCube[dim][0] = node.median
        else
            newCube[dim][1] = node.median
        othernn = getknn(node.right, newCube, queryPoint, newk)
        return best of othernn and nn
    else
        return knn

# returns pivot value of points[start] to points[end]
# by randomly sampling
getPivot(start,end,dim)
    sample = getRandomSample(points[start] to points[end])
    sort(sample by dim)
    pivotVal = sample[len(sample)/2].dim
    return pivotVal

# accepts indices of points array
# returns chosen pivot index
splitPoints(start,end,dim)
    if end - start < 10000
        sort(points[start] to points[end] by dim)
        return end - start/2
    p = getPivot(start,end,dim)
    pivotIndex = partition(points[start] to points[end] by dim with pivot p)
    return pivotIndex

buildTree(start,end,dim)
    # we stop when each leaf refers to a cell (hypercube) containing at most c points
    if end - start <= c:
        return Node(start = start,
                    end = end)
    pivotIndex = splitPoints(start,end,dim)
    node = Node(dim = dim,
                median = points[pivotIndex].dim
                start = 0,
                end = end)
    node.left = buildTree(start,pivotIndex,nextDim)
    node.right = buildTree(pivotIndex,end,nextDim)

readPoints()
readQueries()
time1()
tree = buildTree(points,0,numpoints-1,0)
time2()
for queryPoint in queries:
    query(tree, queryPoint)
time3()
```
