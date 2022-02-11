from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMessageBox

def btn_clicked():
	print(" b clicked")
	QMessageBox.information(MainWindow, 'Welcome','PyQt5 running on a Pizero')

class Ui_MainWindow(object):
	def setupUi(self, MainWindow):
		MainWindow.setObjectName("MailWindow")
		MainWindow.resize(284,123)
		MainWindow.setObjectName("MailWindow")
		self.centralWidget = QtWidgets.QWidget(MainWindow)
		self.centralWidget.setObjectName("centralWidget")
		self.btn = QtWidgets.QPushButton(self.centralWidget)
		self.btn.setObjectName("btn")
		MainWindow.setCentralWidget(self.centralWidget)

		self.retranslateUi(MainWindow)
		QtCore.QMetaObject.connectSlotsByName(MainWindow)

	def retranslateUi(self, MainWindow):
		_translate = QtCore.QCoreApplication.translate
		MainWindow.setWindowTitle(_translate("MainWindow","Message Box Example"))
		self.btn.setText(_translate("MainWindow", "Click Me"))
		'''User Code'''
		self.btn.clicked.connect(btn_clicked)

if __name__ == "__main__":
	import sys
	app = QtWidgets.QApplication(sys.argv)
	MainWindow = QtWidgets.QMainWindow()
	ui = Ui_MainWindow()
	ui.setupUi(MainWindow)
	MainWindow.show()
	sys.exit(app.exec_())

