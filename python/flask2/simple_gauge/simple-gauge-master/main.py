from time import sleep
import threading
import random
from flask import *


num = 0.0

# Thread for handling data stream
class produceRandomNumber(threading.Thread):
 
    def __init__(self):
        super(produceRandomNumber, self).__init__()
        self.terminated = False
        self.start()
 
    def run(self):
        global num
     
        print '\n starting thread'
        while not self.terminated:
        
	    num = random.uniform(0,1)
	    sleep(0.5)
	
            if self.terminated:
	        break
        print 'thread stopped'


# run all threads in the background
print 'Initialize thread'
produceRandomNumber = produceRandomNumber()


app = Flask(__name__)

@app.route('/')
def index():
    global num
 
    return render_template('random-gauge.html', value_at_refresh = num)

# the following route will send JSON data streams to client browser
# variables are GLOBAL as we are grabbing them from other external functions/classes in background thread
@app.route('/stream')
def stream():
    global num

    def generate_random():
        while True:
            stream_state = {
                            'num_json' : num
                           }

            # stream the data to web application once sleep second
            yield 'data:{0}\n\n'.format(json.dumps(stream_state))
            sleep(2)

    return Response(generate_random(), mimetype='text/event-stream')
    
if __name__ == "__main__":
    try:
	print 'Press CTRL+C to quit\n'
	print 'Starting Flask web server...'
	app.run(host='0.0.0.0',debug = True,port= 9000)
	
    except KeyboardInterrupt:
            print '\nShutting down...'
    produceRandomNumber.terminated = True
    produceRandomNumber.join()
   
    print
