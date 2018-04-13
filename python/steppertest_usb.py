from megapi import *

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
	bot = MegaPi()
	bot.start('/dev/ttyUSB0')
	sleep(1);
	onForwardFinish(1);
	try:
		while 1:
			continue;
	except KeyboardInterrupt:
		bot.stepperMotorRun(1,0)
		sleep(2)
		print("Bye!")
