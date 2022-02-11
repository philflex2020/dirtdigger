

import select, socket, sys, Queue
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.setblocking(0)

server.bind(("0.0.0.0", 5541))
server.listen(5)

inputs = [server]
outputs = []
message_queues = {}
output_queues = {}
groups={}
vals = {}

def getGroup(dataw):
    dgroups=dataw[1].split(":")
    #print dgroups
    if len(dgroups) > 1 :
        dgroup = dgroups[0]
    else:
        dgroup = "default"
    return dgroup

def getGvals(dgroup):
    global groups
    if not dgroup in groups.keys():
        groups[dgroup] = {}
    gvals = groups[dgroup]
    return gvals

# data format [group:]name=value
def cSaveData(dataw):
    if dataw[0] == "put":
        gvals = getGvals(getGroup(dataw))
        gvals[dataw[1]] = dataw[2]

def cGetData(dataw):
    op = ""
    if dataw[0] == "get":
        gvals = getGvals(getGroup(dataw))
        if dataw[1] in gvals.keys():
            op = "got " + gvals[dataw[1]]
        else:
            op = dataw[1] + " not found"
    return op


def cmdProcess(conn, data):
    global groups
    op = " no command"
    dataw = data.split()
    if dataw[0] == "put":
        cSaveData(dataw)
        #vals[dataw[1]]= dataw[2]
        op =  "you sent " + data
    elif dataw[0] == "get":
        op = cGetData(dataw) 
        #op =  "you sent" + data
        if conn:
            output_queues[conn].put(op)
    elif dataw[0] == "groups":
        for key in groups.keys():
            op = "group->"+ key + "\n"
            if conn:
                output_queues[conn].put(op)
    elif dataw[0] == "group":
        gvals = getGvals(dataw[1])
        op = "group->"+ dataw[1] + " variables \n" 
        if(conn):
            output_queues[conn].put(op)
        for key in gvals.keys():
            op = "group->"+ key + "\n"
            if(conn):
                output_queues[conn].put(op)

def commandProcess(conn):
    data = message_queues[conn].get_nowait()
    cmdProcess(conn,data)

cmdProcess(0,"put gr1:val11 1234")
cmdProcess(0,"put gr1:val12 23444")
cmdProcess(0,"put gr2:val21 51234")
cmdProcess(0,"put gr2:val23 61234")

while inputs:
    readable, writable, exceptional = select.select(
        inputs, outputs, inputs)
    for s in readable:
        if s is server:
            connection, client_address = s.accept()
            connection.setblocking(0)
            inputs.append(connection)
            message_queues[connection] = Queue.Queue()
            output_queues[connection] = Queue.Queue()
        else:
            data = s.recv(1024)
            if data:
                message_queues[s].put(data)
                #if s not in outputs:
                #    outputs.append(s)
            else:
                if s in outputs:
                    outputs.remove(s)
                inputs.remove(s)
                s.close()
                del message_queues[s]

    for s in writable:
        try:
            next_msg = output_queues[s].get_nowait()
        except Queue.Empty:
            outputs.remove(s)
        else:
            s.send(next_msg)

    for s in exceptional:
        inputs.remove(s)
        if s in outputs:
            outputs.remove(s)
        s.close()
        del message_queues[s]
        del output_queues[s]

    for s in inputs:
        print "got s"
        if s in message_queues.keys():
            if message_queues[s].empty():
                pass
            else :
                commandProcess(s)
                outputs.append(s)
            

        
