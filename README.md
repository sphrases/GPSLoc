# GPSLoc
DISCONTINUED!
I do not work on this project anymore.

GPSLoc is an Arduino program to calculate the route of a boat back to shore if the transmitter looses connection.
I did my best to comment the code.

How It works:
the boats GPS module saves the initial GPS values when the boat is turned on. 
With a phone or via the Serial monitor you can see if the GPS has a stable connection. 
The Arduino has three control states. 
  0. FULLCONTROLLED - The Arudino is in full control over speed and steering of the boat.
  1. HALFCONTROLLED - The User can control Speed, and the Arduino is controlling the steering.
  2. NOTCONTROLLED - The user is in full controll of the boat.

For testing we used a switch on our radio to change between the modes. By default the arduino is set to mode 0. 
When i turn on my radio it switches to mode 2, for normal driving of the boat. In case of a signal loss the ardu changes back to mode 0, 
and steers itself back to the place we set it of .

feel free to use and modify the code.
For questions please write me a message.




