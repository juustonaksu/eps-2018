from megapi import *
import sys, signal

def signal_handler(signal, frame):
	        bot.stepperMotorRun(1,0)
                print("Bye!")
		sys.exit(0)

def onForwardFinish(slot):
	print("Hi")
	sleep(0.8);
	bot.stepperMotorMove(slot,10000,-10000,onBackwardFinish);

def onBackwardFinish(slot):
	print("Ho")
	sleep(0.8);
	print slot;
	bot.stepperMotorMove(slot,16000,10000,onForwardFinish);

if __name__ == '__main__':
	signal.signal(signal.SIGINT, signal_handler)
	bot = MegaPi()
	bot.start('/dev/ttyUSB0')
	sleep(1);
	onForwardFinish(1);
	while 1:
		continue;
