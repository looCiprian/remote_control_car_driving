# Remote Control Car Driving
The project (first working release) was developed and released in one day

# How it works
The project uses a Thrustmaster t-flight hotas x joystick console connected to a Debian distro in order to catch the input events using Python. Roll and throttle events are sent to the Wemos which hosts a UDP server. Wemos board is connected to a l298n motor driver in order to control the electric power from the battery to motors. Both Wemos and Joystick need to be connected on the same network.

# Demo

![Car](https://github.com/looCiprian/remote_control_car_driving/blob/main/doc/car.gif?raw=true)

In the same way it is possible to control other types of "cars" like rovers

![Rover](https://github.com/looCiprian/remote_control_car_driving/blob/main/doc/rover.gif?raw=true)

# TODO List
The first release was developed only to make a working project. Below a list of feautures to implement:
- [ ] Implement checksum
- [ ] Ensuring packet order
- [x] Implement remote telemetry
- [ ] Add GPS module (Buy me a GPS module
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/donate?hosted_button_id=8EWYXPED4ZU5E))
