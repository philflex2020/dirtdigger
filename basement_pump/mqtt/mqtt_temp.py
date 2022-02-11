import os
import time
import sys
#import Adafruit_DHT as dht
import paho.mqtt.client as mqtt
import json



MQTT_HOST = '127.0.0.1'
ACCESS_TOKEN = 'DHT22_DEMO_TOKEN'

# Data capture and upload interval in seconds. Less interval will eventually hang the DHT22.
INTERVAL=2
sensor_data = {'temperature': 0, 'humidity': 0}
next_reading = time.time()
client = mqtt.Client()

path='v1/devices/me/telemetry'
path='test_channel'

# Set access token
#client.username_pw_set(ACCESS_TOKEN)

# Connect to host using default MQTT port and 60 seconds keepalive interval
client.connect(MQTT_HOST, 1883, 60)

client.loop_start()

try:
    while True:
        #humidity,temperature = dht.read_retry(dht.DHT22, 4)
        humidity,temperature = (75.4, 34.56)
        humidity = round(humidity, 2)
        temperature = round(temperature, 2)

        print(u"Temperature: {:g}\u00b0C, Humidity: {:g}%".format(temperature, humidity))
        sensor_data['temperature'] = temperature
        sensor_data['humidity'] = humidity
        
        # Sending humidity and temperature data to MQTT host
        client.publish(path, json.dumps(sensor_data), 1)

        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)

except KeyboardInterrupt:
    pass



client.loop_stop()

client.disconnect()
