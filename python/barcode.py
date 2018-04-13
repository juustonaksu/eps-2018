import sys

fp= open('/dev/input/event3', 'rb')
while True:
	buffer= fp.read(8)
	for c in buffer:
		print raw_input(ord(c))

