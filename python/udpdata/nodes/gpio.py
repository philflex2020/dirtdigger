gDict = {}
uu = None
gName = None

def init(name, dict, ux):
    global gName
    global gDict
    global uu
    gName = name
    uu = ux
    gDict=dict
    print (name + " set up,  uu version :", uu.getVersion())

def run(a,b,c):
    global gName
    print (gName + " running")
