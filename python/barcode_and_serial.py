import evdev
from evdev import InputDevice, categorize  # import * is evil :)
from time import sleep
import serial
devices = [evdev.InputDevice(fn) for fn in evdev.list_devices()]
devloc=""
for device in devices:
     if (device.name=="WIT Electron Company WIT 122-UFS V7.03"):
         devloc = device.fn
dev = InputDevice(devloc)
ser = serial.Serial('//dev/ttyUSB0', 9600) # Establish the connection on a specific port
def mapBattery(batno):
    dict = {'1': "500&-200&", '2': "-300&600&", '3': "-500&500&", '4': "1000&-1000&",'5': "100&-100&"}
    try:
        sendstr=dict[batno]
        ser.write(sendstr.encode())# Convert the decimal number to ASCII then send it to the Arduino
        print (ser.readline() )# Read the newest output from the Arduino
    except KeyError:
         print("No such key number exists!")
#https://stackoverflow.com/a/19757397
# Provided as an example taken from my own keyboard attached to a Centos 6 box:
scancodes = {
    # Scancode: ASCIICode
    0: None, 1: u'ESC', 2: u'1', 3: u'2', 4: u'3', 5: u'4', 6: u'5', 7: u'6', 8: u'7', 9: u'8',
    10: u'9', 11: u'0', 12: u'-', 13: u'=', 14: u'BKSP', 15: u'TAB', 16: u'Q', 17: u'W', 18: u'E', 19: u'R',
    20: u'T', 21: u'Y', 22: u'U', 23: u'I', 24: u'O', 25: u'P', 26: u'[', 27: u']', 28: u'CRLF', 29: u'LCTRL',
    30: u'A', 31: u'S', 32: u'D', 33: u'F', 34: u'G', 35: u'H', 36: u'J', 37: u'K', 38: u'L', 39: u';',
    40: u'"', 41: u'`', 42: u'LSHFT', 43: u'\\', 44: u'Z', 45: u'X', 46: u'C', 47: u'V', 48: u'B', 49: u'N',
    50: u'M', 51: u',', 52: u'.', 53: u'/', 54: u'RSHFT', 56: u'LALT', 100: u'RALT'
}
finstr=""
for event in dev.read_loop():
    if event.type == evdev.ecodes.EV_KEY:
        data = evdev.categorize(event)  # Save the event temporarily to introspect it
        if data.keystate == 1:  # Down events only
            key_lookup = scancodes.get(data.scancode) or u'UNKNOWN:{}'.format(data.scancode)  # Lookup or return UNKNOWN:XX
            if key_lookup != 'CRLF':
                if key_lookup!="LSHFT":
                    finstr+=format(key_lookup)
            else:
                if finstr.isdigit():
                    mapBattery(finstr)
                print(format(finstr))
                finstr=""



