import socketserver
import concurrent.futures
import sys
from PyQt5 import QtGui
from PyQt5 import QtCore
#from Queue import Queue
import queue

HOST = 'localhost'
PORT = 12345
BUFSIZ = 1024
ADDR = (HOST, PORT)

# create a global queue object that both the handle() method and the QThread (see later in the code) can access
myqueue = queue.Queue()

class MyRequestHandler(socketserver.StreamRequestHandler):
    def handle(self):
        print('...connected from:', self.client_address)        
        data = self.rfile.readline().strip()
        print('Data from client %s' % data)
        myqueue.put(data)

class ServerWindow(QtGui.QMainWindow):
    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.setGeometry(1500, 100, 500, 500)

        self._control = QtGui.QWidget()
        self.setCentralWidget(self._control)

        l = QtGui.QVBoxLayout(self._control)
        self.t = QtGui.QTextEdit()
        l.addWidget(self.t)

        self.executor = futures.ThreadPoolExecutor(max_workers=1)
        self.startServerThread()

        self.show()

    @QtCore.pyqtSlot(str)
    def receive_data(self, data):
        self.t.moveCursor(QtGui.QTextCursor.End)
        self.t.insertPlainText( data )

    def startServerThread(self):
        self.executor.submit(self.startServer)

        # How to get information from the thread while it is still running?

    def startServer(self):
        print('starting server')
        self.tcpServ = socketserver.TCPServer(ADDR, MyRequestHandler)
        print('waiting for connection...')
        self.tcpServ.serve_forever()

        # How to get information from the client (custom request handler)
        # back to the GUI in a thread safe manner?

# This class runs in a QThread and listens on the end of a queue and emits a signal to the GUI
class MyReceiver(QtCore.QObject):
    mysignal = QtCore.pyqtSignal(str)

    def __init__(self,queue,*args,**kwargs):
        QtCore.QObject.__init__(self,*args,**kwargs)
        self.myqueue = myqueue

    @QtCore.pyqtSlot()
    def run(self):
        while True:
            text = self.myqueue.get()
            self.mysignal.emit(text)


def launch():
    app = QtGui.QApplication(sys.argv)

    ex = ServerWindow()

    # Create thread that will listen on the other end of the queue, and send the text to the textedit in our application
    thread = QtCore.QThread()
    my_receiver = MyReceiver(myqueue)
    my_receiver.mysignal.connect(ex.receive_data)
    my_receiver.moveToThread(thread)
    thread.started.connect(my_receiver.run)
    thread.start()

    ret_code = app.exec_()
    ex.tcpServ.shutdown()
    sys.exit(ret_code)

if __name__ == '__main__':
    launch()
