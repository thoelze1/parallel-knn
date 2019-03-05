# Parallel k-NN

## Notes
- Input is a "training set" of k-dimensional points

## Pseudocode
points = []
readPoints(points)
sort(points,x)
leftPoints = points[:len(points)/2]
rightPoints = points[len(points)/2:]
sort(leftPoints,y)
sort(rightPoints,y)
leftBotPoints = leftPoints[:len(leftPoints)/2]
leftTopPoints = leftPoints[len(leftPoints)/2:]
rightBotPoints = rightPoints[:len(rightPoints)/2]
rightTopPoints = rightPoints[len(rightPoints)/2:]
