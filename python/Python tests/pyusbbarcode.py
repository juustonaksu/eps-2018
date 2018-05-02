from  keyboard_alike import reader

reader = reader.Reader(0x05fe, 0x1010, 24, 8,should_reset=False, debug=True)
reader.initialize()
while True:
	print(reader.read().strip())
