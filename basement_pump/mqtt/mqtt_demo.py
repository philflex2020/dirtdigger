import time
import paho.mqtt.client as paho
import threading 

broker="broker.hivemq.com"
broker="iot.eclipse.org"
broker="localhost"

#define callback
def on_message(client, userdata, message):
        time.sleep(1)
        print("received message =",str(message.payload.decode("utf-8")))

def pubThread(val, stop):
    global client
    print("thread publishing ")
    client.publish("house/bulb1","on")#publish
    time.sleep(4)

client= paho.Client("client-001") #create client object client1.on_publish = on_publish #assign function to callback client1.connect(broker,port) #establish connection client1.publish("house/bulb1","on")
######Bind function to callback

client.on_message=on_message
#####
print("connecting to broker ",broker)
client.connect(broker)#connect
client.loop_start() #start loop to process received messages
print("subscribing ")

client.subscribe("house/bulb1")#subscribe
time.sleep(2)
stop_thread=False

try:
    t = threading.Thread(target=pubThread, args=(1,lambda: stop_thread))
    t.start()

except KeyboardInterrupt:
        print 'quitting'

        stop_thread = True
        t.join()   
        client.disconnect()
        client.loop_stop()
#client.disconnect() #disconnect
#client.loop_stop() #stop loop

