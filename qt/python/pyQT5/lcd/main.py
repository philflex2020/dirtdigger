import sys
from random          import randint
from PyQt5.QtWidgets import QApplication, QMainWindow 
from PyQt5.QtCore    import QTimer

from ui import Ui_MainWindow

class Form(QMainWindow):
    def __init__(self):
        super().__init__()

        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.i = 0 
        self.voltageMin = 180 
        self.voltageMax = 180
        self.ui.lcdNumberCur.display(self.i)

        self.ui.pushButton.clicked.connect(self.startTimer)

        self.timer = QTimer(self)  
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.updateData)

        self.show()        

    def startTimer(self):  
        if self.ui.pushButton.text() == "Start Timer":
            self.timer.start(1000) 
            self.ui.pushButton.setText("Stop Timer")            
        else:
            self.ui.pushButton.setText("Start Timer")
            self.timer.stop() 

    def updateData(self):
        voltage = randint(90, 260)                  # <--- insert your average voltage here
        self.ui.lcdNumberCur.display(voltage) 
        if voltage > self.voltageMax:
            self.voltageMax = voltage
            self.ui.lcdNumberMax.display(self.voltageMax) 
        elif voltage < self.voltageMin:
            self.voltageMin = voltage
            self.ui.lcdNumberMin.display(self.voltageMin)            


if __name__ == '__main__':
    app = QApplication(sys.argv)
    frm = Form()
    sys.exit(app.exec_())