import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FixedLocator, FormatStrFormatter
import matplotlib, time

class plot3dClass( object ):

    def __init__( self, systemSideLength, lowerCutoffLength ):
        self.systemSideLength = systemSideLength
        self.lowerCutoffLength = lowerCutoffLength
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot( 111, projection='3d' )
        self.ax.set_zlim3d( -10e-9, 10e9 )

        rng = np.arange( 0, self.systemSideLength, self.lowerCutoffLength )
        self.X, self.Y = np.meshgrid(rng,rng)

        self.ax.w_zaxis.set_major_locator( LinearLocator( 10 ) )
        self.ax.w_zaxis.set_major_formatter( FormatStrFormatter( '%.03f' ) )

        heightR = np.zeros( self.X.shape )
        self.surf = self.ax.plot_surface( 
            self.X, self.Y, heightR, rstride=1, cstride=1, 
            cmap=cm.jet, linewidth=0, antialiased=False )
        # plt.draw() maybe you want to see this frame?

    def drawNow( self, heightR ):
        self.surf.remove()
        self.surf = self.ax.plot_surface( 
            self.X, self.Y, heightR, rstride=1, cstride=1, 
            cmap=cm.jet, linewidth=0, antialiased=False )
        plt.draw()                      # redraw the canvas
        time.sleep(10)

matplotlib.interactive(True)

p = plot3dClass(5,1)
for i in range(2):
    p.drawNow(np.random.random(p.X.shape))
