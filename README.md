# Car Control
The goal of this project is to create an applictation that will read inputs from multiple sources and control the piracer vehicle. Also this application aims to provide vehicle's status to other applications via CommonAPI.

# Tabel of Contents
- [Introduction](#introduction)
- [User Functionallity](#user-functionallity)
- [CommonAPI](#commonapi)
- [Dependencies](#dependencies)
- [References](#references)

# Introduction

The following tree shows the project structure. <br>
This application uses Convesa's Common API to connect the PiRacer's drivetrain to the on-board network. <br>

```bash
car_control
├── build 
├── fidl
│   ├── car_control.fidl 
│   ├── car_control.fdepl 
├── include 
│   ├── CarControlStubImpl.hpp 
│   ├── GamePad.hpp 
│   ├── PiRacer.hpp 
├── piracer 
│   ├── gamepads.py
│   ├── vehichles.py
├── scripts 
│   ├── Build.sh 
│   ├── Run_Client.sh 
│   ├── Run_Services.sh 
├── src 
│   ├── CarControlClient.cpp 
│   ├── CarControlService.cpp 
│   ├── CarControlStubImpl.cpp 
│   ├── GamePad.cpp 
│   ├── PiRacer.cpp 
├── src-gen/v0/commonapi 
│   ├── core 
│   │   ├── CarControl.hpp 
│   │   ├── CarControlProxy.hpp 
│   │   ├── CarControlProxyBase.hpp 
│   │   ├── CarControlStub.hpp 
│   │   ├── CarControlStubDefault.hpp 
│   ├── someip 
│   │   ├── CarControlSomeIPDeployment.cpp 
│   │   ├── CarControlSomeIPDeployment.hpp 
│   │   ├── CarControlSomeIPProxy.cpp 
│   │   ├── CarControlSomeIPProxy.hpp 
│   │   ├── CarControlSomeIPStubAdapter.cpp 
│   │   ├── CarControlSomeIPStubAdapter.hpp 
├── CMakeLists.txt 
.github 
├── workflows 
```

The application uses the python modules gamepads.py and vehicles.py from SEA:ME (see: [[1]](#references)) and Convesa's CommonAPI that runs on C++. To connect python and C++, the Boost.Python libary is used. Boost.Python is a C++ library which enables seamless interoperability between C++ and the Python programming language. <br>
Read the follwing [[2]](#references) if you want to know more about it. <br>

## User Functionallity
This section describes the application's user functionallity. <br>
The car's drivetrain is controlled by Gamepad and Head Unit inputs. <br>
Steering and acceleration are set with the joysticks. <br>
Changing lanes can be indicated by a button push of L1 (left indicator) or R1 (right indicator). <br>
The car's gear is selected by the X (Neutral), A (Drive), B (Park), and Y (Reverse) buttons. <br>
If the car is in Drive ("D"), the car will drive forward - In Reverse ("R"), the car will drive backwards. 
The right joystick needs to be pushed in the corresponding direction to accelerate. Hence, the car will not move if the Joystick is pushed backwards while "D is selected. The same rule apply for the gear selection reverse ("R") just in reverse. Last but not least, in Park ("P") & Neutral ("N"), the car will not move at all. <br>

<img src="https://github.com/Lagavulin9/DES_3_Head-Unit/assets/56917072/1c7370b6-bff3-4b86-ba67-9e6023b147a5" width=75%>

| Button | Action |
| ------ | ------ |
| Left Joystick | Steering |
| Right Joystick | Throttle |
| L1 | Left Indicator|
| R1 | Right Indicator |
| X | Neutral ("N")|
| A | Drive ("D")|
| B | Park ("P")|
| Y | Reverse ("R")|

## CommonAPI
The communication between the Head Unit and the Car Control is based on the vSOME/IP protocol. <br>
The application "Head_Unit" (in the following titled as "Head Unit") acts as a client and application "Car_Control" (titled as "Car Control") runs as a services. <br>
The vSOME/IP interface of the application car control is definfied in the .fidl file. <br>
The interface contains two attributes "indicator" and "gear" as well as a method called "gearSelectionHeadUnit". <br>
A client can subscribe to the attributes to get notified about changes. <br>
By invoking the method "gearSelectionHeadUnit", the client (in this case Head Unit) can request the car's gear via synchronous call. <br>

```ruby
package commonapi

interface CarControl {
    version { major 0 minor 1 }

    attribute String indicator readonly
    attribute String gear readonly
    
    method gearSelectionHeadUnit {
        in { 
            String selectedGear
        }
        out { 
            Boolean accepted
        }
    }
}
```

The Head Unit can invoke the method "gearSelectionHeadUnit" on the service side to set the gear. <br>
In the Request-Resonse communication, the Head Unit will sends a request with the selected gear ("D", "R", "P", "N") to the service. The service will response with "true" if the gear change was accepted.<br>
```cpp
 --------------------------- 
Result of synchronous call: 
   callStatus: SUCCESS
   Input Gear:  R
   Output Gear Change accepted: 1
 ---------------------------
```
The service receives the request and checks if the car is in motion. If the car is not in motion, the car will accept the gear change. 
```cpp
 ---------------------------
Head Unit invoked service method & tries to set gear.
  Input Gear: R
->Gear change accepted.
 ---------------------------
 ```
Next, the service will set the attribute "Gear" to the selected gear. All clients which subscriped to this attribute will recieve a notification about the change.<br>
```cpp
 ---------------------------
Service Notification: 'Gear' changed to : R
 ---------------------------
```
A gear change request will be declined, if the car is still in motion. Therefore, the output of the client's synchronous call will false. <br>

```cpp
 ---------------------------
Result of synchronous call: 
   callStatus: SUCCESS
   Input Gear:  D
   Output Gear Change accepted: 0
 ---------------------------
```
The same rules applies for a gear change input made by the Gamepad. <br>
```cpp
 ---------------------------
GamePad tries to set gear. 
  Input Gear: R
->Gear change declined.
 ---------------------------
```
If the shoulder button L1 is pressed, the service will notify the client about the change of the attribute "Indicator". The attribute indicator toggles between the selected direction ("Left" in case of L1) and "None" if the button is released. <br>
The same rules applies for the shoulder button R1. <br>
```cpp
 ---------------------------
Service Notification: 'Indicator' changed to : Left
 ---------------------------
  ```
```cpp
 ---------------------------
Service Notification: 'Indicator' changed to : None
 ---------------------------
  ```

# Dependencies
- [Piracer-py](https://pypi.org/project/piracer-py/)
- [Boost.Python](https://www.boost.org/doc/libs/1_84_0/libs/python/doc/html/index.html)
- [vSOME/IP](https://github.com/COVESA/vsomeip) v3.4.10
- [CommonAPI Core Runtime](https://github.com/COVESA/capicxx-core-runtime) v3.2.3
- [CommonAPI SOME/IP Runtime](https://github.com/COVESA/capicxx-someip-runtime) v3.2.3

# References
[[1]](https://github.com/SEA-ME/piracer_py): SEA-ME Piracer <br>
[[2]](https://www.boost.org/doc/libs/1_83_0/libs/python/doc/html/index.html): Boost <br>
[vsomeip in 10 minutes](https://github.com/COVESA/vsomeip/wiki/vsomeip-in-10-minutes)<br>
[CommonAPI wiki](https://github.com/COVESA/capicxx-core-tools/wiki)
