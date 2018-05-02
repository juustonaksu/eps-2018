from megapi import *

def onForwardFinish(slot):
	print("Hi")
	sleep(0.4);
	bot.stepperMotorMove(slot,3000,-3000,onBackwardFinish);

def onBackwardFinish(slot):
	sleep(0.4);
	print slot;
	bot.stepperMotorMove(slot,3000,3000,onForwardFinish);

if __name__ == '__main__':
	bot = MegaPi()
	bot.start()
	sleep(1);
	onForwardFinish(1);
	while 1:
		continue;
