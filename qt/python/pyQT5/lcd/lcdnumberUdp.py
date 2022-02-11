from PyQt5 import QtCore, QtGui, QtWidgets, QtNetwork

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(500, 300)

        self.udpSocket = QtNetwork.QUdpSocket(self)
        self.udpSocket.bind(5007)
        self.udpSocket.readyRead.connect(self.processPendingDataGrams)
        
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.groupBox = QtWidgets.QGroupBox(self.centralwidget)
        self.groupBox.setGeometry(QtCore.QRect(0, 0, 400, 200))
        self.groupBox.setObjectName("groupBox")

        self.lcdNumberMax = QtWidgets.QLCDNumber(self.groupBox)
        self.lcdNumberMax.setGeometry(QtCore.QRect(225, 10, 151, 41))
        self.lcdNumberMax.setObjectName("lcdNumberMax")
        self.lcdNumberCur = QtWidgets.QLCDNumber(self.groupBox)
        self.lcdNumberCur.setGeometry(QtCore.QRect(225, 55, 151, 41))
        self.lcdNumberCur.setObjectName("lcdNumberCur")
        self.lcdNumberMin = QtWidgets.QLCDNumber(self.groupBox)
        self.lcdNumberMin.setGeometry(QtCore.QRect(225, 100, 151, 41))
        self.lcdNumberMin.setObjectName("lcdNumberMin")        

        self.label = QtWidgets.QLabel(self.groupBox)
        self.label.setGeometry(QtCore.QRect(20, 50, 150, 50))
        self.label.setObjectName("label")
        
        self.labelMax = QtWidgets.QLabel(self.groupBox)
        self.labelMax.setGeometry(QtCore.QRect(20, 8, 150, 50))
        self.labelMax.setObjectName("labelMax")
        self.labelMin = QtWidgets.QLabel(self.groupBox)
        self.labelMin.setGeometry(QtCore.QRect(20, 95, 150, 50))
        self.labelMin.setObjectName("labelMin")        
        self.statusLabel = QtWidgets.QLabel(self.groupBox)
        self.statusLabel.setGeometry(QtCore.QRect(20, 165, 150, 50))
        self.statusLabel.setObjectName("dgram")
        
        self.pushButton = QtWidgets.QPushButton("pushButton", self.groupBox) #(self.scrollAreaWidgetContents)
        self.pushButton.setGeometry(QtCore.QRect(180, 225, 75, 23))
        self.pushButton.setObjectName("pushButton")

        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "LCDNumber - Timer(Start-Stop)"))
        self.groupBox.setTitle(_translate("MainWindow", "Data"))
        self.label.setText(_translate("MainWindow", "Current voltage in the network"))
        self.labelMin.setText(_translate("MainWindow", "Min voltage in the network"))
        self.labelMax.setText(_translate("MainWindow", "Max voltage in the network"))
        self.pushButton.setText(_translate("MainWindow", "Start Timer"))

    def processPendingDatagrams(self):
        while self.udpSocket.hasPendingDatagrams():
            dgram,host,port=self.udpSocket.readDatagram(self.udpSocket.pendingDatagramSize())

            try:
                dgram = str(dgram, encoding='ascii')

            except TypeError:
                pass
            self.statusLabel.setText("Rec dgram:", dgram)
                
if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
