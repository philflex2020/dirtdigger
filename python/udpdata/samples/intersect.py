
#!/usr/bin/python3

f1 = "some fun"
f11 = "cmd"
f12 = "test"
f13 = "dir"
f14 = "data"
f15 = "key"
f16 = "ack"
f17 = "id"

list1 = [f1,f11,f17]
list2 = [f13,f14, f15, f1, f16,f17]

def intersect (l1, l2) :
    t1 = map(tuple,l1)
    t2 = map(tuple,l2)
    return list(map(list,set(t1).intersection(t2)))

def intersect2 (l1, l2) :
    res = []
    for i in l1:
        print(i)
        for j in l2:
            m = 0
            if i == j:
                m = 1
                print ( " ..",j,m)
            if m:
                res.append(i)
    return res

print(intersect2(list1,list2))
      

    
