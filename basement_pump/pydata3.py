
# Python Program to act as a generic data server
# any process can connect and add a data item
#  ==>put mygroup:/my/data/item=1234
# any process can connect and get a list of groups
# ==>groups
# any process can connect and get a list of variables in a group
# ==>group mygroup
# any process can connect and get a variable
# ==>get mygroup:/my/data/item
# any process can connect and get a group
# ==>get mygroup:


# a process can request to be notified whenever a variable is changed
# ==>rget mygroup:/my/data/item

# TODO
# a process can request a whole group (mygroup) whenever a variable is changed
# ==>rgroup mygroup:/my/data/item
# a process can set up a list of variables to get
# ==>ladd mylist mygroup1:/my/data/item
# ==>ladd mylist mygroup2:/my/data/item
# ==>lget mylist 
#

import select, socket, sys, Queue, string

import pydata_mod as pm





pm.cmdProcess(0,"set gr1:val11 1234")
pm.cmdProcess(0,"set gr1:val12 23444")
pm.cmdProcess(0,"set gr2:val21 51234")
pm.cmdProcess(0,"set gr2:val23 61234")
pm.cmdProcess(0,"get gr2:val23")
pm.cmdProcess(0,"get gr2:")

pm.runSock("0.0.0.0",5541)

"""
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.setblocking(0)
pm.inputs = [server]

server.bind(("0.0.0.0", 5541))
server.listen(5)

def delSock(s):
    if s in pm.outputs:
        pm.outputs.remove(s)
        pm.inputs.remove(s)
        s.close()
        del pm.message_queues[s]

while pm.inputs:
    readable, writable, exceptional = select.select(
        pm.inputs, pm.outputs, pm.inputs)

    for s in exceptional:
        print "del Sock 1"
        delSock(s)
        if s in readable:
            readable.remove(s)
            
    for s in readable:
        if s is server:
            connection, client_address = s.accept()
            pm.conns[connection] = pm.Conn(connection, client_address)
            connection.setblocking(0)
            pm.inputs.append(connection)
            pm.message_queues[connection] = Queue.Queue()
            pm.output_queues[connection] = Queue.Queue()
        else:
            data = s.recv(1024)
            if data:
                print "data received"
                print data
                datals = string.split(data,"\n")
                for d in datals:
                    print "item"
                    print d
                    pm.message_queues[s].put(d)
                #if s not in outputs:
                #    outputs.append(s)
            else:
                delSock(s)
                #if s in pm.outputs:
                #    pm.outputs.remove(s)
                #pm.inputs.remove(s)
                #s.close()
                #del pm.message_queues[s]

    for s in writable:
        try:
            next_msg = pm.output_queues[s].get_nowait()
        except Queue.Empty:
            pm.outputs.remove(s)
        else:
            s.send(next_msg)

    for s in exceptional:
        print "Close Sock 2"
        pm.inputs.remove(s)
        if s in pm.outputs:
            pm.outputs.remove(s)
        s.close()
        del pm.message_queues[s]
        del pm.output_queues[s]

    for s in pm.inputs:
        #print "got s"
        if s in pm.message_queues.keys():
            if pm.message_queues[s].empty():
                pass
            else :
                pm.commandProcess(s)
                pm.outputs.append(s)
            
"""

        
