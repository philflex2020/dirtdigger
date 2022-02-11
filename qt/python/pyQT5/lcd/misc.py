# # not working just some ideas


import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QPixmap
import RPi.GPIO as GPIO
import Test_rc
import time
import board
import busio
from adafruit_ads1x15.single_ended import ADS1115

i2c = busio.I2C(board.SCL,board.SDA)

adc = ADS1115(i2c)
GAIN = 2/3

c1 = 525.4
c2=28152

GPIO.setmode(GPIO.BCM)
Relay_1 = 27
Transducer = 17
GPIO.setup(Transducer,GPIO.IN)
GPIO.setup(Relay_1, GPIO.OUT)
GPIO.output(Relay_1, GPIO.HIGH)
GPIO.output(Relay_1, GPIO.LOW)


def Pressure_Reading():
    while True:
        time.sleep(0.02)
        r0 = adc.read_volts(0,gain = GAIN, data_rate=None)
        r1 = adc.read_adc(0, gain = GAIN, data_rate =None)
        Pressure_Volts = r0*94.72436287
        Pressure_adc = r1*(c1/c2)

        time.sleep(0.02)
        r01= adc.read_volts(0,gain = GAIN, data_rate=None)
        r11= adc.read_adc(0, gain = GAIN, data_rate =None)
        Pressure_Volts2 = r01*94.72436287
        Pressure_adc2 = r11*(c1/c2)

        time.sleep(0.02)
        r02= adc.read_volts(0,gain = GAIN, data_rate=None)
        r12= adc.read_adc(0, gain = GAIN, data_rate =None)
        Pressure_Volts3 = r02*94.72436287
        Pressure_adc3 = r12*(c1/c2)

        time.sleep(0.02)
        r03 = adc.read_volts(0,gain = GAIN, data_rate=None)
        r13 = adc.read_adc(0, gain = GAIN, data_rate =None)
        Pressure_Volts4 = r03*94.72436287
        Pressure_adc4 = r13*(c1/c2)

        time.sleep(0.02)
        r04= adc.read_volts(0,gain = GAIN, data_rate=None)
        r14= adc.read_adc(0, gain = GAIN, data_rate =None)
        Pressure_Volts5 = r04*94.72436287
        Pressure_adc5 = r14*(c1/c2)

        Pressure_Volts_Avg = (Pressure_Volts+Pressure_Volts2+Pressure_Volts3+Pressure_Volts4+Pressure_Volts5)/5
        Pressure_adc_Avg = (Pressure_adc+Pressure_adc2+Pressure_adc3+Pressure_adc4+Pressure_adc5)/5
        Pressure_Avg = (Pressure_Volts_Avg+Pressure_adc_Avg)/2

        return Pressure_Avg

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(788, 424)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.stackedWidget = QtWidgets.QStackedWidget(self.centralwidget)
        self.stackedWidget.setGeometry(QtCore.QRect(10, 0, 761, 501))
        self.stackedWidget.setObjectName("stackedWidget")
        self.page_3 = QtWidgets.QWidget()
        self.page_3.setObjectName("page_3")
        self.label = QtWidgets.QLabel(self.page_3)
        self.label.setGeometry(QtCore.QRect(200, 0, 341, 61))
        font = QtGui.QFont()
        font.setFamily("Arial Black")
        font.setPointSize(22)
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName("label")
        self.pushButton_2 = QtWidgets.QPushButton(self.page_3)
        self.pushButton_2.setGeometry(QtCore.QRect(220, 170, 141, 41))
        self.pushButton_2.setObjectName("pushButton_2")
        self.pushButton = QtWidgets.QPushButton(self.page_3)
        self.pushButton.setGeometry(QtCore.QRect(380, 170, 141, 41))
        self.pushButton.setObjectName("pushButton")
        self.label_2 = QtWidgets.QLabel(self.page_3)
        self.label_2.setGeometry(QtCore.QRect(120, 260, 541, 141))
        self.label_2.setObjectName("label_2")
        self.groupBox_2 = QtWidgets.QGroupBox(self.page_3)
        self.groupBox_2.setGeometry(QtCore.QRect(90, 40, 101, 221))
        self.groupBox_2.setTitle("")
        self.groupBox_2.setObjectName("groupBox_2")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.groupBox_2)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.label_7 = QtWidgets.QLabel(self.groupBox_2)
        self.label_7.setObjectName("label_7")
        self.verticalLayout_2.addWidget(self.label_7)
        self.lcdNumber = QtWidgets.QLCDNumber(self.groupBox_2)
        self.lcdNumber.setObjectName("lcdNumber")
        self.verticalLayout_2.addWidget(self.lcdNumber)
        self.lcdNumber.display(Pressure_Reading())



class ControlMainWindow(QtWidgets.QMainWindow):
    def __init__(self,parent=None):
        super(ControlMainWindow,self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.SettingsButton.clicked.connect(lambda:self.ui.stackedWidget.setCurrentIndex(1))
        self.ui.pushButton_5.clicked.connect(lambda:self.ui.stackedWidget.setCurrentIndex(0))

        self.ui.pushButton.clicked.connect(self.OnSwitch)
        self.ui.pushButton_2.clicked.connect(self.OffSwitch)
        self.ui.DelayButton.clicked.connect(self.Delay_Switch)

    def OnSwitch(self):
        GPIO.output(Relay_1, GPIO.HIGH)

    def OffSwitch(self):
        GPIO.output(Relay_1, GPIO.LOW)

    def Delay_Switch(self):
            time.sleep(5)
            GPIO.output(Relay_1,GPIO.HIGH)



if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    mySW = ControlMainWindow()
    mySW.show() 
    sys.exit(app.exec_()) 





 class Worker(QtCore.QThread):
    valueFound = QtCore.pyqtSignal(int, name="valueFound")
    GAIN = 2/3
    c1 = 525.4
    c2=28152
    Relay_1 = 27

    def __init__(self, parent=None):
        super(Worker, self).__init__(parent)
        self.runFlag = False
        self.i2c = busio.I2C(board.SCL,board.SDA)
        self.adc = ADS1115(self.i2c)
        GPIO.setmode(GPIO.BCM)        
        GPIO.setup(self.Relay_1, GPIO.OUT)
        GPIO.output(self.Relay_1, GPIO.HIGH)
        GPIO.output(self.Relay_1, GPIO.LOW)

    def startThread(self):
        self.runFlag = True
        self.start()

    def stopThread(self):
        self.runFlag = False



    def run(self):
        while self.runFlag:
            self.valueFound.emit(self.Pressure_Reading())


self.worker = Worker(self)
    self.worker.valueFound.connect(self.OnValueFound)

self.worker.startThread()

    def closeEvent(self, event):
        self.worker.stopThread()
        while self.worker.isRunning():
            pass
        event.accept()

    def OnValueFound(self, value):
        self.ui.lcdNumber.display(value)