import sys
import os
import socket
from PyQt5 import QtCore, QtWidgets

class UDPWorker(QtCore.QObject):
    dataChanged = QtCore.pyqtSignal(str)

    def __init__(self, parent=None):
        super(UDPWorker, self).__init__(parent)
        self.server_start = False

    @QtCore.pyqtSlot()
    def start(self):
        self.server_start = True
        ip = "192.168.1.4"
        port = 515
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((ip,port))
        self.process()

    def process(self):
        while self.server_start:
            data, addr = self.sock.recvfrom(1024)
            self.dataChanged.emit(str(data))

class UDPWidget(QtWidgets.QWidget):
    started = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super(UDPWidget, self).__init__(parent)
        btn = QtWidgets.QPushButton("Click Me")
        btn.clicked.connect(self.started)
        self.lst = QtWidgets.QListWidget()

        lay = QtWidgets.QVBoxLayout(self)
        lay.addWidget(QtWidgets.QLabel("udp receiver"))
        lay.addWidget(btn)
        lay.addWidget(self.lst)

        self.setWindowTitle("udp receive")

    @QtCore.pyqtSlot(str)
    def addItem(self, text):
        self.lst.insertItem(0, text)

if __name__ == '__main__':
    import sys
    app = QtWidgets.QApplication(sys.argv)
    w = UDPWidget()
    worker = UDPWorker()
    thread = QtCore.QThread()
    thread.start()
    worker.moveToThread(thread)
    w.started.connect(worker.start)
    worker.dataChanged.connect(w.addItem)
    w.show()
    sys.exit(app.exec_())