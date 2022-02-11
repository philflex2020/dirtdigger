from time import sleep
import time
import threading
import random
from flask import *
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

pulse = 0
distance = 0
rpm = 0.00
speed = 0.00
wheel_c = 2 # unit meter
multiplier = 0
elapse = 0.00
addme = 0
start = time.time()

hall = 21
GPIO.setup(hall, GPIO.IN, pull_up_down = GPIO.PUD_UP)

speedval = 0.0

app = Flask(__name__)

@app.route('/')
def index():
    global speedval
    return render_template('random-gauge.html', value_at_refresh = speedval)

# the following route will send JSON data streams to client browser
# variables are GLOBAL as we are grabbing them from other external functions/classes in background thread
@app.route('/stream')
def stream():
    global speedval

    def generate_random():
        while True:
            stream_state = {
                            'num_json' : speedval
                           }

            # stream the data to web application once sleep second
            yield 'data:{0}\n\n'.format(json.dumps(stream_state))
            sleep(0.1)

    return Response(generate_random(), mimetype='text/event-stream')

def get_pulse(channel):
    global speedval,pulse,start,distance,multiplier,rpm
    cycle = 0
    pulse+=1
    cycle+=1
    if pulse > 0:
        elapse = time.time() - start
        pulse -=1
    if cycle > 0:
        distance += wheel_c
        cycle -= 1

    multiplier = 3600/elapse
    speedval = (wheel_c*multiplier)/1000
    rpm = 1/elapse *60

    start = time.time()

if __name__ == "__main__":
    try:
        print 'Press CTRL+C to quit\n'
        print 'Starting Flask web server...'
        GPIO.add_event_detect(hall,GPIO.FALLING,callback = get_pulse,bouncetime=20)
        app.run(host='0.0.0.0',debug = True,port= 9000)
	
    except KeyboardInterrupt:
        print '\nShutting down...'
   
    print
