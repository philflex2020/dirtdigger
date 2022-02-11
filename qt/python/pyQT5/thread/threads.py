
import sys
#import sysInfo
import os
import time
from PyQt5 import QtCore, QtWidgets

def updateProgBar(val):
    print ("progress value :", val)
    #ui.progressBar.setValue(val)

class ThreadClass(QtCore.QThread):
    # Create the signal
    sig = QtCore.pyqtSignal(int)

    def __init__(self, parent=None):
        super(ThreadClass, self).__init__(parent)
        self.cval = 20
        # Connect signal to the desired function
        self.sig.connect(updateProgBar)

    def run(self):
        while True:
            time.sleep(1) 
            #val = os.getloadavg()
            val = self.cval
            self.cval += 1

            # Emit the signal
            self.sig.emit(val)


threadclass = ThreadClass()

# This section does not work
#connect(threadclass, QtCore.pyqtSignal('CPUVALUE'), updateProgBar)
# This section does not work



if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv) 
    threadclass.start()
    #Form.show()
    sys.exit(app.exec_())
    
