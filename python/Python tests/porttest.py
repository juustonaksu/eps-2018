import serial.tools.list_ports
ports= list(serial.tools.list_ports.comports())
for p in ports:
    if(p.product=="USB2.0-Serial"):
        arduloc=p.device
        print (arduloc, p.product)
