# Parallel k-NN

## Expectations
- Linear speedup of queries up to 8 cores
- Some speedup on tree building (not as much as on queries)

## To Do
1. Implement single-threaded k-d tree construction and querying
2. Implement multithreading

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
- Input is a "training set" of k-dimensional points
- Edge case: more dimensions than points?
- To compare pesudocode 1 method with pseudocode 2 method, compare standalone in-place-sort-all-points with sample-and-copy methods
- Each node in the tree represents a hypercube (cell)
- Special case code for n = 1?
- mmap the results file (eliminates locking issues)

## Dumb Querying
- Query runtime best case: log(n)
- Query runtime worst case: ?
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

# accepts root node, query point
# returns leaf node and corresponding hypercube
traverseDown(root, point)
	currNode = root
	hypercube = [(inf,inf),(inf,inf),...]
	while currNode has children:
		d = currNode.dim
		hyperplane = currNode.median
		if point[d] < hyperplane
			hyperCube[d].upper = hyperplane
			currNode = currNode.left
		else
			hyperCube[d].lower = hyperplane
			currNode = currNode.right
	return currNode, hypercube

# accepts node, query point
# returns k nearest neighbors
query(root, query, k)
	leafNode, hypercube = traverseDown(root, query)
	cnn = priorityQueue
	for points[node.start] to points[node.start+k]:
		add (point,distance) to cnn
	walls = priorityQueue
	for dimension in hypercube:
		add (dimension.upper,distance) to walls
		add (dimension.lower,distance) to walls
	while "in the current search area, there aren't
	       k neighbors nearer to point than there are
	       to any wall"
		doubleSearchArea()
	if point[node.dimension] < node.median:
		return query(node.left, point)
	else if point[node.dimension] >= node.median:
		return query(node.right, point)

tree = buildTree()
```

## Tree Construction with Random Sampling
- Training runtime: nlog(n)

   because there are log(c/n) terms in n + 2*(n/2) + ... + (n/c)*c

   and each sort is of at most 10,000 points, an O(1) operation
```
# points is a global
points

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
tree = buildTree(points,0,numpoints-1,0)
```

## Simple Tree Construction
- Training runtime: nlog(n)log(n)

   because there are log(n/c) terms in nlog(n) + 2*(n/2)log(n/2) + 4*(n/4)log(n/4) + ... + (n/c)*c*log(c)

   also see [this post](https://stackoverflow.com/questions/44231116/is-complexity-ologn-logn-2-logn-4-logn-8-log2-olog))
```
# points is global
points

# accepts indices of points array
# returns k-d tree
buildTree(start,end,dim)
    # we stop when each leaf refers to a cell (hypercube) containing at most c points
    if end - start <= c:
        return Node(start = start,
                    end = end)
    sort(points[start] to points[end] by dim)
    node = Node(dim = dim,
                median = points[len/2].dim
                start = 0,
                end = end)
    node.left = buildTree(start,start+end/2,nextDim)
    node.right = buildTree(start+end/2,end,nextDim)

readPoints()
tree = buildTree(points,0,numpoints-1,0)
```

## Brute Force
- Training runtime: N/A
- Query runtime: nlog(n)
```
readPoints()
readQueries()
for query in queries
    sortByDistance(points,query)
    knn = points[0] to points[k-1]
```
