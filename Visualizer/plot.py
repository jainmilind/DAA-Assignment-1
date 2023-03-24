import random
import matplotlib.pyplot as plt
 
## This is a list of X coordinates of input polygon
x_coord = list()
## This is a list of Y coordinates of input polygon
y_coord = list()
 
## Minimum possible value of X coordinate
minx = 9e18
## Minimum possible value of Y coordinate
miny = 9e18
## Maximum possible value of X coordinate
maxx = -9e18
## Maximum possible value of Y coordinate
maxy = -9e18
 
with open('input.txt') as f:
    ## number of polygons in given input
    poly = int(next(f))
 
    for i in range(poly):
        ## number of points in i-th polygon
        n = int(next(f))
 
        ## X coordinates of current polygon
        x_coord_cur = list()
        ## Y coordinates of current polygon
        y_coord_cur = list()
        
        for j in range(n):
            ## current x coordinate
            x = -1.0
            ## current y coordinate
            y = -1.0
            x, y = [float(l) for l in next(f).split()]
            x_coord_cur.append(x)
            y_coord_cur.append(y)
            minx = min(minx, x)
            maxx = max(maxx, x)
            miny = min(miny, y)
            maxy = max(maxy, y)
 
        x_coord.append(x_coord_cur)
        y_coord.append(y_coord_cur)
 
## matplotlib's objects
fig, ax = plt.subplots()
ax.set_facecolor('xkcd:grey')
 
ax.set_xlim(minx - 10, maxx + 10)
ax.set_ylim(miny - 10, maxy + 10)
ax.spines['bottom'].set_position('zero')
ax.spines['left'].set_position('zero')
 
for i in range(len(x_coord)):
    plt.plot(x_coord[i], y_coord[i])
    ## red colour intensity
    r = random.randint(0, 255)
    ## green colour intensity
    g = random.randint(0, 255)
    ## blue colour intensity
    b = random.randint(0, 255)
    rgb = tuple([r / 255, g / 255, b / 255])
    plt.fill(x_coord[i], y_coord[i], color = rgb)
 
plt.show()