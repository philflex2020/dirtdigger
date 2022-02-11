#!/usr/bin/python3
# test systems for udpdata

# here we create and decode the messages and
# manage the data base
# given a list of lables [a,b,c,d]
# descend into a dict structure to find d
# oh and c is a list

# we end up with one dict for them all
# orgamised by hostname, type of data etv
# in addition to an object data base this thing can have simple raw lists of things
# we send the command line to the destination
# well maybe.
# make the parsing etc much simpler.
# and easier to test perhaps
# ths lot was established from a need to make better use of recursive parsing.
#

# refs refs give a single word to a key
# for example
#     motrfs = motors.rightfront.speed
#     motrfd = motors.rightfront.direction
#
# reflist is simply a list of refs
# we can then ask for a reflist
# rgv reflist values
# rgl reflist defines


# getting an object id
# notes on keys. Looks like the key order is not a constant
# we will have to have a dict of known words each with an id
# that way we can reduce motors.leftfront.speed down to 1.5.6
#

#
# lists events and timed stuff
# we can create a list of data items
# we can set or get values in that list (trf)
# we can send out a list as an event
#
# to do we have predefined some prefixes, for example event data lists 
#  setList getList
#        key.append('groups')
#        key.append('refs')
# when recoverig data from a list use <hname> data 
# testGetVal setList getList
#   to point the list items into the local area
#            rstr = 'pi2 data ' + r
#
# decodeMsg
# so far we have
#      avd
#      svd
#      gvd
# more todo

# key = ['gtl' 'pi2','groups','refs','motl']
# key = ['stl' 'pi2','motl' 'val1 '.....]

#   (orig,flag) = checkhostName(args)
# checkHostName
#   we will deprecate the Hostname field and use the data field arg [0]
#     if this is a * 
#     or if we find it in a group (pi2 groups hgroups)
#     then locally substitute for local host name
#
#
# local database
# has hname as the first item this is always added
#
# back to basics again
# avl should probably become ali
# this will create a new list , if needed , and add an item to it.
#    done
#


import json
import os
import sys
import copy

import udputils as uu
import udpmenu

hh = os.uname()
hn = hh[1]

a={}
d = {}
words = {}

lbase = ['groups','refs']
g_word_idx = 0
g_debug = 0

def getWordIdx (wd):
    if wd not in words.keys():
        words[wd] = g_word_idx
        g_word_idx +=1
    return words[wd]

def keysToIdx(keys):
    res = []
    for k in keys:
        ix = getWordIdx(k)
        res.append(ix)
    return res

##def addD(a,k,b):
##    a[k] = b

#def addMD(d,n, depth):
#    for k in range(1,n):
#        ks = str((depth * 100) +k)
#        d[ks] = {}
    
#    if depth > 0:
#        depth -= 1
#        for k in d.keys():
#            addMD(d[k],n, depth)

#def findMD(d, keys):
#    for k in keys:
#        print (k)
#        if k in d.keys():
#            print (d[k])
#            print (keys[1:])
#            return findMD(d[k],keys[1:])
#        else:
#            return k


# here starts the real code
# add a list val
def addLVal(d, n, keys, val):
    dx = {}
    dx[n] = {}
    dy = dx[n]
    d.append(dx)
    addVal(dx[n], keys, val)

    #add a val
# adv
# see ali and testaddVal
# adv hn val h1 h2 h3 h4
# addVal(d, [ hn, "data", "motors", "pstate"], "running")
# adv host val keys
def addVal(d, keys, val):
    if type(d) == type({}):
        n = keys[0]
        if len(keys) > 1:
            if n not in d.keys():
                d[n] = {}
            d1 = d[n]
            keys = keys[1:]
            addVal(d1, keys, val)
        else:
            d[n] = val
    # handle a list, a bit more complex
    #"foo":[{"node1":{"foo1":"v1","foo11","v11"}},{"node2":{"foo2":"v2"}}]
    elif type(d) == type([]):
        n = keys[0]
        if len(keys) > 0:
            if len(d) == 0:
                addLVal(d, n, keys, val)
                return
            for di  in d:
                if n in di.keys():
                    dx = di[n]
                    keys = keys[1:]
                    addVal(dx, keys, val)
                    return
            keys = keys[1:]
            addLVal(d, n, keys, val)
            return
        else:
            d[n] = val
    return

# ali hn h1 h2 h3 h4 val
# addValList(d, [ hn, "groups", "refs", "motl"], "motors power")
# todo add wildcards maybe
# recursivly adds keys then creates a list at the end
def addValList(d, keys, val):
    if type(d) == type({}):
        n = keys[0]
        if len(keys) > 1:
            if n not in d.keys():
                d[n] = {}
            d1 = d[n]
            keys = keys[1:]
            addValList(d1, keys, val)
        else:
            if n not in d.keys():
                d[n] = []
            d[n].append(val)
    # handle a list a bit more complex
    #"foo":[{"node1":{"foo1":"v1","foo11","v11"}},{"node2":{"foo2":"v2"}}]
    elif type(d) == type([]):
        n = keys[0]
        d.append(val)
    return
        
# match a hostname
def kMatch(k, n):
  if n == "*" :
      return 1
  if k == n :
      return 1
  return 0

# get a val
# just gets the val not the whole data entry
# request
# keys = : ['pi2', 'data', 'motors', 'power']
# returns
def getVal(d, keys, res, lname):
    global g_debug
    if len(lname) == 0:
        lname = '[ \"'
        if g_debug:
            print ("keys :", keys)
    if type(d) != type({}):
        print (" ERROR not right type")
        return
    n = keys[0]
    if g_debug:
        print ("looking for ", n, "keys :", keys, " d.keys ", d.keys())
        
    if len(keys) > 1:
        keys = keys[1:]
        for k in d.keys():
            ln = copy.copy(lname)
            if kMatch(k,n):
                d1 = d[k]
                if len(keys) > 0:
                    #res.append(k)
                    if len(ln) > 3:
                        ln = ln + " " + k
                    else:
                        ln = ln + k
                    getVal(d1, keys, res , ln)
    else:
        for k in d.keys():
            ln = copy.copy(lname)
            if kMatch(k,n):
                if g_debug:
                    print ("kmatch  k/n ", k, n, "val  ", d[k])
                if type (d[k]) == type([]):
                    val = json.dumps(d[k])
                    #print (">>>val :", val)

                    xname = lname + " " + k + "\"," + val +"]"
                    res.append(xname)
                        
                if type (d[k]) == type({}):
                    val = json.dumps(d[k])
                    if g_debug:
                        print (">>>val >>>:", val)
                    res.append(val)
                    return val 
                val = toString(d[k])
                if val:
                    #print (">>>val >>>:", val)
                    #print (">>>lname 1 >>>:", lname)
                    lname = lname + " " + k + "\"," + val + "]"
                    #print (">>>lname 2 >>>:", lname)
                    #print (">>>type lname 2 >>>:", type(lname))
                    res.append(json.loads(lname))
                    #print (">>>res >>>:", res)

                #res.append(n)
                #res.append(d[n])
        if g_debug:
            print (">>> ret res 1 >>>:", res)
        return res
    if g_debug:
        print (">>> ret res 2 >>>:", res)
    return res
#testSetVal
#
def setVal(d, keys, val):
    if type(d) != type({}) :
        return
    n = keys[0]
    if len(keys) > 1:
        keys = keys[1:]

        for k in d.keys():
            if kMatch(k,n):
                #print ("setval found :", k)
                d1 = d[k]
                setVal(d1, keys, val)
    else:
        #print(" d:", d, " n:", n)
        if n in d.keys():
            d[n] = val
        return 
    return 

def toString(v):
    val = None
    if type(v) == type(1):
        val = str(v)
    elif type(v) == type(1.2):
        val = str(v)
    elif type(v) == type(""):
        val = "\""+v+"\""
    return val

# this is a bit hacked the target will probably use json
#
def getTree(d, keys=[], sofar=[], msg=[]):
    #print ("getTree d.keys(): ", d.keys(), " msg len :", len(msg))
    if len(keys) == 0:
        if type(d) == type([]):
            # tries to create a list of vals
            # try this
            m1 = json.dumps(d)
            print(" xxx found list :", d, " vals: ", m1)
            #mm ="[\""+ "\",\"".join(sofar) + "\"]= [\""+'\",\"'.join(d)+"\"]"
            mm ="[\""+ "\",\"".join(sofar) + "\"]=" + m1
            #print(" xxx found list :", d)
            msg.append(mm)
            return msg
            
        for n in d.keys():
            ss1 = copy.copy(sofar)     
            d1 = d[n]
            if type(d1) == type({}):
                print("{} d1 = ",d1)
                print("n = ",n)
                ss1.append(n)
                getTree(d1, keys, ss1, msg)
            elif type(d1) == type([]):
                print("[] d1 = ",d1)
                print("n = ",n)
                ss1.append(n)
                getTree(d1, keys, ss1, msg)
                #for n1 in d1:
                #    ss = copy.copy(ss1)     
                #    ss.append(n1)
                #    getTree(d1, keys, ss, msg)
            else:
                val = toString(d1)
                if val:
                    #ss1.append(n)
                    mm ="[\""+ "\",\"".join(ss1) + "\",\""+ str(n)+"\"]=" + val
                    msg.append(mm)
    else:
        k = keys[0]
        if k in d.keys():
            keys=keys[1:]
            msg = getTree(d[k], keys, sofar, msg)
    return msg

# this is the "data" database

d = {}

def initD():
    global d
    global hn
    # host , dtype , node , item, value
    addVal(d, [ hn, "data","motors", "power"],                   "off" )
    addVal(d, [ hn, "data","motors", "leftfront","speed"],       21)
    addVal(d, [ hn, "data","motors", "rightfront","speed"],      22)
    addVal(d, [ hn, "data","motors", "leftfront","dir"], "fwd")
    addVal(d, [ hn, "data","motors", "rightfront","dir"], "rev")
    addVal(d, [ hn, "data","motors", "power"], "on")
           #addVal(d, [ hn,"data","motors", "list"] , [])
    addVal(d, [ hn, "data","motors", "list", "item1","val1"], "I1")
    addVal(d, [ hn, "data","motors", "list", "item1","val2"], "I2")
    addVal(d, [ hn, "data","motors", "list", "item2","val1"], "I3")
    addVal(d, [ hn, "data","motors", "list", "item2","val3"], "i4")
    uu.newHGroup(['ahg', "*","main","pi2" ],None,sys.stdout)
    uu.newHGroup(['ahg', "*","main","pi3" ],None,sys.stdout)
    uu.newHGroup(['ahg', "*","main","pi4" ],None,sys.stdout)
    uu.newDGroup(['adg', "*","motl","motors power" ],None,sys.stdout)
    uu.newDGroup(['adg', "*","motl","motors leftfront speed" ],None,sys.stdout)
    uu.newDGroup(['adg', "*","motl","motors leftfront dir" ],None,sys.stdout)
    uu.newDGroup(['adg', "*","motl","motors rightfront speed" ],None,sys.stdout)
    uu.newDGroup(['adg', "*","motl","motors rightfront dir" ],None,sys.stdout)
    uu.newDGroup(['adg', "*","motl","motors power" ],None,sys.stdout)
    print (d)

#print(json.dumps(d, indent=4, sort_keys=False))
#v = getVal(d, [ hn,"motors", "leftfront", "speed"])
#print ("val = : ", v)
#mv = getVal(d, [ hn,"motors", "leftfront"])
#print ("mv val = : ", mv[0])

# add a new motor
#addVal(d, [ hn,"motors", "leftrear"], mv[0])

#msg = getTree(d ,[hn,"motors","leftfront"])
#print ("tree = :")
#for m in msg:
#    print ("   ", m )
    
#print(json.dumps(d, indent=4, sort_keys=False))

# here are a list of commands regognised by udpsend.py
# d dump
# "g stuff"      ->'{"get":["stuff"]}'
# "g pix stuff"  ->'{"hostname":"pix","id":821162569,"get":["stuff"]}
# resp ==>'{"ackhost":"pix","ack":821162569}'
# resp ==>'{"hostname":"pix","data":{"stuff":{"d1": "v1"}}}'

#"s stuff d22 v22" ->'{"hostname":"*", "id":"821394900","set":[{"stuff":{"d22":"v22"}}]}'
# resp ==>'{"ackhost":"pix","ack":821394900}'

# command states
#
# set command
#'{"hostname":"*", "id":"768842602","set":[{"1234":{"vvv":"dd"}}]}'
#'{"hostname":"*", "id":"768842602","cmd":"set","data":[{"1234":{"vvv":"dd"}}]}'

state = {}
sb = {}
state["base"] = sb

def checkHost(a,b,c):
    print("running checkHost")

def ackId(a,b,c):
    print("running ackId")

def setData(a,b,c):
    print("running setData")

def doCmd(sb,md,mm):
    print("running doCmd")
    cc = md[mm]
    print (" cmd is ", cc)
    cd = md["data"]
    print (" cd  is ", cd)
    if type(cd) == type([]):
        for ci in cd:
            print (" node[s] :", ci.keys())
            for cx in ci.keys():
                dd = ci[cx]
                print (" items[s] are:", dd.keys())
                for vv in dd.keys():
                    vi = dd[vv]
                    print (" items is:", vv , " value is :", vi)


addVal(sb, [ "hostname"], checkHost)
addVal(sb, [ "id"], ackId)
addVal(sb, [ "cmd"], doCmd)
addVal(sb, [ "set"], setData)

msg = '{"hostname":"*", "id":"768842602","cmd":"set","data":[{"1234":{"vvv":"dd"}}]}'


def runParse(st, bs, ms, stuff):
    sb = st[bs]
    md = json.loads(ms)
    print(md)
    print(md.keys())
    print(sb)
    for mm in md.keys():
        print(mm)
        if mm in sb.keys():
            cd = sb[mm]
            print(type(cd).__name__)
            if type(cd).__name__ == "function":
                cd (sb,md,mm)

runParse(state, "base", msg, None)

gDict = {}
# hmsg
def getHostMsg(kk, cmd):
    global hn
    msg = ""
    if kk != "-":
        id = 1234
        msg = '\"host\":\"'+ kk +'\",\"id\":'+ str(id) +',\"cmd\":\"' + cmd + '\",'
    return msg

def decNum(v):
    try:
        r = int(v)
        return r, True
    except ValueError:
        try:
            r = float(v)
            return r, True
        except ValueError:
            return v, False

def checkHostName(mm):
    global hn
    print ("Checkhostname")
    ok = True
    print ("OK true", ok)
    
    ok = False
    
    keys = mm["data"]
    hret = keys[2]
    print ("hret ", hret, " hostname ", hn)
    if hret == hn:
        ok = True
    elif hret == "*":
        keys[2] = hn
        ok = True
        
    # check for a group with our name int it
    print ("Checkhostname ok ", ok, " keys[2] " , keys[2], " keys " , keys)
    return(ok,hret)        


# change command to cmd dest [val] [keys ...]
# dest can be a direct match or a alias list
# we'll replicat the cmd in the header
# this is registered with a socket
def decodeMsg(d, msg, dest):
    global hn
    mm = json.loads(msg)
    (hret,ok) = checkHostName(mm)
    if not ok:
        print ( "msg :", msg, "rejected (hostname)")
        return msg
    # sendack if we see an id
    if "id" in mm.keys():
        id = mm["id"]
        msg = '{"ackhost":"'+hn+'","ack":'+str(id)+'}'
        #sockSend(msg, ssock)
        #uu.sendMsg(dest, msg)
    #decode ackhost
    if "ackhost" in mm.keys():
        id = mm["ack"]
        hname = mm["ackhost"]
        uu,confirmAck(id, hname)
        print(" got ack")
        return
    msh = None
    keys = mm["data"]
    try:
        cmd = mm["cmd"]
    except:
        cmd = keys[0]
        
    print ( " decodeMsg: cmd:[", cmd, "] keys are now :" , keys)
    if cmd == "avd":
        if keys[1] == "*":
            keys[1] = hn
        # avd hn h1 h2 h3 h4 val
        val = keys[-1]
        key = keys[1:-1]
        print ( " decodeMsg: cmd", cmd, "key is now :" , key , " val ",val)

        msg = addVal(d, key, val)
    elif cmd == "svd":
        # svd hn h1 h2 h3 h4 val
        if keys[1] == "*":
            keys[1] = hn
        val = keys[-1]
        key = keys[1:-1]
        msg = setVal(d, key, val)
    elif cmd == "gvd":
        #val = keys[1]
        key = keys[1:]
        key [0] = hn
        res = []
        val = getVal(d, key, res , "")
        #msg = json.dumps(res)
        msg = res
    elif cmd == "ali":
        # ali hn h1 h2 h3 h4 val
        val = keys[-1]
        key = keys[1:-1]
        key [0] = hn
        addValList(d, key , val)
    elif cmd == "alid":
        # ali hn h1 h2 h3 h4 val
        val = keys[-1]
        key = keys[:-1]
        key [0] = hn
        addValList(d, key , val)
    elif cmd == "gtl":
        print ( " running gtl keys:",keys)
        if keys[1] == "*":
            keys[1] = hn
        # gtl hn key
        key = keys[1:]
        key = keys
        key [1] = hn
        print ( " running getList keys:",keys)
        getList(key, keys, dest)
    return msg

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

g_count = 0
def testFun(a):
    global g_count
    print (" test fun running :", g_count, " args :", a)
    g_count += 1
    
def testRunFun(a, b, dest):
    uu.run_func(1,testFun,a,10)
    
#"avd"
# avd hn h1 h2 h3 h4 val
def testAddVal(keys, b, dest):
    global d
    global hn
    if len(keys) > 1:
        # extract the number or string from keys[1]
        (val,ok) = decNum(keys[-1])
        cmd = keys[0]
        host = keys[1]
        #keys[2]=val
        #key = keys[1:]
        #key[1] = host
        print("AddVal val ", val, " keys :", keys)
        hmsg = getHostMsg(keys[1], keys[0])
        msg = '{'+ hmsg + '\"data":'+ json.dumps(keys) +'}'
        print (" msg :", msg)
        rsp = decodeMsg(d, msg, dest)
        #addVal(d, key, val)
    else:
        print("avd dest key.... val")

#"svd"
def testSetVal(keys, b, dest):
    global d
    global hn
    if len(keys) > 1:
        # extract the number or string from keys[1]
        (val,ok) = decNum(keys[-1])
        keys[-1]=val
        #key = keys[3:]
        hmsg = getHostMsg(keys[1], keys[0])
        if keys[1] == "*":
            keys[1] = hn

        msg = '{'+ hmsg + '"data":'+ json.dumps(keys) +'}'
        print (" msg :", msg)
        rsp = decodeMsg(d, msg, dest)
        #setVal(d, key, val)
    else:
        print("needs svd dest key.. val")

#"gvd"
# gvd hname = where to send the commands
#
def testGetVal(keys, b, dest):
    global d
    if len(keys) > 2:
        key = keys[2:]
        print(" key = :", key)
        #addVal(d, [ "pi3","motors", "list", "item2","val3"], "i3")
        #addVal(d, [ "pi3","gpios", "outputs", "gpio2","dir"], "out")
        #addVal(d, [ "pi3","gpios", "outputs", "gpio2","val"], 0)
        hmsg = getHostMsg(keys[1],keys[0])
        msg = '{'+ hmsg + '"data":'+ json.dumps(keys) +'}'
        print (" msg :", msg)
        rsp = decodeMsg(d, msg, dest)
        #res = []
        #val = getVal(d, key, res , "")
        #for x in res:
        print(" rsp = :", rsp)
    else:
        print ("usage gvd dest data *")

def testGetList(a, b, dest):
    global d
    global hn
    if len(a) > 2:
        cmd = a[0]
        cmd = 'gtl'
        host = a[1]
        if host == "*":
            host = hn
        #(val,ok) = decNum(a[-1])
        #val = a[2]
        key = a
        key[0] = 'gtl'
        key[1] = host
        #key[-1] = val
        print("getList  key :", key)
        hmsg = getHostMsg(a[1],cmd)
        msg = '{'+ hmsg + '"data":'+ json.dumps(key) +'}'
        print (" msg :", msg)
        rc = decodeMsg(d, msg, dest)
    else:
        print ("usage gtld dest list")

# the key must be a list 
# ali hn h1 h2 h3 h4 val
# alid for the decode version
# addValList(d, [ hn, "groups", "refs", "motl", "motors power"])
def testAddValListDec(a, b, dest, dec):
    global d
    global hn
    if len(a) > 3:
        cmd = a[0]
        host = a[1]
        if host == "*":
            host = hn
        (val,ok) = decNum(a[-1])
        #val = a[2]
        key = a[1:]
        key[0] = host
        #key[-1] = val
        print("AddValList val ", val, " key :", key)
        hmsg = getHostMsg(a[1],a[0])
        msg = '{'+ hmsg + '"data":'+ json.dumps(key) +'}'
        print (" msg :", msg)
        if dec:
            rc = decodeMsg(d, msg, dest)
        else:
            key = a[1:-1]
            key[0] = host
            #val = a[2]
            rc = addValList(d, key , val)
        print(" avi rc :", rc)
        
        #addValList(d, [ hn,"groups", "nodes", "pinodes"], hn)
        #addValList(d, [ hn,"groups", "nodes", "pinodes"], "pi1")
        #addVal(d, [ "pi3","gpios", "outputs", "gpio2","dir"], "out")
        #addVal(d, [ "pi3","gpios", "outputs", "gpio2","val"], 0)
        #addValList(d, key, val)
    else:
        print("usage ali dest key... val")

#ali
def testAddValList(a, b, dest):
    return testAddValListDec(a, b, dest, False)
def testAddValListD(a, b, dest):
    return testAddValListDec(a, b, dest, True)

def testTree(a, b, dest):
    global d
    if len(a) > 1:
        mmsg = getTree(d ,a[:-1], [],[])
        print ("key tree = :", a[:-1])
    else:
        mmsg = getTree(d, [], [],[])
        print ("base tree len = :", len(mmsg))
    for m in mmsg:
        print ("   ", m )
    mmsg = []
    print(json.dumps(d, indent=4, sort_keys=False))

def runDump(a,b,dest):
    global d
    global g_debug
    res = []
    g_debug = 0
    rc = getVal(d, b, res, "")
    #print(" runDump rc : ", rc)
    return (rc,res)
    
#  test all
# "ta"
def testAll(a, b, dest):
    global d
    global hn
    
    print(" udp ticks",   udpmenu.udpMenuCtl["tick"] , "\n")
    # ali ok
    
    rc = addValList(d, [ hn, "groups", "refs", "mott"], "motor power")
    testAddValList(udpmenu.mysplit('ali * groups refs mott "motors state" '), b , dest) 
    testAddValListDec(udpmenu.mysplit('ali * groups refs mott "motors amps" '), b , dest, True) 
    # gli
    # testGetval
    res  = []
    rc = getVal(d, [ hn, "groups", "refs", "mott"], res, "")
    #rc = getValList(d, [ hn, "groups", "refs", "mott"])
    print(" getValList rc :", rc ,"res :",res)
    rc = testGetVal(["gvd", hn, "groups", "refs","mott"],None, sys.stdout)
    print(" testGetValList 1 rc :", rc )
    rc = testGetVal(udpmenu.mysplit('gvd * groups refs mott'), b , dest) 
    print(" testGetValList 2 rc :", rc )
    #return

    rc = addVal(d, [ hn, "data", "motors", "state"], "OK")
    print(" addVal rc :", rc)
    res = []
    rc = getVal(d, [ hn,"data", "motors", "state"], res, "")
    print(" getVal rc(single) :", rc)
    print(" getVal res (single):", res)
    res = []
    rc = getVal(d, [ hn,"groups", "refs", "mott"], res, "")
    print(" getVal rc(list) :", rc)
    print(" getVal res (list):", res)

    rc = setVal(d, [ hn,"data", "motors", "state"], "Fail")
    print(" setVal state rc :", rc)
    rc = setVal(d, [ hn,"data", "motors", "stuff"], "Fail")
    print(" setVal stuff rc :", rc)
    res = []
    rc = getVal(d, [ hn,"data", "motors", "state"], res, "")
    print(" getVal rc(state) :", rc)
    print(" getVal res (state):", res)
    res = []
    rc = getVal(d, [ hn,"data", "motors", "stuff"], res, "")
    print(" getVal rc(stuff) :", rc)
    print(" getVal res (stuff):", res)
    a = [ hn,"groups"]
    (rc,res) = runDump(d, a, None) 
    print(" runDump a :", a)
    print(" runDump res :", res)

    rc = testGetVal(["gvd", hn, "data", "motors","power"],None, sys.stdout)
    print(" tGV 1 res :", rc)
    rc = testGetVal(["gvd", hn, "data", "motors"],None, sys.stdout)
    print(" tGV 2 res :", rc)
    
# "stl"
# key = ['gtl' 'pi2','groups','refs','motl']
# key = ['stl' 'pi2','motl' 'val1 '.....]
def setList(lkey, b, dest):
    global d
    key = []
    lval = 3
    if len(lkey) > 1:
        key.append(lkey[1])
        key = key + lbase
        #key.append('groups')
        #key.append('refs')
        key.append(lkey[2])
        print("setList key :", key)
        res = []    
        val = getVal(d, key, res , "")
        # res = ['[ " pi2 groups refs motl",["motors power", "motors rightfront speed", "motors rightfront direction", "motors leftfront speed", "motors leftfront direction", "motors power"]]']
        print ("getvald result :", res)
        r1 = res[0]
        xx=json.loads(r1)
        print ("getvald xx :", xx)

        ans = []
        for r in xx[1]:
            if lval < len(lkey):
                rstr = hn + ' data ' + r
                rkeys = rstr.split(" ")
                #res = []
                val = setVal(d, rkeys, lkey[lval])
            else:
                print("****out of values:",lval , len(lkey))
            lval += 1
                #if len(res) > 0:
                #    rr = json.loads(res[0])
                #    ans.append(rr[1])
                #else:
                #    ans.append("")
            #print("************* ans :",ans)
    else:
        print("stl " + hn + " motl vals .. ")

# "gtl"
# key = ['gtl' 'pi2','groups','refs','motl']
# key = ['gtl' 'pi2','motl']
def getList(lkey, b, dest):
    global d
    global hn
    key = []
    if len(lkey) > 1:
        key.append(lkey[1])
        key = key + lbase
        #key.append('groups')
        #key.append('refs')
        key.append(lkey[2])
        print("***** getList lkey :", lkey)
        print("***** getList key :", key)
        res = []    
        val = getVal(d, key, res , "")
        # res = [[ " pi2 groups refs motl",["motors power", "motors rightfront speed", "motors rightfront direction", "motors leftfront speed", "motors leftfront direction", "motors power"]]]
        print ("getvald result :", res)
        r1 = res[0]
        xx=json.loads(r1)
        #xx = r1
        #print ("getvald xx :", xx)
        #print ("getvald xx[1] :", xx[1])

        ans = []
        for r in xx[1]:
            rstr = hn + ' data ' + r
            rkeys = rstr.split(" ")
            res = []
            val = getVal(d, rkeys, res , "")
            if len(res) > 0:
                #rr = json.loads(res[0])
                rr = res[0]
                ans.append(rr[1])
            else:
                ans.append("")
        print("************* ans :",ans)
    else:
        print("gtl " + hn +" motl")
        

# "trf"
def testRefs(a, b, dest):
    global d
    global hn
    addValList(d, [ hn, "groups", "refs", "motl"], "motors power")
    addValList(d, [ hn, "groups", "refs", "motl"], "motors rightfront speed")
    addValList(d, [ hn, "groups", "refs", "motl"], "motors rightfront dir")
    addValList(d, [ hn, "groups", "refs", "motl"], "motors leftfront speed")
    addValList(d, [ hn, "groups", "refs", "motl"], "motors leftfront dir")
    addValList(d, [ hn, "groups", "refs", "motl"], "motors power")

    key = [hn,'groups','refs','motl']
    res = []    
    val = getVal(d, key, res , "")
    # res = [[ " pi2 groups refs motl",["motors power", "motors rightfront speed", "motors rightfront direction", "motors leftfront speed", "motors leftfront direction", "motors power"]]]
    print ("getvald result :", res)
    r1 = res[0]
    xx=json.loads(r1)
    print ("getvald xx :", xx)

    ans = []
    for r in xx[1]:
        rstr = hn + ' data ' + r
        #print ("***** r =", r)
        rkeys = rstr.split(" ")
        #print("************* rkeys :",rkeys)
        res = []
        val = getVal(d, rkeys, res , "")
        #print ("res =",res)
        #print ("len res :", len(res))
        #print ("type res :", type(res))
        if len(res) > 0:
            #print ("res[0] =",res[0])
            #print ("type res[0] :", type(res[0]))

            #rr = json.loads(res[0])
            rr = res[0]
            #print ("rr =",rr)
            #print ("rr[1] =",rr[1])
            ans.append(rr[1])
        else:
            ans.append("")
            #print ("none")
    print("************* ans :",ans)
            
if __name__ == "__main__":
    #global hostname
    tmstart = uu.cur_mtime()
    gDict = uu.setupgDict()
    udpmenu.setDict(gDict)    
    #g_acks = {}
    initD()

    hh = os.uname()
    hostname = hh[1]
    
    udpmenu.addMenuItem("tv", "testTree", " test tree dump", testTree, sys.stdout)
    udpmenu.addMenuItem("avd", "testAvd", " add value to data", testAddVal, sys.stdout)
    udpmenu.addMenuItem("gvd", "testGvd", " get value[s] from data", testGetVal, sys.stdout)
    udpmenu.addMenuItem("svd", "testSvd", " set Value", testSetVal, sys.stdout)
    udpmenu.addMenuItem("ali", "testAvl", " add value to list", testAddValList, sys.stdout)
    udpmenu.addMenuItem("alid", "testAvld", " add value to list", testAddValListD, sys.stdout)
    udpmenu.addMenuItem("gtlo", "testGtl", " get list values", getList, sys.stdout)
    udpmenu.addMenuItem("gtl", "testGtl", " get list values", testGetList, sys.stdout)
    udpmenu.addMenuItem("stl", "testStl", " set list values", setList, sys.stdout)
    udpmenu.addMenuItem("trf", "testref", " test ref lists", testRefs, sys.stdout)
    udpmenu.addMenuItem("ta", "testAll ", " test All ", testAll, sys.stdout)
    udpmenu.addMenuItem("td", "testD", " test Dict", testDict, sys.stdout)
    udpmenu.addMenuItem("ra", "reg", "register ack", uu.testReg,  sys.stdout)
    udpmenu.addMenuItem("ca", "conf", "confirm ack", uu.testConf, sys.stdout)
    udpmenu.addMenuItem("sa", "showA", "show acks",   uu.showAcks, sys.stdout)
    udpmenu.addMenuItem("sg", "showG", "show groups", uu.showGroups, sys.stdout)
    udpmenu.addMenuItem("ahg", "addHG", "add host group", uu.newHGroup, sys.stdout)
    udpmenu.addMenuItem("adg", "addDG", "add data group", uu.newDGroup, sys.stdout)
    udpmenu.addMenuItem("li", "udp listener [5007, 5008]", "add listener", udpmenu.newListener, sys.stdout)
    udpmenu.addMenuItem("ss", "sendsock 5008 'msg'", "send msg to sock", udpmenu.msendSock, sys.stdout)
    udpmenu.addMenuItem("run", "run function", "run test function", testRunFun, sys.stdout)

    # todo dump
    #  config
    # test acks
    

    udpmenu.runMenu(sys.stdin, "udptest")

    print("bye  : quit",   udpmenu.udpMenuCtl["quit"] , "\n")
