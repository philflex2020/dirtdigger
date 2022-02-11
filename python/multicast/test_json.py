# Test the json message structure
#
import json

def findNode(node, nlist) :
    for i in range(len(nlist)):
        if node in  nlist[i].keys():
            return nlist[i]
    return None

def findItem(node, item, nitem) :
    mlist = nitem[node]
    print (mlist)
    print (mlist[0].keys()[0])
    #print("Ok so far")
    for i in range(len(mlist)):
        if item ==  mlist[i].keys()[0]:
            return mlist[i]
    return None

def findItemVal(node, item, nitem) :
    aa = findNode(node, xy)
    ab = findItem(node,item, aa)
    if ab:
        return ab.get(item)
    return None

msg = (
  '['  
  '{'
   '"1234" :['
	      '{"item1":"val1"},'
              '{"item2":"val2"},'
              '{"item3":"val3"},'
              '{"item4":"val4"}'
           ']'
           '},'
  '{'
   '"1235":['
	     '  {"item1":"val1"},'
              ' {"item2":"val2"},'
               '{"item3":"val3"},'
               '{"item4":"val4"}'
          ']'
    '}'
    ']'
)
jc = json.loads(msg)
#jc[0]

xx = json.dumps(jc[0])
print (xx)
#print (xx.keys())
print(json.dumps(jc[0].keys()))
print(json.dumps(jc[0]["1234"][0]["item1"]))
print(json.dumps(jc[0]["1234"][0].keys()))
print(json.dumps(jc[0]["1234"][1].keys()))
print(range(len(jc)))
# list of dicts
#
xy = [{"1234":[{"item1":"val1"},{"item2":"val2"}]}]
xy1 = {"1235":[{"item1":"val1"},{"item2":"val2"}]}
xy.append(xy1)
xyj = json.JSONEncoder().encode(xy)

xys = json.dumps(xyj)
print(xys)

#xy is our main object
# now find <node> <item> in xy
print(range(len(xy)))

aa = findNode("1234", xy)
ab = findItem("1234","item2", aa)
ac = findItemVal("1234","item2", xy)

print(aa)
print(ab)
print(ab.get("item2"))
print(ac)

