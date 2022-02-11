#!/usr/bin/python3

import socket
import struct
import json
import sys
import ctypes
import os
import udputils as uu

gDict = {}

#
# overview
# we have one or more listening channels each channel has a group address a listening port
# and possibly a response port
#MCAST_GRP = '224.1.1.1'
#MCAST_PORT = 5007
# each channel has a command set and n input group (list of host names)
# the hostname field , if present , must match the input group for the channel
# the command set looks for the cmd field 
# data nodes on a listening channel may have on or more actions responding to set commands for
# data items in the node
# each channel will have one or more data nodes and a config node
# an incoming cmd must have a qualifying hostname and must refer to an established node.
# the node command (node host node) will add a node to a host data set
# TODO add id to message send
# create hostgrp list
#
# new
#{
#   "data": [{
#       "1234":{
#	        "item1":"value1",
#               "item2":"value2",
#               "item3":"value3",
#               "item4":"value4"
#              }
#	   }
#          ]
#}

# test with
# ./udpdata.py <hostname> [nodes]
# on one terminal
#
# then on another terminal
#
# ./udpsend.py host <hostname> <groupname> # allocate a group to this host
# ./udpsend.py get config                  # see who's playing
# ./udpsend.py set 1234 item22 "some value 224"
# ./udpsend.py get 1234 

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 5007
MCAST_SPORT = 5008
IS_ALL_GROUPS = True
IS_ALL_GROUPS = False

hostname = 'Test'
dumpFile = 'Test.json'

MY_NODES = ["1234","1236","abdc"]

# this is the data repo really a list of nodes
#myData = []
#dict1 = {}
#dict2 = {}
#dict1["1234"] = {"item1":"value1","item2":"value2"}
#dict2["abcd"] = {"item1a":"value1a","item2a":"value2a"}
#myData.append(dict1)
#myData.append(dict2)
#print(myData)

#myData = []


class nData:
    def __init__(self):
        # md is the direct data array
        # rd is the reference list used to hold references
        # refs is the ref dict to hold values
        # the rds contain refs
        # many items can point to the same rref
        # each node is a dict of name value pairs

        # md is a dict of nodes
        self.md = {}
        # rd is a dict of indirect nodes ( using refs)
        self.rd = {}
        # refs dict of name value pairs
        self.refs = {}
        # cfg dict of name value pairs
        self.cfg = {}
        self.hostgrp = []
        
    def addNode(self, node):
        d = {}
        self.md[node] = d
        return d
    
    def addRefNode(self, node):
        d = {}
        self.rd[node] = d
        return d

    def setConfigItem(self, item, value):
        self.cfg[item]=value

    def getConfigItem(self, item):
        if item in self.cfg.keys():
            return self.cfg[item]
        return None
    
    def findNode(self, node):
        if node in self.md.keys():
            return self.md[node]
        return None

    def findRefNode(self, node):
        if node in self.rd.keys():
            return self.rd[node]
        return None
    

    # this sets a value in the ref entry
    # lets see if that is matched in the ref entry
    # if so it will also set the ref val
    def setNodeItemRef(self, node, item, ref, val):
        #rref = node +':'+item
        #d = self.findNode(node)
        r = self.findRefNode(node)
        if not r:
            #    d = self.addNode(node)
            r = self.addRefNode(node)
            print (" created ref node ", node)
        else:
            print (" found ref node ", node)
        if r:
            rl = r[node]
            #if item in rl.keys():
            #    rl[item] = ref
            #    self.refs[ref] = val
            #    return
            rl[item] = ref
            self.refs[ref] = val

    # this sets a value in the direct entry
    # lets see if that is matched in the ref entry
    # if so it will also set the ref val
    def setNodeItemVal(self, node, item, val):
        print(" set node item val node", node, " item ", item, " val ", val)

        #rref = node +':'+item
        d = self.findNode(node)
        #r = self.findRefNode(node)
        if not d:
            print(" added direct node", node)
            self.addNode(node)
            d = self.findNode(node)
            print("this is d", d)
            d[item]=val
        else:
            print(" node " + node +" found here it is")
            print(d)
            #r = self.addRefNode(node)
        # get the dict associated with the node
        l = self.md[node]
        l[item] = val
        print("set val this is l again  2", l)
        # we have found the direct data node 
        # now need to see if this is a ref node and set that value too
        #if not l:
        #    print (" created node dict for", node)
        #    l = {}
        #    d[node] = l
        r = self.findRefNode(node)
        if r:
            print (" found ref node ", node)
            if item in r.keys():
                rref = r[item]
                self.refs[rref] = val
        else:
            print (" no ref node ", node)
        return

    # this will create a rref item    
    def setRefVal(self, rref, val):
        self.refs[rref] = val
        
    # do not create anything
    #def setVal(self, node, item, val):
    #    d = self.findNode(node)
    #    if d:
    #        l = d[node]
    #        if item in l.keys():
    #            l[item] = val
            
    # this sets a value in the rd entry
    # if the node item is found it sets the md value ( redundant)
    # it also sets up an rd entry as the rref
    # it also sets the value in the rref dict
    # if a value is found it only sets the rref entry
    # when we get md  the rref entry will be transferred to the md struct 
    #def setRefNodeItemVal(self, node, item, rref, val):
    #    dr = self.findRefNode(node)
    #    if not dr:
    #        dr = self.addRefNode(node)
    #        dl= self.addNode(node)
    #    lr = dr[node]
    #    ll = dl[node]
    #    if item in lr.keys():
    #        lr[item] = rref
    #        self.refs[rref] = val 
    #        return
    #    ll[item] = val
    #    lr[item] = rref
    #    self.refs[rref] = val 
  
    def getJson(self):
        # transfer the vals from the refs
        # self.getRefVals()  
        jc =json.dumps(self.md)
        return jc

    def getRefVals(self):
        for node in self.rd.keys():
            dn = self.rd[node]
            for item in dn.keys():
                rval = dn[item]
                val = self.refs[rval]
                print(" node:", node, " item", item, " rval", rval, " val", val)
                self.setNodeItemVal(node, item, val)

    def getConfigJson(self):
        jc =json.dumps(self.cfg)
        return jc

    def getDirectJson(self):
        # transfer the vals from the refs
        # self.getRefVals()  
        jc =json.dumps(self.md)
        return jc

    def getRefJson(self):
        # transfer the vals from the refs
        # self.getRefVals()  
        jc =json.dumps(self.rd)
        return jc

    def getRefDefsJson(self):
        jc =json.dumps(self.refs)
        return jc

    def selfDump(self):
        jj = {}
        jj["config"] = self.cfg
        jj["nodes"] = self.md
        jj["ref"]    = self.rd
        jj["defs"]   = self.refs
        return json.dumps(jj)

    def selfLoad(self, jdata):
        jj = json.loads(jdata)
        self.cfg = jj["config"]
        self.md = jj["nodes"]
        self.rd = jj["ref"]   
        self.refs = jj["defs"]
        
    
def testnData():
    print('testnData start\n')
    nd = nData()
    nd.addNode("node1")
    nd.addNode("node2")
    # this simply sets an item value
    # if the node:item is new  
    # a default ref is set up as node:item and the ref value is set
    nd.setNodeItemVal("node3","item3","val3")
    nd.setNodeItemVal("node3","item1",1)
    # this will change the ref value
    #nd.setRefVal("node3:item3","rval3")
    nd.setNodeItemVal("node4", "item4","oldval")
    msg = nd.getDirectJson()
    print('testnData Direct json 1', msg)
    #make node4 item4 follow the value of fooref
    nd.setNodeItemRef("node4", "item4", "fooref" ,"newval")
    #make node3 item3 follow the value of fooref
    nd.setNodeItemRef("node3", "item3", "fooref" ,"newval")

    msg = nd.getDirectJson()
    print('testnData Direct json 2', msg)

    msg = nd.getRefDefsJson()
    print('testnData ref defs json', msg)
    msg = nd.getRefJson()
    print('testnData Ref json 2', msg)

    msg = nd.getDirectJson()
    print('testnData Direct json x1', msg)
    print('testnData getrefvals')
    nd.getRefVals()
    msg = nd.getDirectJson()
    print('testnData Direct json x2', msg)

    print('testnData done\n')
    foo1 = packMsg("this is a message",4567)
    foo2 = packMsg("this is another message",4568)
    foo = foo1+foo2
    print(" packed foo =",foo)
    mlen, mtime, msg, msgr  = unPackMsg(foo)
    print(" msgr =",msgr)
    mlen, mtime, msg, msgr  = unPackMsg(msgr)
    print(" msg 2 =",msg)
    print(" time 2 =",mtime)
    print(" msgr =",msgr)

def packMsg(msg, ti):
    mbytes = bytes(msg,'utf8')
    mreq = struct.pack('<QQ'+str(len(mbytes))+'sQ', len(mbytes)+24, ti, mbytes, ti)
    return mreq

# returns len, time, msg , remainder 
def unPackMsg(msg):
    #mbytes = bytes(msg,'utf8')
    mlen = struct.unpack('<Q', msg[0:8])[0]
    mtime = struct.unpack('<Q', msg[8:16])[0]
    mtime2 = struct.unpack('<Q', msg[mlen-8:mlen])[0]
    msgb = msg[16:mlen-8]
    msgrem = msg[mlen:]
    msgs = "".join(chr(x) for x in bytearray(msgb))
    print(" len :", str(mlen))
    print(" time :", str(mtime))
    print(" time 2:", str(mtime2))
    print(" msg :", msgs)
    #print(" msg :", msg[16:mlen-16])
 

    return mlen, mtime, msgs, msgrem

# rsock is the receive  socket 
rsock = uu.getUSock(MCAST_GRP)
ssock = uu.getUSock(MCAST_GRP)

if IS_ALL_GROUPS:
    # on this port, receives ALL multicast groups
    rsock.bind(('', MCAST_PORT))
    #ssock.bind(('', MCAST_SPORT))
else:
    # on this port, listen ONLY to MCAST_GRP
    rsock.bind((MCAST_GRP, MCAST_PORT))



def sockSend(msg, sock):
    if(sock == None):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    sock.sendto(bytes(msg,'utf8'), (MCAST_GRP, MCAST_SPORT))
    
def testHostName(nd, jc):
    global hostname
    #global hostgrp
    ignore = 0
    if "hostname" in jc.keys() :
        hname = jc["hostname"]
        print ("looking for ", hname ,"in", nd.hostgrp)

        if hname != hostname:
            print ('incorrect host:',hname,'  cmd ignored')
            #print (nd.hostgrp)
            ignore = 1
        if nd :
            if hname in nd.hostgrp:
                print ('correct host:',hname,'  cmd accepted')
                ignore = 0
        if hname == "*":
            print ('default host:',hname,'  cmd accepted')
            ignore = 0
    if ignore == 0:
        # send id 
        if "id" in jc.keys():
            id = jc["id"]
            msg = '{"ackhost":"'+hostname+'","ack":'+str(id)+'}'
            sockSend(msg, ssock)

    return ignore

def testHostRun(nd, jc ,fcn):
    ignore = testHostName(nd,jc)
    if ignore == 0:
        fcn(nd,jc)
        
#b'{"data":[{"1234":{"item1":"value1"}}]}'
#{'1234': {'item1': 'value1'}}

def dumpNd(nd):
    global hostname
    global dumpFile
    piddir = '/tmp/udpdata'
    dumpFile = piddir + '/'+hostname+'.json'
    cmd = 'sudo mkdir -p ' + piddir
    rval =  os.system(cmd)
    cmd = 'sudo chmod a+rw ' + piddir
    rval =  os.system(cmd)
    msg = nd.selfDump()    
    f = open(dumpFile, 'w')
    f.write(msg)
    f.close()

def loadNd(nd, jc):
    global hostname
    global dumpFile
    piddir = '/tmp/udpdata'
    dumpFile = piddir + '/'+hostname+'.json'
    cmd = 'sudo mkdir -p ' + piddir
    rval =  os.system(cmd)
    cmd = 'sudo chmod a+rw ' + piddir
    rval =  os.system(cmd)
    #msg = nd.selfDump()    
    f = open(dumpFile, 'r')
    msg = f.read()
    nd2 = nData()
    nd2.selfLoad(msg)
    nd2
    
    
#def dumpServer():
#    global hostname
#    piddir = '/etc/udpdata'
#    pidfile = piddir + '/'+hostname+'.json'
#    cmd = 'sudo mkdir -p ' + piddir
#    rval =  os.system(cmd)
#    cmd = 'sudo chmod a+rw ' + piddir
#    rval =  os.system(cmd)
#    msg = json.dumps(myData)    
#    f = open(pidfile, 'w')
#    f.write(msg)
#    f.close()

def isNum(sss):
    try:
        v = int(sss)
        return v, True
    except ValueError:
        try:
            v = float(sss)
            return v, True
        except ValueError:
            return sss, False
    return sss, False
        
def updateHost(nd, jc):
    global hostname
    ignore = testHostName(nd,jc)
    if ignore == 0:
        print (" running update host .." , hostname)
        pid = os.getpid();
        print (" running update pid .." , pid)
        pwd = os.getcwd()
        os.system("git pull")
        os.system(pwd+"/udpdata.py "+ hostname +"&")
        os.system(" kill " + str(pid))
    else:
        print (" NOT running update host .." , hostname)

def dumpHost(nd, jc):
    ignore = testHostName(nd,jc)
    if ignore == 0:
        #dumpServer()
        dumpNd(nd)
        nnmsg = nd.selfDump()
        msg = '{"hostname":"'+hostname+'","msg":['+nnmsg+']}'
        sockSend(msg, ssock)

def groupHost(nd,jc):
    ignore = testHostName(nd,jc)
    if ignore == 0:
        for h in jc.keys():
            if h == hostname:
                node = jc[h]
                if node not in nd.hostgrp:
                    nd.hostgrp.append(node)
                    print('adding group ', node)
                    print('hostgrp ', nd.hostgrp)
                else:
                    print('Not adding group ', node)

def setHostNode(nd, jc):
    jcd = jc["node"]
    for h in jcd.keys():
        if h == hostname:
            node = jcd[h]
            mnode = nd.findNode(node)
            if not mnode:
                print('adding node ', node)
                nds = nd.getConfigItem("nodes")
                nds.append(node)
                print (nds)
                nd.addNode(node)
            else:
                print('Not adding node ', node)
                #msg = '{"data":[{"1234":{"item1":"value1","item2":"value2"}}]}'
                #mreq = struct.pack('<Q'+str(len(msg))+'s', len(msg), bytes(msg,'utf8'))
                #munp = struct.unpack('<Q', mreq[0:8])
                #print(mreq)
                #print(len(msg))
                #print(munp[0])

#{"set":[{"1234":{"v1":"d1"}}]}
# new "set":{"1234":{"v1":"d1"}},"1236":{"v2":"d2"}}}
def runSet(nd, jc):
    jcd = jc["set"]

    print("len jcd :",len(jcd))
    print("type jcd :",type(jcd).__name__)
    # type.__name__ can be list or dict
    if type(jcd).__name__ == "list":
        for j in range(len(jcd)):
            for node in jcd[j].keys():
                nds = nd.getConfigItem("nodes")

                print('node', node)
                if node in nds:
                    for l in jcd[j][node].keys():
                        print('item', jcd[j][node][l])
                        di = jcd[j][node]
                        for item in di.keys():
                            
                            val,t = isNum(di[item])
                            print ( "set node ",node," item", item , "->", val)
                            nd.setNodeItemVal(node, item, val)
                            print(nd)

                else:
                    print (" node", node , " skipped")
                    #print(jc["set"][0])
                        
    if type(jcd).__name__ == "dict":
        for node in jcd.keys():
            nds = nd.getConfigItem("nodes")

            print('node', node)
            if node in nds:
                for l in jcd[j][node].keys():
                    print('item', jcd[j][node][l])
                    di = jcd[j][node]
                    for item in di.keys():
                            
                        val,t = isNum(di[item])
                        print ( "set node ",node," item", item , "->", val)
                        nd.setNodeItemVal(node, item, val)
                        print(nd)

            else:
                print (" node", node , " skipped")
                #print(jc["set"][0])

def runGet(nd,jc):
    print(len(jc["get"]))
    msg = None
    jcd = jc["get"]
    for j in range(len(jcd)):
        node = jcd[j]
        if node == "config":
            nnmsg = json.dumps(nd.cfg)
            print ('nnmsg =', nnmsg)
            msg = '{"hostname":"'+hostname+'","data":['+nnmsg+']}'
        else:    
            print ('get node',jcd[j])
            nn = nd.findNode(node)
            if nn:
                nnmsg = json.dumps(nn)
                print ('nnmsg =', nnmsg)
                msg = '{"hostname":"'+hostname+'","data":{"'+node+'":'+nnmsg+'}}'
            else:
                msg = '{"hostname":"'+hostname+'","no node":['+node+']}'
    if msg:
        print (msg)
        #sendMsg
        sockSend(msg, ssock)

def readSsock(nd, rs):
    data = rs.recv(10240)
    # todo split up multiple data items if needed
    print (data)
    jc = json.loads(data.decode('utf8'))
    if "cmd" in jc.keys():
        print (jc, " cmd :" , jc["cmd"])
        cmd = jc["cmd"]
        if cmd == "update":
            testHostRun(nd, jc ,updateHost)
        elif cmd == "dump":
            testHostRun(nd, jc ,dumpHost)
        elif cmd == "group":
            testHostRun(nd, jc ,groupHost)
                    
    elif "dump" in jc.keys():
        print(" running cmd dump ...")
        dumpHost(nd, jc)

    if "load" in jc.keys():
        testHostRun(nd,jc ,loadNd)
                    
    if "node" in jc.keys():
        setHostNode(nd, jc)

    if "data" in jc.keys() :
        print(len(jc["data"]))
        for j in range(len(jc["data"])):
            for k in jc["data"][j].keys():
                print(k)
                for l in jc["data"][j][k].keys():
                    print(jc["data"][j][k][l])
                    print(jc["data"][0])

    #'{"hostname":"nav1","set":[{"fnode":{"fitem":"23"}}]}'
    if "set" in jc.keys() :
        #runSet(nd,jc)
        testHostRun(nd,jc,runSet)

    if "get" in jc.keys() :
        testHostRun(nd,jc,runGet)
        #runGet(nd,jc)
                          
    if "update" in jc.keys() :
        #updateHost(nd, jc)
        testHostRun(nd,jc,updateHost)

        
def runServer(nd):
    global hostname
    print (" hostname =", hostname)
    piddir = '/var/run/udpdata'
    uu.savePid(piddir, hostname)            

    while True:
        rsock.settimeout(5.0)
        try:
            readSsock(nd, rsock)
                    
        except socket.timeout:
            print (" timeout no data")

        except KeyboardInterrupt:
            print (" process quitting")
            sys.exit(0)

if __name__ == "__main__":
    #testnData()
    nd = nData()
    gDict = uu.setupgDict()
    #hostgrp.append("fum")
    #global hostname
    hh = os.uname()
    hostname = hh[1]
    mynodes = MY_NODES
    
    if len(sys.argv) < 5:
        if len(sys.argv) > 1 :
            hostname = sys.argv[1]
        if len(sys.argv) > 2 :
            nn = sys.argv[2]
            print(hostname)
            print(nn)
            mynodes = nn.split()
            print(mynodes)
            MY_NODES = mynodes
            #sys.exit(0)
            
        nd.setConfigItem("hostname", hostname)
        nd.setConfigItem("nodes", mynodes)
        nd.setConfigItem("pid", os.getpid())
        #nd.setNodeItemVal("config", "hostname", hostname)
        #nd.setNodeItemVal("config", "nodes", MY_NODES)
        runServer(nd)
