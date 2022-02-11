#/usr/bin/python3
import websockets
import asyncio

#from websocket import create_connection

import json
async def hello():
    
    #ws = websocket.WebSocket()
    uri ="ws://192.168.1.249/"
    async with websockets.connect("ws://192.168.1.249:81/") as ws:
        #ws =  create_connection ("ws://192.168.1.249/")
        myDict = {"sensor": "temperature", "identifier":"SENS123456789", "value":10, "timestamp": "20/10/2017 10:10:25"}
 
        #await ws.send("LED=on")
        await ws.send(json.dumps(myDict))
        result = await ws.recv()
        print(result)
        
asyncio.get_event_loop().run_until_complete(hello())
#ws.close()
