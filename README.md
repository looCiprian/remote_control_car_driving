# Remote Control Car Driving
The project (first working release) was developed and released in one day

# How it works
The main project uses a Thrustmaster t-flight hotas x joystick console connected to a Debian distro in order to catch the input events using Python. Roll and throttle events are sent to the Wemos which hosts a UDP server. Wemos board is connected to a l298n motor driver in order to control the electric power from the battery to motors. Both Wemos and Joystick need to be connected on the same network.

***Please read [Release history](#Release-history) before continue reading expecially [V0.1](#V0.1)***

# NodeJS vs Thrustmaster t-flight hotas x Joystick
Since not every person has an ariplane joystick a web interface using nodeJS was developed.

# How to  use
## Set up Wi-Fi connection on Wemos
For the first use, Wemos will spawn a Wi-Fi network named "Wemos-rover". You need to join the network and browse 192.168.4.1. Set your ssid and password from the web interface. Wemos will then reboot and join your home Wi-Fi network.

## Set up Thrustmaster t-flight hotas x Joystick

1. Connect power to Wemos

2. List events
```bash
$ joystick.py -l 
```

3. Set event and wemos_ip ([Read how to get the ip without serial port](#Problem-solving-challenge)) and wemos_port
```bash
$ joystick.py <input_event> <wemos_ip> <wemos_port>
```

4. Have fun :)

## Set up NodeJs if you don't have the joystick

1. Connect power to Wemos

2. Install needed packages
```bash
$ cd web_joystick_rover; npm install
$ node web_joystick.js
```

3. Browse web interface on your ip:3000

4. Set Wemos ip [Read how to get the ip without serial port](#Problem-solving-challenge)

5. Have fun :)


# Demo

![Car](https://github.com/looCiprian/remote_control_car_driving/blob/main/doc/car.gif?raw=true)

In the same way it is possible to control other types of "cars" like rovers

![Rover](https://github.com/looCiprian/remote_control_car_driving/blob/main/doc/rover.gif?raw=true)

# Problem solving challenge
## The problem
In order to comunicate with the Wemos we need to know its ip, since it will start up a UDP server in order to receive commands. How do we know its ip?

## Logic answer
Add a display and print the ip on it

## Why cannot logic answer be applied?
Wemos hasn't got enough pins to control a motor driver and a display

## Creative answer
Since Wemos has a built in led, we can comunicate the address using light signals XD

### How it works
We assume that home address is a private ip with the following syntax 192.168.x.x . So the first six digits can be obmitted.
In order to comunicate the last two portions of the ip, the address is splitted and last two values are taken (es. 192.168.1.43 --> object[0]=1, object[1]=43)
Then, each value is splitted by digit (es. object[1]=43 --> is splitted and becomes objectSplited[0]=4 objectSplitted[1]=3), these numbers indicate how many times the led needs to blink.

Final example.
IP=192.168.1.43
1. Led blinks very quickly for 20 times and way 3 seconds (to comunicate the start)
2. Led starts to comunicate the first number (1) by blinking one time and waits 2 seconds
3. Led blinks very quickly for 10 times and waits 3 seconds (to comunicate the "dot" between 1 and 43)
4. Led starts to comunicate the first number of the second value (4) by blinking four times and waits 2 seconds (to comunicate the end of the first number)
5. Led starts to comunicate the second number of the second value (3) by blinking three times and waits 2 seconds
6. Led blinks very quickly for 20 times (to comunicate the end)

## Demo
IP: 192.168.1.25

![Rover_ip](https://github.com/looCiprian/remote_control_car_driving/blob/main/doc/rover_ip.gif?raw=true)

# TODO List
The first release was developed only to make a working project. Below a list of feautures to implement:
- [ ] Implement checksum
- [ ] Ensuring packet order
- [x] Add Wi-Fi manager library
- [x] Implement remote telemetry
- [ ] Add GPS module (Buy me a GPS module
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/donate?hosted_button_id=8EWYXPED4ZU5E))

# Release history

## V0.1
Since using the rover is funnier that a "normal" rc car, from this version (V0.1) commits involve only the rover. The .ino file for the car will ***not*** update anymore.