#!/usr/bin/python3

import socket
import struct
import json
import sys
import threading
import time
import os

import udputils as uu
import udpmenu

#from nodes import gpio

#from Crypto.Cipher import AES
#python3 -m pip install --user pycryptodome
#pycryptodome.readthedocs.io/en/latest/examples.html

#cur_mtime = lambda: int(round(time.time() * 1000))
#cur_utime = lambda: int(round(time.time() * 1000000))

hostname = None

dosend = 0

MCAST_GRP = '224.1.1.1'
MCAST_CPORT = 5007
MCAST_RPORT = 5008

tstart = uu.cur_utime()
tmstart = uu.cur_mtime()
tmbase = 1579915173470
subHost=None
subNode=None

# we will copy all our setup into a dict so we can share with nodes
#
gDict ={}


# regarding socket.IP_MULTICAST_TTL
# ---------------------------------
# for all packets sent, after two hops on the network the packet will not 
# be re-sent/broadcast (see https://www.tldp.org/HOWTO/Multicast-HOWTO-6.html)
MULTICAST_TTL = 2

def runSubFcn(hname, dat):
    print ("subFcn running host ", hname, " data ", dat)

def readRsock(rsock, data, stuff):
    return uu.readRsock(rsock, data, stuff)

def readCsock(rsock, data, stuff):
    return uu.readCsock(rsock, data, stuff)

g_timeout = 0

def runRsock(rsock, done):
    global g_timeout
    tval = 0.5

    decode = True
    try:
        data = rsock.recv(10240)
    except socket.timeout:
        g_timeout += 1
        print (" timeout no data")
        if subNode == None and g_timeout > 3 :
            done = True
            print (" timeout : quitting")
        decode = False
    except:
        print(" recv error")
        decode = False
        
    if decode:
        rsock.settimeout(tval)
        tdone = cur_utime() - tstart
        
        print(" trip time ", tdone , "uS")
        print (data)

        try:
            jc = json.loads(data.decode('utf8'))
        except:
            print(" data decode error ignoring")
            decode = False
        if decode:
            #{"hostname":"pi4","ack":1035432}
            if "ackhost" in jc.keys():
                id = jc["ack"]
                hname = jc["ackhost"]
                confirmAck(id, hname)
                print(" got ack")
                
            if "hostname" in jc.keys():
                print ( "src host =" , jc["hostname"], "subHost = ", subHost)
                try:
                    dat = jc["data"]
                except:
                    print ("no data found:",data)
                    decode = False
            if decode:
                print ("data found:",dat)
                #jd = json.loads(dat)
                #print (" jd decoded")
                if type(dat) == type({}):
                    x = dat.keys()
                    print (" dat keys found")
                    for d in dat.keys():
                        print ("d found :",d)
                        dd = dat[d]
                        print ("dd found :",dd)
                        for di in dd.keys():
                            print("di :" ,di , " val :", dd[di])
                    if subNode in dat.keys():
                        print ("found subnode ", subNode)
                        runSubFcn(subHost,dat[subNode])
                    


    return done



#todo add this to the menu poll loop
def rport_thread(rport):
    global tstart
    global tmstart
    global subHost
    done = False

    rsock = uu.createRsock(MCAST_GRP, rport)
    tval = 5.0
    rsock.settimeout(tval)
    while not done:
        done = runRsock(rsock, done)



def setSubs(hname, node):
    global subHost
    global subNode
    subHost = hname
    subNode = node

import importlib

def loadModule(path,module):
    cwd = os.getcwd()
    mod = cwd + '/' + path
    if mod not in sys.path:
        sys.path.append(mod)
    m = importlib.import_module(module)
    return m

def setSubsFcn(hname, node, fcn):
    global g_subs
    global gDict
    if hname == None:
        hname = "*"
    if node not in g_subs.keys():
        g_subs[node] = {}
    gs = g_subs[node]
    if hname not in gs.keys():
        gs[hname] = {}
    gn = gs[hname]
    if node not in gn.keys():
        gn[node] = {}
    gf = gn[node]
    try:
        m = loadModule("nodes", fcn)
    except:
        print ( "unable to import node fcn")
        return
    if "fcn" not in gf.keys():
        m.init(node, gDict, uu)
        gf["fcn"] = m.run
        print ( "node fcn set up")
    else:
        print ( "node fcn already set up")
    
def runSubs(argv, data, dest):
    if type(argv) == type("string"):
        argv = uu.mysplit(argv)
    print(" running subs host node command " )
    if len(argv) < 3:
        print(argv[0] + " [host] node fcn")
        return
    else:
        if len(argv) < 4:
            node = argv[1]
            fcn = argv[2]
            msg = '{"get":["'+node+'"]}'
            setSubsFcn(None, node , fcn)
        else:
            hname = argv[1]
            node = argv[2]
            fcn = argv[3]
            msg = '{"hostname":"'+hname+'","get":["'+node+'"]}'
            setSubsFcn(hname, node , fcn)
        print (msg)
    
def sendNode(hname,node):
    msg = '{"node":{"'+hname+'":"'+node+'"}}'
    print (msg)
    sockSend(msg)

def runSet(argv, data, dest):
    hname = "*"
    if type(argv) == type("string"):
        argv = uu.mysplit(argv)
    if len(argv) < 4:
        print(" not enough args for set command")
        return
    id = str(uu.getAckId())

    if len(argv) == 4:
        node = argv[1]
        item = argv[2]
        val = argv[3]
        print ( "argv Len :", len(argv))
        val = uu.setEsc(val)

        msg = "Setting [" +item +"] value to ["+val+"]"
        uu.sendMsgNl(dest,msg)
        # todo foreach node
        # todo foreach item
        
        msg = '{"hostname":"*", "id":"'+id+'","set":[{"'+node+'":{"'+item+'":"'+val+'"}}]}'
    else:
        hname = argv[1]
        node = argv[2]
        item = argv[3]
        val = argv[4]
        val = uu.setEsc(val)
        msg = "Setting [" +item +"] value to ["+val+"] on host ["+hname+"]"
        uu.sendMsgNl(dest,msg)
        msg = '{"hostname":"'+hname+'","id":"'+id+'","set":[{"'+node+'":{"'+item+'":"'+val+'"}}]}'

    #print (msg)

    registerAck(id, hname, msg)
    sockSend(msg)


def setupGroup(l,hname):
    uu.setupGroup(l , hname)
        
# this is where we register the ack
# id should be unique
# hname can be an individual host or a group name
# if an individual we'll create a group anyway
# a group will have a name and a dict of members.

def registerAck(id, hname, msg):
    uu.registerAck(id, hname, msg)

def addHnameToGroups(gn,hname):
    uu.addHnameToGroups(gn,hname)

def findAckKey(id, hname):
    return uu.findAckKey(id, hname)

def findAckHname(l, hname):
    return uu.findAckHname(l, hname)

# confirm ack is used to register the fact we have received an ack
# from this host the id tells us which group to look at
#
def confirmAck(id, hname):
    uu. confirmAck(id, hname)
    
# helper function to show acks
def jsonAcks(a,b,dest):
    global g_acks
    msg = json.dumps(g_acks)
    uu.sendMsgNl(dest, msg )


def showAcks(a,b,dest):
    uu. showAcks(a,b,dest)
    

ACKRETRY = 3
                

def showGroups(a ,b, dest):
    uu.showGroups(a ,b, dest)

def testConf(a, b, dest):
    if len(a) < 3:
        msg = " need id , hname\n" 
        udpmenu.sendMsg(dest, msg)
        return
    confirmAck(a[1], a[2])

# needs hname plus id
#registerack id ,name , msg
def testReg(a, b, dest):
    if len(a) < 4:
        msg = " need id , name , msg\n" 
        udpmenu.sendMsg(dest, msg)
        return
    registerAck(a[1] ,a[2] ,a[3])

def testDict(a, b, dest):
    global gDict
    print (" local gDict******************")
    print (gDict)
    #msg = json.dumps(gDict)
    #uu.sendMsgNl(dest, msg )
    gd = uu.getDict()
    print (" utils gDict******************")
    print (gd)
    #msg = json.dumps(gd)
    #uu.sendMsgNl(dest, msg )

def testAcks(a, b, dest):
    for x in range(1,10):
        id = uu.getAckId()
        id += x
        hname = "somehost"
        node = "somenode"
        msg = '{"hostname":"'+hname+'","id":'+str(id)+',"get":["'+node+'"]}'
        registerAck(id, hname, msg )
        uu.sendMsgNl(dest, " at start" )

    showAcks(a, b, dest)
    for x in range(1,10):
        msg = " start run  " + str(x) 
        uu.sendMsgNl(dest,msg)
        uu.scanAcks(1)
        msg = " after " + str(x) + "runs"
        uu.sendMsgNl(dest,msg)

        showAcks(a, b, dest)
    uu.sendMsgNl(dest, " groups found" )
    showGroups(a, b, dest)


#runSet            
def runGet(argv,data,dest):
    global dosend
    global tmstart
    global tmbase
    if type(argv) == type("string"):
        argv = uu.mysplit(argv)

    print(" Len args :",len(argv))
    if len(argv) < 2:
        return
    
    if len(argv) == 3 and argv[2] == "config":
        node = argv[2]
        msg = '{"get":["'+node+'"]}'
    elif len(argv) == 2:
        node = argv[1]
        #item = argv[3]
        msg = '{"get":["'+node+'"]}'
    else:
        hname = argv[1]
        node = argv[2]
        id = uu.getAckId()
        msg = '{"hostname":"'+hname+'","id":'+str(id)+',"get":["'+node+'"]}'
        registerAck(id, hname, msg)
    uu.sendMsgNl(dest, msg )
    #dosend = 1
    #x = threading.Thread(target=rport_thread, args=(MCAST_RPORT,))
    #x.start()
    #tstart = cur_utime()
    sockSend(msg)
    #x.join()

    return msg
        
def runUpdate(argv, data, dest):
    global hostname
    if type(argv) == type("string"):
        argv = uu.mysplit(argv)

    print(" update args len :", len(argv)) 
    if len(argv) == 1:
        msg = '{"sender":"'+hostname+'","cmd":"update"}'
    else:
        host = argv[1]
        msg = '{"sender":"'+hostname+'","hostname":"'+host+'","cmd":"update"}'
    print (msg)
    sockSend(msg)

def runNode(argv, data, dest):
    global hostname
    if type(argv) == type("string"):
        argv = uu.mysplit(argv)

    #print(" node args len :", len(argv))
    if len(argv) < 3:
        print(" node host node")
    else:
        host = argv[1]
        node = argv[2]
        #print(" host  :", host) 
        #print(" node  :", node) 

        sendNode(host, node)
        
def runDump(argv, data, dest):
    global hostname
    if type(argv) == type("string"):
        argv = uu.mysplit(argv)
    print(" running dump command")
    if len(argv) < 1:
        print(" dump [nost]")
        return
    
    else:
        if len(argv) > 1:
            hname = argv[1]
            msg = '{"hostname":"'+hname+'","dump":"all"}'
        else:
            msg = '{"dump":"all"}'
        print (msg)
        sockSend(msg)

        
def sockSend(msg):
    uu.sockSend(msg, MCAST_GRP, MCAST_CPORT)
#    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
#    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
#    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#    sock.sendto(bytes(msg,'utf8'), (MCAST_GRP, MCAST_CPORT))


if __name__ == "__main__":
    #global hostname
    hh = os.uname()
    tmstart = uu.cur_mtime()
    gDict = uu.setupgDict()
    udpmenu.setDict(gDict)    
    #g_acks = {}
    g_groups = {}
    g_tlist  = gDict["test_list"] 
    g_tlist.append(1)
    g_tlist.append(21)

    uu.showgDict(gDict)
    

    #print(hh)
    hostname = hh[1]
    uu.setHostName(hostname)
    if len(sys.argv) == 1:

        udpmenu.addMenuItem("td", "testD", " test Dict", testDict, sys.stdout)
        udpmenu.addMenuItem("ta", "testA", " test Acks", testAcks, sys.stdout)
        udpmenu.addMenuItem("a", "showA", " show Acks", showAcks, sys.stdout)
        udpmenu.addMenuItem("gr", "showG", " show Groups", showGroups, sys.stdout)
        udpmenu.addMenuItem("j", "jacts", "json acks", jsonAcks, sys.stdout)
        udpmenu.addMenuItem("ra", "reg", "register ack", testReg, sys.stdout)
        udpmenu.addMenuItem("ca", "conf", "confirm ack", testConf, sys.stdout)
        udpmenu.addMenuItem("s", "set", "set [host] node item val", runSet, None)
        udpmenu.addMenuItem("g", "get", "get [host] node ", runGet, None)
        udpmenu.addMenuItem("u", "update", " update nodes ", runUpdate, None)
        udpmenu.addMenuItem("n", "node", " add a node to a host ", runNode, None)
        udpmenu.addMenuItem("d", "dump", " dump [host] config[s]", runDump, None)
        udpmenu.addMenuItem("sf", "subs", " setup sub fcn", runSubs, None)

        rsock = uu.createRsock(MCAST_GRP, MCAST_RPORT)
        udpmenu.addRsock(rsock,readRsock,None)
        ssock = uu.createRsock(MCAST_GRP, MCAST_CPORT)
        udpmenu.addRsock(ssock,readCsock,None)
        
        #runMenuInput("quit the menu", sys.stdout)
        udpmenu.runMenu(sys.stdin)

        print("bye  : quit",   udpmenu.udpMenuCtl["quit"] , "\n")

        #msg = '{"data":[{"1234":{"item1":"value1","item2":"value2"}}]}'
        #print (msg)
        #jcb = bytes(msg,'utf8')
        #jc = json.loads(jcb.decode('utf8'))
        #sockSend(msg)

        # this version sends a single command
        # todo decode args 'cmd' 'node' 'item' 'value'
        # cmd set - set node item value 
        # cmd get - get node item
        #     the get will send a "get node item " message and
        #     wait for the next update from the node showing the value
        # no command sends the default data
        #
    else:
        cmd = sys.argv[1]
        #print ("command = ", cmd)
        if cmd == "node":
            print(" running node command")
            if len(sys.argv) < 4:
                print(" not enough args for node command")
                sys.exit(-1)
            host = sys.argv[2]
            node = sys.argv[3]
            sendNode(host,node)
# not done yet            
        if cmd == "load":
            print(" running load command")
            if len(sys.argv) < 2:
                print(" not enough args for load command")
                sys.exit(-1)
            else:
                if len(sys.argv) > 3:
                    hname = sys.argv[2]
                    msg = '{"hostname":"'+hname+'","load":"all"}'
                else:
                    msg = '{"load":"all"}'
                print (msg)
                sockSend(msg)

        if cmd == "update":
            print(" running update command")
            node = None
            if len(sys.argv) > 2:
                node = sys.argv[2]
            runUpdate(sys.argv, None, sys.stdout)

        if cmd == "set":
            print(" running set command")
            runSet(sys.argv, None, sys.stdout)


        dosend = 0
# this is a sort of subscribe
# the sub function will be run

        if cmd == "subs":
            print(" running subs host node command")
            if len(sys.argv) < 3:
                print(" not enough args for subs command")
                sys.exit(-1)
            else:
                if len(sys.argv) < 4:
                    node = sys.argv[2]
                    msg = '{"get":["'+node+'"]}'
                    setSubs(None, node)
                else:
                    hname = sys.argv[2]
                    node = sys.argv[3]
                    msg = '{"hostname":"'+hname+'","get":["'+node+'"]}'
                    setSubs(hname, node)
                print (msg)
                dosend = 1
#ok            
        if cmd == "dump":
            print(" running dump command")
            if len(sys.argv) < 2:
                print(" not enough args for dump command")
                sys.exit(-1)
            else:
                if len(sys.argv) > 3:
                    hname = sys.argv[2]
                    msg = '{"hostname":"'+hname+'","dump":"all"}'
                else:
                    msg = '{"dump":"all"}'
                    print (msg)
                    #sockSend(msg)
                dosend = 1
#ok                
        if cmd == "group":
            print(" running group command")
            if len(sys.argv) < 3:
                print(" not enough args for group command")
                sys.exit(-1)
            else:
                if len(sys.argv) > 3:
                    hname = sys.argv[2]
                    gname = sys.argv[3]
                    msg = '{"hostname":"'+hname+'","cmd":"group","'+hname+'":"'+gname+'"}'
                    print (msg)
                    #sockSend(msg)
                    dosend = 1
                else:
                    msg = '{"dump":"all"}'
                
#ok
        if cmd == "get":
            print(" running get command")
            if len(sys.argv) < 3:
                print(" not enough args for get command")
                sys.exit(-1)
            msg = runGet(sys.argv, None, sys.stdout)
            dosend  = 0
        
        if dosend > 0:
            #get command requires a RPORT to be set up and a listening thread started
            x = threading.Thread(target=rport_thread, args=(MCAST_RPORT,))
            x.start()
            tstart = cur_utime()
            sockSend(msg)
            x.join()
            #runSet(sys.argv, None, None)

            #showAcks(None, None, sys.stdout)
            #testAcks(None, None, sys.stdout)
