import asyncore
import socket

connects = []

class EchoHandler(asyncore.dispatcher_with_send):

   def __xinit__(self, sock):
      asyncore.dispatcher_with_send.__init__(self)
      self.buffer = ""
      
   def handle_read(self):
       data = self.recv(8192)
       if data:
          #self.buffer.append(data)
          self.send(data)
          
   def handle_close(self):
      print 'Close connection'
      self.close()

   def xxwritable(self):
      return (len(self.buffer) > 0)

   def xxhandle_write(self):
      sent = self.send(self.buffer)
      self.buffer = self.buffer[sent:]

class EchoServer(asyncore.dispatcher):
   def __init__(self, host, port):
      asyncore.dispatcher.__init__(self)
      self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
      self.set_reuse_addr()
      self.bind((host, port))
      self.listen(5)

   def handle_accept(self):
      pair = self.accept()
      if pair is not None:
         sock, addr = pair
         print 'Incoming connection from %s' % repr(addr)
         handler = EchoHandler(sock)


server = EchoServer("0.0.0.0", 8080)
asyncore.loop()
