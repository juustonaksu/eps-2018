import RPi.GPIO as GPIO
import time
# to use Raspberry Pi board pin numbers
GPIO.setmode(GPIO.BOARD)
 
# set up the GPIO channels - one input and one output
GPIO.setup(7, GPIO.IN)
# input from pin 7
oncounter = 0
offcounter = 0
while True:
	input_value = GPIO.input(7)
	if(input_value and oncounter > 8):
		print("Battery fully charged")
		offcounter=0
		oncounter=oncounter+1
	elif(input_value):
		print("Empty slot or battery over 75%")
		offcounter=0
		oncounter= oncounter+1	
	elif(not input_value and offcounter > 10):
		print("Battery charging below 75%")
		offcounter = offcounter +1
		oncounter=0
	else:
		oncounter=0
		offcounter=offcounter+1
	time.sleep(0.1)
