import time
try:
	import RPi.GPIO as GPIO
except RunTimeError:
     	print("Error importing RPi.GPIO ")

#pin 16
chan1 = GPIO23
#pin 17
chan2 = GPIO24


GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(chan1, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(chan1, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)


def my_callback_one(channel):
    print('Callback one  LCD mode 1')
    # todo set display to mode 1
    # if GPIO.input(chan1) or GPIO.input(chan2):
    #  stay in mode 1
    # 


def my_callback_two(channel):
    print('Callback two LCD MOde 2')
    # todo set display to mode 2



GPIO.add_event_detect(chan1, GPIO.BOTH)
GPIO.add_event_detect(chan2, GPIO.BOTH)
GPIO.add_event_callback(chan1, my_callback_one, bouncetime=200)
GPIO.add_event_callback(chan2, my_callback_two, bouncetime=200)

try:
        while 1:
                if GPIO.input(chan1) or GPIO.input(chan2):
                        pass
                #  stay in mode 1
                else:
                        print('Idle LCD Mode 3')

                # return to mode3
                time.sleep(1.0)
except KeyBoardInterrupt:
        pass

GPIO.cleanup()
