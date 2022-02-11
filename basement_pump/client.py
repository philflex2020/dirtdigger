socket client.py

import socket
import sys

HOST, PORT = "localhost", 9999
data = " ".join(sys.argv[1:])

# Create a socket (SOCK_STREAM means a TCP socket)
#3.6 version
#with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
# Create a socket (SOCK_STREAM means a TCP socket)
sock =socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
# Connect to server and send data
sock.connect((HOST, PORT))
sock.sendall(bytes(data + "\n"))

# Receive data from the server and shut down
received = str(sock.recv(1024))
if __name__ == "__main__":
    print("Sent:     {}".format(data))
    print("Received: {}".format(received))
