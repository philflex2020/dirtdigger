#!/usr/bin/python3
# here we create and decode the messages and
# manage the data base
# given a list of lables [a,b,c,d]
# descend inyo a dict structure to find d
# oh and c is a list

import json

a={}

def addD(a,k,b):
    a[k] = b

def addMD(d,n, depth):
    for k in range(1,n):
        ks = str((depth * 100) +k)
        d[ks] = {}
    
    if depth > 0:
        depth -= 1
        for k in d.keys():
            addMD(d[k],n, depth)

def findMD(d, keys):
    for k in keys:
        print (k)
        if k in d.keys():
            print (d[k])
            print (keys[1:])
            return findMD(d[k],keys[1:])
        else:
            return k

def addLVal(d, n, keys, val):
    dx = {}
    dx[n] = {}
    dy = dx[n]
    d.append(dx)
    addVal(dx[n], keys, val)

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
    # handle a list a bit more complex
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
        

def getVal(d, keys):
    n = keys[0]
    if len(keys) > 1:
        if n in d.keys():
            d1 = d[n]
            keys = keys[1:]
            return getVal(d1, keys)
    else:
        return[d[n]]
    return None

import copy
def getTree(d,keys,sofar=[], msg=[]):
    if len(keys) == 0:
        for n in d.keys():
            ss1 = copy.copy(sofar)     
            d1 = d[n]
            if type(d1) == type({}):
                ss1.append(n)
                getTree(d1, keys, ss1 ,msg)
            elif type(d1) == type([]):
                ss1.append(n)
                for n1 in d1:
                    ss = copy.deepcopy(ss1)     
                    ss.append(n1)
                    getTree(d1, keys, ss, msg)
            else:
                #ss1.append(n)
                mm ="[\""+ "\",\"".join(ss1) + "\",\""+ str(n)+"\"]=\"" + str(d1)+"\""
                msg.append(mm)
    else:
        k = keys[0]
        if k in d.keys():
            keys=keys[1:]
            msg = getTree(d[k], keys, sofar, msg)
    return msg
                
#addMD(a, 5, 7)

#print (a)
#print ("a.keys")
#print (a.keys())
#print (a["702"].keys())
#a["703"]["602"]["504"]["403"]["foo"] = "fum"

#keys = [ "703","602", "504","403"]

#d = findMD(a, keys)
#print (d)

# this is the "data" database

d = {}
# host , node , item, value
addVal(d, [ "pi2","motors", "power"], "off")
addVal(d, [ "pi2","motors", "leftfront","speed"], 21)
addVal(d, [ "pi2","motors", "rightfront","speed"], 22)
addVal(d, [ "pi2","motors", "leftfront","dir"], "fwd")
addVal(d, [ "pi2","motors", "rightfront","dir"], "fwd")
addVal(d, [ "pi2","motors", "power"], "on")
#addVal(d, [ "pi2","motors", "list"], [])
addVal(d, [ "pi2","motors", "list", "item1","val1"], "i1")
addVal(d, [ "pi2","motors", "list", "item1","val2"], "i2")
addVal(d, [ "pi2","motors", "list", "item2","val1"], "i1")
addVal(d, [ "pi2","motors", "list", "item2","val3"], "i3")
print (d)

print(json.dumps(d, indent=4, sort_keys=False))
v = getVal(d, [ "pi2","motors", "leftfront", "speed"])
print ("val = : ", v)
mv = getVal(d, [ "pi2","motors", "leftfront"])
print ("mv val = : ", mv[0])

# add a new motor
addVal(d, [ "pi2","motors", "leftrear"], mv[0])

msg = getTree(d ,["pi2","motors","leftfront"])
print ("tree = :")
for m in msg:
    print ("   ", m )
    
print(json.dumps(d, indent=4, sort_keys=False))

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



