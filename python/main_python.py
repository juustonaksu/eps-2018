import evdev
from evdev import InputDevice, categorize  # import * is evil :)
from time import sleep
import serial
import serial.tools.list_ports
import RPi.GPIO as GPIO
import time
# use Raspberry Pi board pin numbers
GPIO.setmode(GPIO.BOARD)
 
# set up the GPIO channels for registering battery charger blinks
GPIO.setup(13, GPIO.IN) #Battery charger number 2
GPIO.setup(18, GPIO.IN) #Battery charger number 1

#Maps battery number to "coordinates"
def mapBattery(batno):
    print("Searching battery number",batno," co-ordinates")
    dict = {'1': "950&1970&", '2': "950&4160&",
            '3': "920&6200&", '4': "800&5000&", '5': "800&7000&"}
    try:
        sendstr = dict[batno]
        # Convert the decimal number to ASCII then send it to the Arduino
        ser.write(sendstr.encode())
        print("Co-ordinates sent to Arduino!")
    except KeyError:
        print("No such key number exists!")
# ASCII table for decoding barcode scanner input, from https://stackoverflow.com/a/19757397
scancodes = {
    # Scancode: ASCIICode
    0: None, 1: u'ESC', 2: u'1', 3: u'2', 4: u'3', 5: u'4', 6: u'5', 7: u'6', 8: u'7', 9: u'8',
    10: u'9', 11: u'0', 12: u'-', 13: u'=', 14: u'BKSP', 15: u'TAB', 16: u'Q', 17: u'W', 18: u'E', 19: u'R',
    20: u'T', 21: u'Y', 22: u'U', 23: u'I', 24: u'O', 25: u'P', 26: u'[', 27: u']', 28: u'CRLF', 29: u'LCTRL',
    30: u'A', 31: u'S', 32: u'D', 331
    : u'F', 34: u'G', 35: u'H', 36: u'J', 37: u'K', 38: u'L', 39: u';',
    40: u'"', 41: u'`', 42: u'LSHFT', 43: u'\\', 44: u'Z', 45: u'X', 46: u'C', 47: u'V', 48: u'B', 49: u'N',
    50: u'M', 51: u',', 52: u'.', 53: u'/', 54: u'RSHFT', 56: u'LALT', 100: u'RALT'
}

# Read barcode raw input, part from https://stackoverflow.com/a/19737619 
def readCode():
    print("Waiting for barcode to be read")
    finstr = ""
    for event in dev.read_loop():
        if event.type == evdev.ecodes.EV_KEY:
            # Save the event temporarily to introspect it
            data = evdev.categorize(event)
            if data.keystate == 1:  # Down events only
                key_lookup = scancodes.get(data.scancode) or u'UNKNOWN:{}'.format(
                    data.scancode)  # Lookup or return UNKNOWN:XX
                #Raw barcode scanner input contains CRLF and LSHIFT on the beginning and at end
                if key_lookup != 'CRLF':
                    if key_lookup != "LSHFT":
                        finstr += format(key_lookup)
                else:
                    #We have read the full barcode, now inspect if it's a (battery) number
                    if finstr.isdigit():
                        #Mao found battery number to coords
                        mapBattery(finstr)
                        #Move on to wait for serial input
                        readSerial()
                    #This is only for debugging the read string
                    print("DEB",format(finstr))
                    finstr = ""
#Check for full batteries in the system. 
def fullBattery():
    while True:
        batt2= checkBattery(13,"2")
        batt1 =checkBattery(18,"1")
        if(batt1 == True):
            mapBattery("1")
            readCode()
            break
        elif(batt2==True):
            mapBattery("2")
            readCode()
            break
        else:
            print("No battery full")
#Inspect battery charger LED blinking patterns to determine if the battery is full or not
def checkBattery(pin, batteryno):
    #Count how many while-rounds the led has been on
    oncounter = 0
    #Iteration variable
    it=0
    while it <12:
        input_value = GPIO.input(pin)
        #If the led stays on for more than 8 rounds, it means it's not blinking (so the battery is full)
        if(input_value and oncounter > 8):
            #Battery fully charged
            print("Battery ",batteryno," is full")
            oncounter=oncounter+1
            #Return true for the calling function 
            return True
            break
        elif(input_value):
            #Empty slot or battery over 75% (led is either blinking or fully on)
            oncounter= oncounter+1	
        else:
            #led is not on
            oncounter=0
        it=it+1
        time.sleep(0.1)
    return False
# Read serial messages coming from Arduino/Mega Pi
def readSerial():
    req=""
    print("Waiting for Arduino serial message")
    req = ser.readline()
    req=req.rstrip()
    req=req.decode("utf-8")
    print("Arduino sent: ",req,"\n\n***********************\n")
    #Arduino will send F when requesting for a full battery
    if(req=="F"):
        fullBattery()       
        
if __name__ == "__main__":
    #Find where the barcode scanner is hooked, so the location does not need to be inserted manually
    devices = [evdev.InputDevice(fn) for fn in evdev.list_devices()]
    devloc = ""
    for device in devices:
        if (device.name == "WIT Electron Company WIT 122-UFS V7.03"):
            devloc = device.fn
            print("Barcode scanner is at ",devloc)
    dev = InputDevice(devloc)
    #Find the port where MegaPi is connected, so it's number does not need to be inserted manually
    ports= list(serial.tools.list_ports.comports())
    for p in ports:
        if(p.product=="USB2.0-Serial"):
            arduloc=p.device
            print ("Arduino is at ",arduloc, p.product,"\n")
            ser = serial.Serial(arduloc, 9600)
    #Go to wait for Arduino/MegaPi serial message
    readSerial()


