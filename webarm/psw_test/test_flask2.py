from flask import Flask
from flask import request

import time

servo_min = 150
servo_max = 600

app = Flask(__name__)

@app.route('/')
def web_interface():
    html=open("index2.html")
    response = html.read().replace('\n', '')
    html.close()
    return response

@app.route("/set_move1")
def set_move1():
    value=request.args.get("value")
    print "Received " + value
#    PCA9685.set_pwm(0,0,int(speed))
    return "Received " + value

@app.route("/set_servo1")
def set_servo1():
    speed=request.args.get("speed")
    print "Received " + str(speed)
#    PCA9685.set_pwm(0,0,int(speed))
    return "Received " + str(speed)

@app.route("/set_servo2")
def set_servo2():
    speed=request.args.get("speed")
    print "Received " + str(speed)
#    PCA9685.set_pwm(1,0,int(speed))
    return "Received " + str(speed)

@app.route("/set_servo3")
def set_servo3():
    speed=request.args.get("speed")
    print "Received " + str(speed)
#    PCA9685.set_pwm(2,0,int(speed))
    return "Received " + str(speed)

@app.route("/set_servo4")
def set_servo4():
    speed=request.args.get("speed")
    print "Received " + str(speed)
#    PCA9685.set_pwm(3,0,int(speed))
    return "Received " + str(speed)

@app.route("/set_servo5")
def set_servo5():
    speed=request.args.get("speed")
    print "Received " + str(speed)
#    PCA9685.set_pwm(4,0,int(speed))
    return "Received " + str(speed)

@app.route("/set_servo6")
def set_servo6():
    speed=request.args.get("speed")
    print "Received " + str(speed)
#    PCA9685.set_pwm(5,0,int(speed))
    return "Received " + str(speed)


#def index():
#    return 'Hello Flask'


if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=8181)

