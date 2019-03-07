# Parallel k-NN

## Expectations
- Linear speedup of queries up to 8 cores
- Some speedup on tree building (not as much as on queries)

## To Do
1. Implement single-threaded k-d tree construction and querying according to pseudocode
2. Implement multithreaded training
3. Implement multithreaded multiple queries
4. Implement multithreaded single query
5. Play around with parameters to improve timing

## Parallelization
- Build disjoint training subtrees in parallel
- Compute different queries at the same time
- Make one query faster by searching disjoint subtrees in parallel

## Parameters to tune
- Number of points per leaf cell (per hypercube)
- Number of points to sample for median
- Number of threads per core
- Number of children per node in k-d tree (ternary tree? quaternary?)

## Assumptions
- Number of dimensions is a positive integer

## Notes
- Edge case: more dimensions than points?
- Edge case: num points < k?
- Special case code for n = 1? Queries could be O(k)...
- mmap the results file (eliminates locking/concurrency issues)
- Root of tree should also have bounding hypercube and pointer to points
- If tree is less-than-balanced, one thread working on training will finish before another, and should help unfinished threads continue training
- Join all training threads before starting querying

## Tree Construction with Random Sampling and Smart Querying
- Training runtime: nlog(n)
- Query runtime best case: log(n)
- Query runtime worst case: n
```
getCloserChild(node, cube, queryPoint)
    leftDistance = min(distance to closest corner of node.left, distance to closest wall of node.left)
    rightDistance = min(distance to closest corner of node.left, distance to closest wall of node.left)
    if leftDistance < rightDistance
        return node.left
    else
        return node.right

# accepts node, bounding box representing current node,
# querypoint, number of k still needed
# returns nn
externalknn(node, cube, queryPoint, k)
    if node is leaf
        return k nn in leaf (or as many as you can)
    newCube = cube
    if getCloserChild == node.left
        newCube[dim][1] = node.median
        nn = externalknn(node.left, newCube, queryPoint, k)
    else
        newCube[dim][0] = node.median
        nn = externalknn(node.right, newCube, queryPoint, k)
    if len(nn) < k or nn.min() > distanceToFurtherCube:
        othernn = get newnewk nn from furtherCube
        return best of othernn and nn
    return othernn

queryWorse(node, cube, queryPoint, k):
    newCube = cube
    if queryPoint[node.d] < node.median
        newCube[dim][0] = node.median
        newk = k - (number of nn closer to query than to wall)
        othernn = externalknn(node.right, newCube, queryPoint, newk)
    else
        newCube[dim][1] = node.median
        newk = k - (number of nn closer to query than to wall)
        othernn = externalknn(node.left, newCube, queryPoint, newk)
    return othernn

queryBetter(node, cube, queryPoint, k):
    newCube = cube
    if queryPoint[node.d] < node.median
        newCube[dim][1] = node.median
        nn = internalknn(node.left, newCube, queryPoint, k)
    else
        newCube[dim][0] = node.median
        nn = internalknn(node.right, newCube, queryPoint, k)
    return nn

# accepts node, bounding box representing current node,
# querypoint, number of k still needed, whether node is in cube
# returns nn
internalknn(node, cube, queryPoint, k)
    if node is leaf
        return k nn in leaf (or as many as you can)
    nn = queryBetter(node, cube, queryPoint, k)
    if len(nn) < k or nn.min > distanceFromQueryToMedian
        othernn = queryWorse(node, cube, queryPoint, k)
        return best of othernn and nn
    else
        return knn

needMore(node, cube, queryPoint, worstNeighbor, in)
    if in:
        return len(nn) < k or nn.min > distanceFromQueryToMedian
    if out:
        return len(nn) < k or nn.min > distanceToFurtherCube:

getBetterChild(node, cube, queryPoint, in)
    if in:
        betterChild = (queryPoint[node.d] < node.median)? node.left : node.right
    else:
        leftDistance = min(distance to closest corner of node.left, distance to closest wall of node.left)
        rightDistance = min(distance to closest corner of node.left, distance to closest wall of node.left)
        if leftDistance < rightDistance
            return node.left
        else
            return node.right

getknn(node, cube, queryPoint, k, in)
    # Base case
    if node is leaf
        return k nn in leaf (or as many as you can)
    # Determine which subtree is better
    better = getBetterChild(node, cube, queryPoint, in)
    # Search better subtree
    newCube = cube
    if better == node.left
        newCube[dim][1] = node.median
    else
        newCube[dim][0] = node.median
    nn = getknn(better, newCube, queryPoint, k, in)
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
