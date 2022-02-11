"""
basic udp server
We'll add mqtt stuf in a few minutes
This will instruct all listners to send their status 

"""
import socket
import time
import paho.mqtt.client as mqtt
import json

MQTT_TOPIC = "/udp/request"
MQTT_SERVER="127.0.0.1"
MQTT_SERVER="pi4001"

server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) # UDP
server.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
server.settimeout(0.2)

id = 1234

server.bind(("",44444))
hostname = socket.gethostname()
hd = {}
hd["pi4001"] = "1234"


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(MQTT_TOPIC)
    print("Subscribed to " + MQTT_TOPIC)



def on_message(client, userdata, msg):
    if msg.topic == MQTT_TOPIC:
        jmes = {}
        print("Got"+ msg.topic+"->>"+str(msg.payload) + "<<")
        inmsg = str(msg.payload.decode("utf-8","ignore"))
        print ("Message received type :"),
        print (type(inmsg))
        print ("Message received  :" ),
        print (inmsg)
        try:
            global jmes
            jmes = json.loads(str(msg.payload))
        except:
            print ("invalid json")
        if "hostname" in jmes.keys():
            print ("Message received from :"),
            print (jmes["hostname"])
            hname = jmes["hostname"]
            if "ipaddr" in jmes.keys():
                hip = jmes["ipaddr"]
                if hname not in hd.keys():
                    hd[hname]=hip
                    print (" New host found %s at %s"%(hname,hip))
                else:
                    hdi = hd[hname]
                    if hdi == hip:
                        pass
                    else:
                        print (" Address change for %s from %s to %s" %(hname,hdi[1],hip)) 
                        

print(" starting mqtt sniffer\n")
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_SERVER, 1883, 60)
print(" starting udp send\n")
mtime = 5
tcount = 600
uport = 37030
while True:
    client.loop()
    if tcount>mtime:
        message = "{\"cmd\":\"getstatus\",\"replyto\":\"%s\",\"topic\":\"%s\",\"id\":%d}" % (hostname,MQTT_TOPIC,id)
        rc = server.sendto(message,('<broadcast>',uport))
        print ( "sent message [%s] topic [%s] rc %d" % (message, MQTT_TOPIC, rc))
        id = id + 1
        tcount = 0
    tcount += 1
    
    time.sleep(0.1)
    
    





