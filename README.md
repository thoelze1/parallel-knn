# Parallel k-NN

## Notes
- Input is a "training set" of k-dimensional points
- Edge case: more dimensions than points
- We should tune parameter c (number of points per hypercube)
- What if we make this a ternary tree instead of a binary tree? Make this a paramter to test...
- To compare pesudocode 1 method with pseudocode 2 method, compare standalone in-place-sort-all-points with sample-and-copy methods

## Pseudocode 3
```
query(node, point)
	while "in the current search area, there aren't
	       k neighbors nearer to point than there are
	       to any wall"
		expandSearchArea()
	if point[node.dimension] < node.median:
		return query(node.left, point)
	else if point[node.dimension] >= node.median:
		return query(node.right, point)

splitPoints(points, dim)
	sample = getSample(points)
	sort(sample,dim)
	median = sample[len(sample)/2].dim
	newpoints = []
	ileft = 0
	iright = len(points)-1
	for i = 0 to len(points):
		if points[i].dim < median:
			newpoints[ileft] = points[i]
			ileft++
		else
			newpoints[iright] = points[i]
			iright--
	assert ileft = iright + 1 # they pass eachother
	return newpoints, mid

# accepts array of points, returns k-d tree and array
buildTree(points)
	newpoints, mid = splitPoints(points,x)
	root = Node(dim = x,
	            start = 0,
	            end = len(points),
	            mid = mid
	            median = newpoints[mid].x)
	# Recurse left
	leftPoints = newpoints[0:mid]
	newLeftPoints, leftMid = splitPoints(leftPoints,y)
	leftKid = Node(dim = y,
	               start = 0,
	               end = mid,
	               mid = leftMid,
	               median = newLeftPoints[leftMid].y)
	root.left = leftKid
	# Recurse right
	rightPoints = newpoints[mid:-1]
	newRightPoints, rightMid = splitPoints(rightPoints,y)
	rightKid = Node(dim = y,
	                start = mid,
	                end = len(points),
	                mid = rightMid,
	                median = newRightPoints[rightMid].y)

main()
	points = []
	queryPoint = ()
	readPoints(points)
	readQueryPoint(queryPoint)
	tree = buildTree(points)
	neighbors = query(tree, queryPoint)
```

## Pseudocode 2
- Sorting at most 10,000 points at a time (which is O(1))
- Training runtime: n + 2*(n/2) + ... + (n/c)*c = nlog(n)
```
splitPoints(points, dim)
	sample = getSample(points)
	sort(sample,dim)
	median = sample[len(sample)/2].dim
	newpoints = []
	ileft = 0
	iright = len(points)-1
	for i = 0 to len(points):
		if points[i].dim < median:
			newpoints[ileft] = points[i]
			ileft++
		else
			newpoints[iright] = points[i]
			iright--
	assert ileft = iright + 1 # they pass eachother
	return newpoints, mid

main
	points = []
	readPoints(points)
	newpoints, mid = splitPoints(points,x)
	root = Node(dim = x,
	            start = 0,
	            end = len(points),
	            mid = mid
	            median = newpoints[mid].x)
	# Recurse left
	leftPoints = newpoints[0:mid]
	newLeftPoints, leftMid = splitPoints(leftPoints,y)
	leftKid = Node(dim = y,
	               start = 0,
	               end = mid,
	               mid = leftMid,
	               median = newLeftPoints[leftMid].y)
	root.left = leftKid
	# Recurse right
	rightPoints = newpoints[mid:-1]
	newRightPoints, rightMid = splitPoints(rightPoints,y)
	rightKid = Node(dim = y,
	                start = mid,
	                end = len(points),
	                mid = rightMid,
	                median = newRightPoints[rightMid].y)
	...
```

## Pseudocode 1
- We stop when each leaf refers to a hypercube containing at most c points
- Sort is nlog(n)
- Training runtime: nlog(n) + 2*(n/2)log(n/2) + 4*(n/4)log(n/4) + ... + (n/c)*c*log(c)
- This is nlog(n)log(n) (I think, because there are log(n/c) terms above)
```
points = []
readPoints(points)
sort(points,x)
root = Node(dim = x,
            start = 0,
            end = len(points))
leftPoints = points[:len(points)/2]
rightPoints = points[len(points)/2:]
sort(leftPoints,y)
leftKid = Node(dim = y,
               start = 0,
               end = len(points)/2)
sort(rightPoints,y)
rightKid = Node(dim = y,
                start = len(points)/2,
                end = len(points))
leftBotPoints = leftPoints[:len(leftPoints)/2]
leftTopPoints = leftPoints[len(leftPoints)/2:]
rightBotPoints = rightPoints[:len(rightPoints)/2]
rightTopPoints = rightPoints[len(rightPoints)/2:]
...
```
