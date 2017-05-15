# GPSLoc
DISCONTINUED!
I do not work on this project anymore.
Feel free to fork the shit out of this

This is version 1 of the final GPSLoc code. 
GPSLoc is an Arduino program to calculate the back route of boat to steer it automatically back to the controlling person in case of a signal loss. 
I did my best to translate the commenting and variables to english.(It was written in german at first)
Shoutout to my buddy whith whom I made this project.

How It works:
the boats GPS module saves the initial GPS values when the boat is turned on. 
With a phone or via the Serial monitor you can see if the GPS has a stable connection. 
The Arduino has three control states. 
  0. FULLCONTROLLED - The Arudino is in full control over speed and steering of the boat.
  1. HALFCONTROLLED - The User can control Speed, and the Arduino is controlling the steering.
  2. NOTCONTROLLED - The user is in full controll of the boat.

For testing we used a switch on our radio to change between the modes. By default the arduino is set to mode 0. 
When i turn on my radio it siwtches to mode 2, for normal driving of the boat. In case of a signal loss the ardu changes back to mode 0, 
and steers itself back to the place we set it of .

feel free to use and modify the code.
for questions please write me a message or start an issue.




