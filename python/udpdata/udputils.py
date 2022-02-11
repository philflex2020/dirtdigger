#!/usr/bin/python3

import socket
import struct
import json
import sys
import threading
import time
import os
# the ack story
# when we want to make sure something gets to a destination we'll add an
#  id in the message.
# we'll also record the message together with the id and the destination
# at the sending end
# the receiver (one or more) will detect the id in the message and send out
# an ack.
#It will also record the fact that it has seen the ack and process the data on
# the first arrival of the message only. 
#
# UDP is unreliable so the original message send may be dropped
# one or more receivers may not have got the message.
# the ack response may also have been dropped.
# if after a short period some acks are missing for the message destinations
# the sender will issue a "cack" message with a list of non responsive hosts
# each host mentioned can then issue a "nack" to indicate it did not get the
# original message or an "ack" to indicate that the message has arrived.
# The sender will resend the message in response to any "nacks" and repeat the
# ack cycle up to 5 times.
# We'll use a select timeout of say 1 mS to handle this

# the receiver will register the arrival of the message and send an ack.
# the first time it sees this id from this host it will process the message.
# at all other times it will send back an ack.
#
# groups
# messages can be sent to a group of hosts specified by a group name
# even if we are sending to a single host we'll make a group for it.
# we may not know the members of a group and send a message out to the
# group name
# all hosts in the group may well respond.
# the response will contain the id and the actual host name.
# this way the sender can add the hostname of the responding host to the
# group name, efffectively building up the group from the responses.
# once a group has been set up , the next time it is used all the members of the# group will be added to the registered id.
# This way , when a message is sent , an answer is expected from each
# member of the group.
# the message can be sent up to 5 times . hosts who have already seen the
# message will ack the message but not act on the message.
#

# build up a register of acks
# each ackid has an id, a message, a group name
# then we populate known hosts assiociated with the group name


g_subs={}
g_acks={}
g_groups = {}
g_tlist=[]
g_socks={}

# we will copy all our setup into a dict so we can share with nodes
#
gDict ={}


def run_func(interval, the_func, the_args, iterations = 0):
    if iterations != 1:
        threading.Timer(
            interval,
            run_func, [interval, the_func, (the_args), 0 if iterations == 0 else iterations-1]).start()
    the_func(the_args)
    


def savePid(piddir, hostname):
    
    pidfile = piddir + '/'+hostname+'.pid'
    cmd = 'sudo mkdir -p ' + piddir
    rval =  os.system(cmd)
    cmd = 'sudo chmod a+rw ' + piddir
    rval =  os.system(cmd)
    
    f = open(pidfile, 'w')
    f.write(str(os.getpid()))
    f.close()
    f = open(pidfile, 'r')
    pid = int(f.read())
    f.close()
    print (" pid is " + str(pid))
    return pid

MULTICAST_TTL = 2

cur_mtime = lambda: int(round(time.time() * 1000))
cur_utime = lambda: int(round(time.time() * 1000000))
tstart = cur_utime()
version = '01.001'
tmbase = cur_mtime() -20
hostname = ""
lastid = cur_mtime() - tmbase

def setHostName(hh):
    global hostname
    hostname = hh
    
def getVersion():
    return version

def setDict(dict):
    global gDict
    gDict = dict

def getDict():
    global gDict
    return gDict

def setupgDict():
    global gDict
    gDict["acks"] = g_acks 
    gDict["subs"] = g_subs 
    gDict["groups"] = g_groups
    gDict["socks"] = g_socks
    gDict["test_list"] = g_tlist 
    gDict["tmbase"] = tmbase
    return gDict

def showgDict(gd):
    print(" local dict:")
    print (gDict)
    print(" incoming dict:")
    print (gd)
    
    
def createRsock(mgrp, rport):
    rs = findNewSock(mgrp,rport)
    if rs :
        #rs = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        #rs.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        rs.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        rs.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        mreq = struct.pack("4sl", socket.inet_aton(mgrp), socket.INADDR_ANY)
        rs.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        rs.bind((mgrp, rport))
    return rs

def getUSock(mgrp):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    mreq = struct.pack("4sl", socket.inet_aton(mgrp), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    return sock

    

def testHostName(nd, jc):
    global hostname
    #global hostgrp
    ignore = 0
    if "hostname" in jc.keys() :
        hname = jc["hostname"]

        if hname != hostname:
            print ('incorrect host:',hname,'  cmd ignored need ', hostname)
            #print (nd.hostgrp)
            ignore = 1
        if nd :
            print ("looking for ", hname ,"in", nd.hostgrp)
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
            print ("Csock msg")
            print (msg)
            # 5008
            #sockSend(msg, ssock)
        # now see if we have a subscriber for this message
        # then run it sd will contain one or more nodes
        if "set" in jc.keys():
            sl = jc["set"]
            if type(sl) == type([]):
                print ("Csock msg set list ")
                for sd in sl:
                    print (sd.keys())
            if type(sl) == type({}):
                print ("Csock msg set dict ")
                print (sl.keys())
            
    return ignore

# we look for something in subs
# we must see if it is for this host and send an ack
def readCsock(rsock, data, stuff):
    global tstart
    try:
        data = rsock.recv(10240)
    except:
        print(" data receive error ")
        return -1
    
    tdone = cur_utime() - tstart
        
    print(" Csock trip time ", tdone , "uS")
    print(" Csock data len ", len(data) )
    print (data)
    try:
        jc = json.loads(data.decode('utf8'))
        #{"ackhost":"pi4","ack":1035432}
        # send back to 5008

    except:
        print(" Csock data decode error ignoring")
        return -1
    #try:
    testHostName(None, jc)
    #except:
    #    print(" Csock testhostname  error ignoring")
    #    return -1

def readRsock(rsock, dat, dest, decode=None):
    global tstart
    data = ""
    print(" running readRSock ")

    try:
        data = rsock.recv(10240)
    except:
        print(" data receive error ")
        return -1
    tdone = cur_utime() - tstart
        
    print(" trip time ", tdone , "uS")
    print(" data len ", len(data) )
    print (data)
    if decode:
        ret = decode(rsock,dat,data,dest)
        if ret:
            return data

    try:
        jc = json.loads(data.decode('utf8'))
        #{"ackhost":"pi4","ack":1035432}

    except:
        print(" data decode error ignoring")
        return -1
    
    if "ackhost" in jc.keys():
        id = jc["ack"]
        hname = jc["ackhost"]
        confirmAck(id, hname)
        print(" got ack")
    return data


def findSock(grp, port):
    global g_socks
    if grp not in g_socks.keys():
        g_socks[grp] = {}
    gg = g_socks[grp]
    if port not in gg.keys():
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        gg[port] = s
    else:
        #print ( " findSock already in use ", port) 
        s = gg[port]
    return s

def findNewSock(grp, port):
    global g_socks
    s = None
    if grp not in g_socks.keys():
        g_socks[grp] = {}
    gg = g_socks[grp]
    if port not in gg.keys():
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        gg[port] = s
    else:
        print ( " findNewSock already in use ", port) 
        #s = gg[port]
    return s

        
def sockSend(msg, grp, port):
    global tstart
    sock=findSock(grp,port)
    tstart = cur_utime()
    sock.sendto(bytes(msg,'utf8'), (grp, port))

def sendMsg(dest, msg):
    #print(dest)
    if dest == sys.stdin:
        sys.stdout.write(msg)
        sys.stdout.flush()
    elif dest == sys.stdout:
        sys.stdout.write(msg)
        sys.stdout.flush()
    else:
        dest.send(msg.encode("utf-8"))

def sendMsgNl(dest,msg):
    sendMsg(dest,msg)
    sendMsg(dest,"\n")

def findAckKey(id, hname):
    global gDict
    g_acks = gDict["acks"]
    
    if id in g_acks.keys():
        l = g_acks[id]
    else:
        l = {}
        l['hlist']  = {}
        l['hgroup'] = hname
        l['msg']    = ""
        g_acks[id] = l
    return l


def findAckKey2(id, hname):
    global gDict
    g_acks = gDict["acks"]
    #global g_acks
    if id in g_acks.keys():
        l = g_acks[id]
    else:
        l = {}
        l['hlist']  = {}
        l['hgroup'] = hname
        l['msg']    = ""
        g_acks[id] = l
    return l


def setEsc(sss):
    if sss[0] == '"':
        sss=sss.replace('"','\\"')
    return sss

def getAckId():
    global gDict
    tmbase = gDict["tmbase"]
    id  = cur_mtime() - tmbase
    return id

def findAckHname(l, hname):
    hl = l['hlist']
    if hname in hl.keys():
        d = hl[hname]
    else:
        d = {}
        hl[hname] = d
        d['host'] = hname    
        d['acnt'] = 1
        d['msg'] = ""
    return d

# confirm ack is used to register the fact we have received an ack
# from this host the id tells us which group to look at
#
def confirmAck(id, hname):
    global gDict
    g_groups = gDict["groups"]
    g_acks = gDict["acks"]
    tmbase = gDict["tmbase"]

    l = findAckKey(id, hname)
    d = findAckHname(l, hname)
    gn = l['hgroup']
    # add hname to hgrpup
    addHnameToGroups(gn,hname)
    d['ack'] = cur_mtime() - tmbase

def addHnameToGroups(gn, hname):
    global gDict
    a = [0,1,2,3]
    a[2] = gn
    a[3] = hname
    dest =  sys.stdout
    d = newGroup(a, 'hlist', dest)

#    g_groups = gDict["groups"]
#    if gn not in g_groups.keys():
#        g_groups[gn] = {}
#    gg = g_groups[gn]
#    if hname not in gg.keys():
#        gg[hname] = {}

# l is the ack key
# hname is the name used to broadcast the ack 
# group members are in group items
# register ack should run through this
# each ack will have a list of hosts thar we expect an ack from

def setupGroup(l, hname):
    global gDict
    g_groups = gDict["groups"]
    hl = l['hlist']
    a = [0,1,2]
    a[2] = hname
    dest =  sys.stdout
    g = newGroup(a, 'hlist', dest)
    print(" setupgroup g:",g)
    
    for gn in g['items']:
        print(" setupgroup gn:",gn)

        d = {}
        d['host'] = gn    
        d['acnt'] = 1    
        hl[gn] = d
    print(" setupgroup l:",l)

#    else:
#        d = {}
#        d['host'] = hname    
#        d['acnt'] = 1    
#        hl[hname] = d
#        dg = {}
#        dg[hname] = d
#        g_groups[hname] = dg
# groups are:
# hlists ( a name followed by items (hosts) in the list)
# dlist ( a name followed by items  in the list)
def getGroup(a, hlist, dest):
    global gDict
    g_groups = gDict["groups"]
    name = ''
    d = {}
    if len(a) > 2:
        if hlist not in g_groups.keys():
            g_groups[hlist] = {}
        d = g_groups[hlist]
        name = a[2]
        if name not in d.keys():
            d[name] = {}
            d[name]['items'] = []
        return d[name]
    return None

def newGroup(a, hlist, dest):
    d = getGroup(a, hlist, dest)
    if d:
        if len(a) > 3:
            d['items'].append(a[3])
    return d

def newHGroup(a, b, dest):
    newGroup(a, 'hlist', dest)
        
def newDGroup(a, b, dest):
    newGroup(a, 'dlist', dest)

def newDGroupFcn(a, b, dest):
    d = getGroup(a, 'dlist', dest)
    # a[3] is a function    

def showGroups(a ,b, dest):
    global gDict
    g_groups = gDict["groups"]
    for g in g_groups.keys():
        gg = g_groups[g]
        msg = "group name :" + g + "\n" 
        sendMsg(dest, msg )

        for gh in gg.keys():
            msg = "     " + gh
            msg += json.dumps(gg[gh])
            msg += "\n"
            
            sendMsg(dest, msg )

# needs hname plus id
#registerack id ,name , msg
def testReg(a, b, dest):
    global lastid
    if len(a) < 4:
        msg = " need id , name , msg\n" 
        sendMsg(dest, msg)
        return
    id = a[1]
    if id == 0 or id == '0':
        id = str(cur_mtime() - tmbase)
        lastid = id
    registerAck(id ,a[2] ,a[3])

            
def registerAck(id, hname, msg):
    l = findAckKey(id, hname)
    l['msg'] = msg
    l['hgroup'] = hname    
    l['hlist'] = {}    
    setupGroup(l,hname)

# confirm ack
# called by the decoder when a cak cmd is received
#           #{"ackhost":"pi4","ack":1035432}
#            if "ackhost" in jc.keys():
#                id = jc["ack"]
#                hname = jc["ackhost"]
#                confirmAck(id, hname)
#                print(" got ack")
# ack generated like this
#  if "id" in jc.keys():
#            id = jc["id"]
#            msg = '{"ackhost":"'+hostname+'","ack":'+str(id)+'}'
#            sockSend(msg, ssock)

def testConf(a, b, dest):
    if len(a) < 3:
        msg = " need id , hname\n" 
        sendMsg(dest, msg)
        return
    if a[1] == 0 or a[1] == '0':
        id  = lastid
    else:
        id = a[1]
    #print(" confirm Ack id:[",id,"] type:", type(id))
    #print(" confirm Ack a:",a)
    
    confirmAck(id, a[2])
    
def showAcks(a,b,dest):
    global gDict
    g_groups = gDict["groups"]
    g_acks = gDict["acks"]
    tmbase = gDict["tmbase"]

    for id in g_acks.keys():
        l = g_acks[id]
        if not l:
            continue
        msg =  "id :" + str(id) + " msg :" + l["msg"] + " gname :" + l["hgroup"]
        #print (msg)
        sendMsgNl(dest, msg )
        hl = l['hlist']    

        for hname in hl.keys():
            msg =  "     hname :" + hname
            sendMsgNl(dest, msg)
            #print (msg)
            d = hl[hname]
            if 'ack' in d.keys():
                msg =  "        ack :" + str(d["ack"])
                #print (msg)
                sendMsgNl(dest, msg)

ACKRETRY = 3

def scanAcks(dx):
    global gDict
    g_groups = gDict["groups"]
    g_acks = gDict["acks"]
    tmbase = gDict["tmbase"]
    hdel = []
    for id in g_acks.keys():
        l = g_acks[id]
        if len(l) == 0:
            hdel.append(id)
        print ("scan ", id, l)
        hl = l['hlist']    
        ldel = []
        for hname in hl.keys():
            d = hl[hname]
            if 'ack' not in d.keys():
                #msg =  d["msg"]
                x = d['acnt'] +1
                if x < ACKRETRY:
                    d['acnt'] = x
                    #sockSend(msg)
                    #sendMsg
                    print ("x ",x," Resend []")
                else:
                    if dx:
                        ldel.append(hname)
        print("ldel ", ldel)
        for ld in ldel:
            del hl[ld]
        #if dx:
        #    if len(l) == 0:
        #        del g_acks[id]
    for hd in hdel:
        del g_acks[hd]
