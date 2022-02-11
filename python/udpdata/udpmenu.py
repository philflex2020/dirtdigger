#!/usr/bin/python3

import sys
import select
import socket
import udputils as uu

udpMenu = {}
udpKeys = {}
udpMenuCtl = {}
udpMenuCtl["tick"] = 0
udpMenuCtl["quit"] = 0
udpMenuCtl["menu"] = {}
menuSocks = [ sys.stdin,]
menuServers = [ ]
rSocks ={}

gDict = {}

def runMenu(dest, data= None):
    global menuSocks
    global menuServers
    global udpMenu
    global rSocks
    if len(udpMenu) == 0:
        setup()
    

    while udpMenuCtl["quit"] == 0:
        if not data:
            msg = "udpmenu >"
        else:
            msg = data + " >"
        sendMsg(dest, msg)
        mflag = 0
        while not mflag:
            read_socks,write_socks,error_socks = select.select(menuSocks,[],[],0.01)
            udpMenuCtl["tick"] = udpMenuCtl["tick"] + 1
            #print ("tick")
            for s in read_socks:
                if s == sys.stdin:
                    mflag = 1;
                    cmd = s.readline()
                    runMenuInput(udpMenu, cmd, sys.stdout, data)
                    
                elif s in menuServers:
                    #mflag = 1;
                    con, ca = s.accept()
                    con.setblocking(0)
                    menuSocks.append(con)

                elif s in rSocks.keys():
                    print (rSocks)
                    #mflag = 1;
                    d = rSocks[s]
                    decoder = d["decoder"]
                    dest = d["dest"]
                    fcn = d["read"]
                    dat = d["data"]
                    rc = fcn(s, dat, dest, decoder)
                
                elif s in menuSocks:
                    mflag = 1;
                    cmd = s.recv(1024)
                    c = cmd.decode("utf-8")
                    runMenuInput(udpMenu, c, s, data)
                    #runMenuInput(udpMenu,cmd, s, data)

                    
def addSocket(cmds, data, dest):
    global menuSocks
    print (" len cmds ", len(cmds))
    if len(cmds) > 1:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.setblocking(0)
        sa = ('localhost',int(cmds[1]))
        s.bind(sa)
        s.listen(5)
        menuSocks.append(s)
        menuServers.append(s)
        print(s)
    else:
        sendMsg(dest," please provide a port\n")
        
def addRsock(rsock,readsock, data, decode=None):
    global rSocks
    d = {}
    rSocks[rsock] = d
    d["read"] = readsock
    d["data"] = data
    d["dest"] = data
    d["decoder"] = decode
    menuSocks.append(rsock)
    print (" appended rsock :", rsock)
    
MCAST_GRP = '224.1.1.1'
mgrp = MCAST_GRP
def testDecode(rsock,dat,data,dest):
    print(" test decode:", data)
    return 0

def newListener(a, b, dest):
    global mgrp
    #decode = None
    rs = uu.createRsock(mgrp, int(a[1]))
    print (" new listener on port " , a[1])
    if rs :
        if len(a) > 2:
            print (" new listener decoder " , a[2])
            #setRdecoder(a[1], a[2])
            if len(a) > 3:
                print (" new listener resp " , a[3])
                #setRreply(a[1], a[3])
        addRsock(rs, uu.readRsock, dest, testDecode)
    else:
        print (" port " , a[1], " already assigned")

def msendSock(a, b, dest):
    global mgrp
    if len(a) > 2:
        port =  a[1]
        msg =  a[2]
        uu.sockSend(a[2], mgrp, int(a[1]))
            
def addMenuItem(key, name, desc, fcn, data=None):
    global udpMenu
    if len(udpMenu) == 0:
        setup()
    setMenuItem(key, name, desc, fcn, data)

def setMenuItem(key, name, desc, fcn, data=None):
    global udpMenu
    global udpKeys
    d = {}
    d["desc"]=desc
    d["fcn"]=fcn
    d["data"]=data
    d["key"]=key
    udpMenu[name] = d
    udpKeys[key] = d

def addChar(v,n):
    #if n in ['\'','\"']:
    #    v += '\\'
    v += n
    #print(v ,n)
    return v

def mysplit(cmd):
    res = []
    esc = None
    v = ""
    for n in cmd:
        if n == " ":
            if not esc:
                if len(v) > 0:
                    res.append(v)
                    v = ""
            else:
                v += n       
        elif n in ['"','\'']:
            if n == esc:
                esc = None
            else:
                if esc:
                    v = addChar(v,n)
                    #v += n
                else:
                    esc = n
        elif n == '\n':
            if not esc:
                if len(v) > 0:
                    res.append(v)
                    v = ""
            else:
                v = addChar(v,n)
        else:
            v += n
    if len(v) > 0:
        res.append(v)
        v = ""
    return res
    
def runMenuInput(gMenu, cmd, dest, data = None):
    #global udpMenu
    cmds = mysplit(cmd)
    print(" cmds ")
    print(cmds)
    return runMenuCmds(gMenu, cmds, dest)
    
def runMenuCmds(gMenu, cmds, dest, data = None):
    global udpMenu
    global udpKeys
    d = None
    if len(cmds) > 0:
        if  cmds[0] in udpKeys.keys():
            d = udpKeys[cmds[0]]
            if  cmds[0] in gMenu.keys():
                d = gMenu[cmds[0]]
    if d:            
        d["fcn"] (cmds, d["data"], dest)
        
def showMenuItems(cmds, data, dest):
    global udpMenu
    for k in udpMenu.keys():
        d = udpMenu[k]
        msg = "("+d["key"]+") "+ k +": " + d["desc"] +"\n"
        sendMsg(dest,msg)

def sendMsg(dest,msg):
    #print(dest)
    if dest == sys.stdin:
        sys.stdout.write(msg)
        sys.stdout.flush()
    elif dest == sys.stdout:
        sys.stdout.write(msg)
        sys.stdout.flush()
    else:
        dest.send( msg.encode("utf-8"))

def sendMsgNl(dest,msg):
    sendMsg(dest,msg)
    sendMsg(dest,"\n")

def setup():
    setMenuItem("h", "help", " Show menu options", showMenuItems)
    setMenuItem("q", "quit", " Quit", quitMenu)
    setMenuItem("l", "listen", "Add Listen Port", addSocket)
    addMenuItem("t", "test", " run a test item", testMenu, None)
    
def setDict(dict):
    global gDict
    gDict = dict
    
def testMenu(cmds, data, dest):
    global udpMenu
    global gDict
    gDict["cmds"] = cmds
    sendMsg( dest, "running test menu\n")

def quitMenu(cmds, data, dest):
    global udpMenu
    sendMsg(dest ,"running quit menu\n")
    udpMenuCtl["quit"] = 1


if __name__ == "__main__":
    print("hello\n")
    addMenuItem("t", "test", " run a test item", testMenu, None)
    runMenuInput(udpMenu,"test the menu", sys.stdout)
    runMenuInput(udpMenu, "help the menu", sys.stdout)
    #runMenuInput("quit the menu", sys.stdout)
    runMenu(sys.stdin)
    print("bye  : quit",   udpMenuCtl["quit"] , "\n")

          
