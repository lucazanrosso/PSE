# Winter Homework 2018

The purpose of this program is to create a software for controlling a rehabilitation device that is based on the model-view-controller architectural pattern.

The program provides that the patient can use the device remotely. Communication takes place via client-server and a stream socket connection.

The screen shows the movement of the device that the patient is moving.

### Server

To compile and run the server, type the following lines into the terminal:
```
gcc -Wall -o haptic *.c *.h -lpthread
./haptic -30 20 12 20
```
- The first and second parameters represent the barriers that the patient cannot overcome.
- The third and fourth parameters indicate the refresh rate of the controller and view components.

### Client

The client is located inside the client folder. To compile and run the client, type the following lines into the terminal: 
```
gcc -Wall -o client client.c
./client
```

### Simulation

The simulation of the data acquisition is done through txt files. The files contain, at fixed time intervals, the displacement of the device dedicated to rehabilitation. There are three examples, two for the server and one for the client.
