import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

X_range = 10
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

n = 10
zlow = -10
zhigh = 10

def randrange(n, vmin, vmax):
    '''
    Helper function to make an array of random numbers having shape (n, )
    with each number distributed Uniform(vmin, vmax).
    '''
    return (vmax - vmin)*np.random.rand(n) + vmin

def getXYZ(aa,ds):
    xs = []
    ys = []
    zs = []
    for i in  range(0,len(aa)):
        a = aa[i]*np.pi/180
        xs.append(ds[i]*np.sin(a))
        ys.append(ds[i]*np.cos(a))
        zs.append(0)
    return xs,ys,zs

for k in xrange(0,X_range):
    ds = randrange(n, 21, 2300)
    aa = randrange(n, 0, 100)
    zs = randrange(n, zlow, zhigh)
    #print aa
    xs,ys,zs = getXYZ(aa,ds)
    #zs = randrange(n, zlow, zhigh)

    #ax.plot(x, y, z)
    ax.scatter(xs, ys, zs)
    plt.draw()
    plt.pause(1.0)
    ax.cla()
