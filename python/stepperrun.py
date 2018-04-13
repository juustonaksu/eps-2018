from megapi import *

if __name__ == '__main__':
	bot = MegaPi()
	bot.start()
	bot.stepperMotorRun(1,0);
	sleep(1);
	while 1:
		bot.stepperMotorRun(1,-3000);
		sleep(10);
		bot.stepperMotorRun(1,0);
		sleep(4);
