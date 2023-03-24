import random
import matplotlib.pyplot as plt
x_coord = []
y_coord = []
arr1 = []
minx = miny = 9e18
maxx = maxy = -9e18
with open('input.txt') as f:
    poly = int(next(f))
    for i in range(poly):
        n = int(next(f))
        x_coord1 = []
        y_coord1 = []
        for j in range(n):
            x,y = [float(l) for l in next(f).split()]
            x_coord1.append(x)
            y_coord1.append(y)
            minx = min(minx, x)
            maxx = max(maxx, x)
            miny = min(miny, y)
            maxy = max(maxy, y)
        x_coord.append(x_coord1)
        y_coord.append(y_coord1)
f.close()
fig, ax = plt.subplots()
ax.set_facecolor('xkcd:grey')

# ax.plot(x_coord, y_coord)
ax.set_xlim(minx - 10, maxx + 10)
ax.set_ylim(miny - 10, maxy + 10)
# Make all four axis lines visible
ax.spines['bottom'].set_position('zero')
ax.spines['left'].set_position('zero')
for i in range(len(x_coord)):
    plt.plot(x_coord[i],y_coord[i])
    r = random.randint(0, 255)
    g = random.randint(0, 255)
    b = random.randint(0, 255)
    rgb = tuple([r / 255, g / 255, b / 255])
    plt.fill(x_coord[i],y_coord[i],color = rgb)
plt.show()
 
 
 
#Input format :
#Number of polygons
#Number of vertices of polygon
#List of vertices