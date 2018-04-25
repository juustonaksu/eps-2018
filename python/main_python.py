import evdev
from evdev import InputDevice, categorize  # import * is evil :)
from time import sleep
import serial
import serial.tools.list_ports
import RPi.GPIO as GPIO
import time
# to use Raspberry Pi board pin numbers
GPIO.setmode(GPIO.BOARD)
 
# set up the GPIO channels - one input and one output
GPIO.setup(7, GPIO.IN)
ser=""
dev=""
def mapBattery(batno):
    dict = {'1': "500&-200&", '2': "-300&600&",
            '3': "-500&500&", '4': "1000&-1000&", '5': "100&-100&"}
    try:
        sendstr = dict[batno]
        # Convert the decimal number to ASCII then send it to the Arduino
        ser.write(sendstr.encode())
        #ser.readline()

    except KeyError:
        print("No such key number exists!")
# https://stackoverflow.com/a/19757397
scancodes = {
    # Scancode: ASCIICode
    0: None, 1: u'ESC', 2: u'1', 3: u'2', 4: u'3', 5: u'4', 6: u'5', 7: u'6', 8: u'7', 9: u'8',
    10: u'9', 11: u'0', 12: u'-', 13: u'=', 14: u'BKSP', 15: u'TAB', 16: u'Q', 17: u'W', 18: u'E', 19: u'R',
    20: u'T', 21: u'Y', 22: u'U', 23: u'I', 24: u'O', 25: u'P', 26: u'[', 27: u']', 28: u'CRLF', 29: u'LCTRL',
    30: u'A', 31: u'S', 32: u'D', 33: u'F', 34: u'G', 35: u'H', 36: u'J', 37: u'K', 38: u'L', 39: u';',
    40: u'"', 41: u'`', 42: u'LSHFT', 43: u'\\', 44: u'Z', 45: u'X', 46: u'C', 47: u'V', 48: u'B', 49: u'N',
    50: u'M', 51: u',', 52: u'.', 53: u'/', 54: u'RSHFT', 56: u'LALT', 100: u'RALT'
}


def readCode():
    finstr = ""
    for event in dev.read_loop():
        if event.type == evdev.ecodes.EV_KEY:
            # Save the event temporarily to introspect it
            data = evdev.categorize(event)
            if data.keystate == 1:  # Down events only
                key_lookup = scancodes.get(data.scancode) or u'UNKNOWN:{}'.format(
                    data.scancode)  # Lookup or return UNKNOWN:XX
                if key_lookup != 'CRLF':
                    if key_lookup != "LSHFT":
                        finstr += format(key_lookup)
                else:
                    if finstr.isdigit():
                        mapBattery(finstr)
                        readSerial()
                    print(format(finstr))
                    finstr = ""
def fullBattery():
    oncounter = 0
    while oncounter <10:
        input_value = GPIO.input(7)
        if(input_value and oncounter > 8):
            #Battery fully charged
            print("Battery full")
            oncounter=oncounter+1
            mapBattery(1)
            readCode()
        elif(input_value):
            #Empty slot or battery over 75%
            oncounter= oncounter+1	
        else:
            print("M")
            oncounter=0
        time.sleep(0.1)
def readSerial():
    req = ser.readline()
    if(req="Full"):
        fullBattery()
def initall():
    devices = [evdev.InputDevice(fn) for fn in evdev.list_devices()]
    devloc = ""
    for device in devices:
        if (device.name == "WIT Electron Company WIT 122-UFS V7.03"):
            devloc = device.fn
    dev = InputDevice(devloc)
    # Establish the connection on a specific port
    ports= list(serial.tools.list_ports.comports())
    for p in ports:
        if(p.product=="USB2.0-Serial"):
            arduloc=p.device
            print (arduloc, p.product)
            ser = serial.Serial(arduloc, 9600)
if __name__ == "__main__":
    initall()
    fullBattery()


