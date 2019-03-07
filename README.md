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
- Special case code for n = 1?
- mmap the results file (eliminates locking/concurrency issues)
- Root of tree should also have bounding hypercube and pointer to points
- If tree is less-than-balanced, one thread working on training will finish before another, and should help unfinished threads continue training
- Join all training threads before starting querying

## Tree Construction with Random Sampling and Smart Querying
- Training runtime: nlog(n)
- Query runtime best case: log(n)
- Query runtime worst case: n
```
# accepts query point, current nearest neighbors (priority queue), bounding hypercube (priority queue)
# returns whether or not we're done
solved(point, cnn, hypercube)
	furthestCNN = cnn.peek()
	closestWall = hypercube.peek()
	if closestWall < furthestCNN
		return True
	else
		return False

# accepts node, bounding box representing current node,
# querypoint, number of k still needed, whether node is in cube
# returns knn
query(node, cube, queryPoint, k, isInCube)
    if node is leaf
        return k nn in leaf (or as many as you can)
    if isInCube
        newCube = cube
        if queryPoint[node.d] < node.median
            newCube[dim][1] = node.median
            nn = query(node.left, newCube, queryPoint, k, true)
        else
            newCube[dim][0] = node.median
            nn = query(node.right, newCube, queryPoint, k, true)
        if len(nn) < k or nn.min > distanceFromQueryTo
            newCube = cube
            if queryPoint[node.d] < node.median
                newCube[dim][0] = node.median
                newk = k - (number of nn closer to query than to wall)
                othernn = query(node.right, newCube, queryPoint, newk, false)
            else
                newCube[dim][1] = node.median
                newk = k - (number of nn closer to query than to wall)
                othernn = query(node.left, newCube, queryPoint, newk, false)
            return best of othernn and nn
        else
            return knn
    else
        othernn = get newk nn from closerCube
        if nn.min() > distanceToFurtherCube:
            otherothernn = get newnewk nn from furtherCube
            return best of othernn and nn
        return othernn

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
