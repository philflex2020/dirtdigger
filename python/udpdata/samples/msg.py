
import json
up = 1
num = 1
msg ='{{{{{}}}}}'
msg ='{"a":{"b":{"c":{"d":{}}}}}rem{{}}'


for i in msg[1:]:
    if i == '{':
        up += 1
    elif i == '}':
        up -= 1
    num += 1
    if up == 0:
        print("done")
        break
xx = json.loads(msg[:num])


print (num, msg[num:], xx)
