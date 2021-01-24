import evdev
import socket
import json
import sys
from threading import Thread
import time
from prettytable import PrettyTable
import signal
from functools import partial

##### DEFAURT VALUE ####
wemos_ip = "192.168.1.13"
wemos_port = 4210
event = "/dev/input/event7"
########################

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP

def list_devices():
    print("Listing devices:")
    devices = [evdev.InputDevice(path) for path in evdev.list_devices()]
    for device in devices:
        print(device.path, device.name, device.phys)

def print_data(data):
    dataJson = json.loads(data)
    motor_table = PrettyTable()
    motor_table.field_names = ["Motor Name", "Direction", "Power"]
    for motor in dataJson['motors']:
        row = []
        row.append(motor['motor'])
        row.append(motor['direction'])
        row.append(motor['power'])
        motor_table.add_row(row)
    print(motor_table)

def receive_message():
    while True:
        global sock
        data, address = sock.recvfrom(4096)
        #print(data.decode('utf-8'))
        print_data(data.decode('utf-8'))
        time.sleep(0.01)

def send_message(message):
    global wemos_ip
    global wemos_port
    global sock
    message = json.dumps(message)
    sock.sendto(bytes(message, "utf-8"), (wemos_ip, wemos_port))

def execute():
    global event
    device = evdev.InputDevice(event)
    for event in device.read_loop():
        
        if (event.value != 0):
            if (event.code == 0): #joistick destra
                #print("Sending message")
                send_message({"code":event.code,"value":event.value})
            if (event.code == 1): #joistick avanti/indietro
                #print("Sending message")
                send_message({"code":event.code,"value":event.value})
            if (event.code == 2): #joistick manetta
                #print("Sending message")
                send_message({"code":event.code,"value":event.value})

def main():
    if(len(sys.argv) == 2 and sys.argv[1] == '-l'):
        list_devices()

    if(len(sys.argv) < 4):
        print("Usage: " + sys.argv[0] + ";")
        print("[+] Mode listing: " + sys.argv[0] + " -l")
        print("[+] Mode executing:" + sys.argv[0] + " <event e.g /dev/input/event7> <wemos_ip> <wemos_port>")
        exit(1)

    global event
    global wemos_ip
    global wemos_port
    event = sys.argv[1]
    wemos_ip = sys.argv[2]
    wemos_port = int(sys.argv[3])
    listener = Thread(target=receive_message)
    listener.start()
    print("[+] Listener is running")
    execute()
    print("[+] Execution is running")

if __name__=="__main__":
    main()