from collections import namedtuple  
import math     
def sort_counterclockwise(points, centre = None):
  if centre:
    centre_x, centre_y = centre
  else:
    centre_x, centre_y = sum([x for x,_ in points])/len(points), sum([y for _,y in points])/len(points)
  angles = [math.atan2(y - centre_y, x - centre_x) for x,y in points]
  counterclockwise_indices = sorted(range(len(points)), key=lambda i: angles[i])
  counterclockwise_points = [points[i] for i in counterclockwise_indices]
  return counterclockwise_points

points = []
for i in range(18):
    point_x, point_y = input().split()        # 
    # print(point_x, point_y)
    points.append((float(point_x), float(point_y)))

points = sort_counterclockwise(points)
points = points[::-1]
print(len(points))
for x, y in points:
    print(x, y)
