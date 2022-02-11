
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
conns = {}

groups={}
#vals = {}

# a connection can have a list of vars it is interested in
class Conn:
    def __init__ (self,conn, addr):
        self.addr = addr
        self.conn=conn
        self.lists ={}
        self.type="string"

    def addVarToList(self, var, lname):
        if not lname in self.lists.keys():
            self.lists[lname]=[]
        if not var in self.lists[lname]:
            self.lists[lname].append(var)

    def getLvars(self , lname):
        global output_queues
        global outputs
        if lname in self.lists.keys():
            for v in  self.lists[lname]:
                msg = v.name+"->"  + v.getValue() + "\n"  
                output_queues[self.conn].put(msg)
        if not self.conn in outputs:
            outputs.append(self.conn)
                
# a var has a name and handles get and set 
# the set also forwards chages to other processes that have issued an rget

class Var:
    def __init__ (self,group,name,value=""):
        self.name = name
        self.value=value
        self.notify =[]
        self.type="string"
        self.group = group
        
    def setValue(self,value):
        global output_queues
        global outputs
        if self.value != value:
            msg = "var->"+self.name+" value changed from " + self.value + " to "+ value + "\n"
            self.value = value
            for conn in self.notify:
                output_queues[conn].put(msg)
                if not conn in outputs:
                    outputs.append(conn)

    def getValue(self):
        return self.value

    def addNotify(self, conn):
        if conn:
            if not conn in self.notify:
                self.notify.append(conn)

    def getName(self):
        return self.name
    
                
def getGroup(dataw):
    dgroups=dataw[1].split(":")
    #print dgroups
    if len(dgroups) >= 1 :
        dgroup = dgroups[0]
    else:
        dgroup = "default"
    #print "dgroup ["+ dgroup + "]"
    return dgroup

def getGvals(dgroup):
    global groups
    if not dgroup in groups.keys():
        groups[dgroup] = {}
    gvals = groups[dgroup]
    return gvals

# data format [group:]name=value
def cSaveData(dataw):
    if dataw[0] == "set":
        dgroup = getGroup(dataw)
        print "set " + dgroup 
        gvals = getGvals(dgroup)
        if not dataw[1] in gvals.keys():
            gvals[dataw[1]] = Var(dgroup, dataw[1],dataw[2])
        else:
            gvals[dataw[1]].setValue(dataw[2])

def cGetData(dataw):
    op = ""
    if dataw[0] == "get":
        print len(dataw) 
        dgroup = getGroup(dataw)
        gvals = getGvals(dgroup)
        op =""
        if (len(dataw) == 2) and (dgroup==dataw[1]):
            for dat in gvals.keys():
                op = op + "get:: " + gvals[dat].getName() + "-->"+ gvals[dat].getValue() + "\n"
        else:
            if dataw[1] in gvals.keys():
                op = "get:: " + gvals[dataw[1]].getName() + "-->"+ gvals[dataw[1]].getValue() + "\n"
            else:
                op = dataw[1] + " not found"
    return op

def crGetData(conn, dataw):
    op = ""
    if dataw[0] == "rget":
        dgroup = getGroup(dataw)
        gvals = getGvals(dgroup)
        if dataw[1] in gvals.keys():
            op = "rget:: " + gvals[dataw[1]].getValue() + "\n"
        else:
            gvals[dataw[1]] = Var(dgroup, dataw[1])
            op = "rget:: "+ dataw[1] + "new var created\n"
        gvals[dataw[1]].addNotify(conn)
    return op


def cmdProcess(conn, data):
    global groups
    op = " no command\n"
    dataw = data.split()
    if len(dataw) < 2:
        if conn:
            output_queues[conn].put(op)
        return
    
    if dataw[0] == "set":
        cSaveData(dataw)
        #vals[dataw[1]]= dataw[2]
        op =  "you sent " + data
    elif dataw[0] == "get":
        op = cGetData(dataw) 
        #op =  "you sent" + data
        if conn:
            output_queues[conn].put(op)
    elif dataw[0] == "rget":
        op = crGetData(conn,dataw) 
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

cmdProcess(0,"set gr1:val11 1234")
cmdProcess(0,"set gr1:val12 23444")
cmdProcess(0,"set gr2:val21 51234")
cmdProcess(0,"set gr2:val23 61234")

while inputs:
    readable, writable, exceptional = select.select(
        inputs, outputs, inputs)
    for s in readable:
        if s is server:
            connection, client_address = s.accept()
            conns[connection] = Conn(connection, client_address)
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
        #print "got s"
        if s in message_queues.keys():
            if message_queues[s].empty():
                pass
            else :
                commandProcess(s)
                outputs.append(s)
            

        
