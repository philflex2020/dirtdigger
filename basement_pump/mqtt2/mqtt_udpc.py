"""
basic udp client
We'll add mqtt stuf in a few minutes
We want to listen for a request to publish some information about ourselves
like {"replyto":"test.mosquito.org","topic":"mqtt2/ident/info","id":1235,"seq":2234}
we send back
{ "id":1234,"seq":2234,"host":"pine64-003", "hostip":"192.168.123.45","system_type":"whatever"}
"""

import socket
import json
from urllib import urlopen
import paho.mqtt.publish as publish
import requests

def msgjson(data):
    try:
        mjs = json.loads(data)
        print(" got json")
        res = "OK"
    except ValueError:
        print (" not json")
        res = "Error"
        return data
    return mjs, res

uport = 37030
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
client.bind(("",uport))

print(" mqtt_udpc running on port %d" % uport)

while True:
    data,addr = client.recvfrom(1024)
    print ( "received message :[%s]" % data)
    mjs, res = msgjson(data)
    print ( "decode message :[%s]" % res)
    if res == "OK":
        print res
        myhostname = socket.gethostname()
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
        ipaddr = s.getsockname()[0]
        #ipaddr = socket.gethostbyname_ex(hostname)[2]
        url = 'http://myip.dnsomatic.com'
        info = requests.request('GET',url)
        extaddr = info.text
        # create the response
        rsp = {}
        rtopic = ""
        if "topic" in mjs.keys():
            rtopic = mjs["topic"]
        rhost = ""
        if "replyto" in mjs.keys():
            rhost = mjs["replyto"]
        rid = 0
        if "id" in mjs.keys():
            rid  = mjs["id"]
        rseq = 100
        if "seq" in mjs.keys():
            rsp["seq"] = mjs["seq"]
        rsp["topic"] = rtopic
        rsp["seq"] = rseq
        rsp["id"] = rid
        rsp["hostname"] = myhostname
        rsp["ipaddr"] = ipaddr
        rsp["extaddr"] = extaddr
    
        msg = json.dumps(rsp);
        print msg
        publish.single(rtopic,msg,hostname=rhost)

#http://www.steves-internet-guide.com/using-javascript-mqtt-client-websockets/
#https://icircuit.net/arduino-getting-started-mqtt-using-esp32/2138    





