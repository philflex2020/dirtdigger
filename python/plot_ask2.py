import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

fig, ax = plt.subplots()
xdata, ydata = [], []
ln, = plt.plot([], [], 'ro', animated=True)

def init():
    ax.set_xlim(-20, 20)
    ax.set_ylim(-20, 20)
    return ln,

def update(frame):
    a = input (" angle ")
    d = input (" dist ")
    a = a*np.pi/180
    x = d*np.sin(a)
    y = d*np.cos(a)
    print"X is ", x

    xdata.append(x)
    ydata.append(y)
    ln.set_data(xdata, ydata)
    return ln,

ani = FuncAnimation(fig, update, frames=np.linspace(0, 2*np.pi, 128),
                    init_func=init, blit=True)
plt.show()
