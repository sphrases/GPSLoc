@@ -0,0 +1,383 @@

#include <Servo.h>                        // Servo lib
#include <TinyGPS.h>                      // GPS lib

// --------------------------------------------------------------------------------------------------------------
// Setting up the reviecer and servos
// --------------------------------------------------------------------------------------------------------------
                                          // defining pins
static const int rcPinTakeControl   =  5; // Switch control receiver input
static const int rcPinRudder        =  4; // Rudder receiver input
static const int rcPinSpeed         =  3; // Speed control receiver input
// servo pins
static const int servoRudderPin     =  45; // Rudder Arduino output
static const int servoSpeedPin      =  44; // Speed Arduino output

// --------------------------------------------------------------------------------------------------------------
// initialising GPS variables
// --------------------------------------------------------------------------------------------------------------
#define rxGPS 19                           
#define txGPS 18                           
TinyGPS gps;

// --------------------------------------------------------------------------------------------------------------
// Global varaibles
// --------------------------------------------------------------------------------------------------------------
                                           // GPS
unsigned long  age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
byte           month, day, hour, minute, second, hundredths;
int            year;
float          latitude, longitude;
float          latitude_initial, longitude_initial;
float          speed, course;



                                           // pin 13 LED for status info
int led = 13;

// servos
int servoValueRudder  = 0;
int servoValueSpeed   = 0;
int servoValueControl = 0;

int value = 0;
int newServoValue = 0;

int controlState = 0;                       // the ControlState
static int NOTCONTROLLED    = 0;            // the arduino is in control
static int HALFCONTROLLED   = 1;            // the user has only control of speed
static int FULLCONTROLLED   = 2;            // the user is in full ocntrol

                                            // a maximum of 8 servo objects can be made
Servo servoRudder;                          // rudder servo
Servo servoSpeed;                           // speed controller

                                            // calibrating PWM values, in our case MultiplexRC (RoyalPRO, RoyalSX)
static const int servoMin      =  1040  ;   // min PWM value
static const int servoMid      =  1590 ;    // avr PWM value
static const int servoMax      =  2130;     // max PWM value
                                                     
int i = 0;                                  // variable to store the servo position 
unsigned long timer = 1000;                 // for delay in ms 
                                            							
bool newData = false;          
// --------------------------------------------------------------------------------------------------------------
// Calculation variables
// --------------------------------------------------------------------------------------------------------------

int locked = 1;
int lamp = 11;


double coursetemp;
double coursetemp1;
double b1;
double B2;
double L1;
double L2;

double V;
double H;
double calculationstep_0, calculationstep_1, calculationstep_2, calculationstep_3, calculationstep_4;





//---------------------------------------------------------------------------------------------------------------
// steering
//---------------------------------------------------------------------------------------------------------------

double goalcourse_border_down;
double goalcourse_border_up;
boolean signal;

double dist_latitude, dist_longitude; 
double dist;
double sinusalpha;
double alpha;


// --------------------------------------------------------------------------------------------------------------
// Setup section
// --------------------------------------------------------------------------------------------------------------

void setup() {
  

  
  
  unsigned long fix_age;
  
                                                // the normal serial port is used for Serial.print functions to the pc and bluetooth module
                                                // the Arduino Mega outputs Serial to RX0/TX0 (Pin X/Y).
                                                // we connected a bluetooth module to those pins
  Serial.begin(115200);                         // console & bluetooth output

                                                // serial1 is for the GPS module (model EM406A) 
  Serial1.begin(4800);                          // GPS input with 4800 Baud
                                                
                                                // defining some out- and inputs
  pinMode(led, OUTPUT);
  pinMode(rcPinTakeControl, INPUT);
  pinMode(rcPinRudder, INPUT);
  pinMode(rcPinSpeed, INPUT);
  pinMode(lamp, OUTPUT);

                                                // Servo initialization
  servoRudder.attach(servoRudderPin);           // attaches the servo on pin 5 to the servo object 
  servoSpeed.attach(servoSpeedPin);             // attaches the servo on pin 3 to the servo object 
  
                                                // getting initial GPS coords
                                                // For one second (variable "timer") we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < timer;)
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      if (gps.encode(c))                        // Did a new valid sentence come in?
      newData = true;
    }
   }   
  
  if (newData == true)                          // Did a new valid sentence come in?
  {
                                                // Establishing a fix 
   gps.f_get_position(&latitude_initial, &longitude_initial, &fix_age);
                          
   if (fix_age == TinyGPS::GPS_INVALID_AGE)    // if " == TinyGPS::GPS_INVALID_AGE" is returned, GPS might not have a fix
           Serial.println("Cannot lock initial position");
                                                // when "fix_age" is higher then 1500 a possible loss of data could happen
                                                
      else if (fix_age > 1500)
           Serial.println("Warning: possible loss of data!");
                                                 
      else
           Serial.println("Initial position set");  
         signal = 1;
                                                // getting speed
   speed = gps.f_speed_kmph();                  // speed in km/h
   course = gps.f_course();                     // course in degree
         
                                                // initial values are put out         
   Serial.println("----------------------------------------------------------------------------------------------");
   Serial.print(latitude_initial, 6); 
   Serial.print(";");
   Serial.print(longitude_initial, 6);
   Serial.print(";");
   Serial.print(speed, 6);
   Serial.print(";");
   Serial.println(course, 6);                                             
   newData = false;                            // "newData" to false, so the program works later
  }                                            // end of "if (newData == true)"   
}                                              // end of setup


// --------------------------------------------------------------------------------------------------------------
// Main programm
// --------------------------------------------------------------------------------------------------------------

void loop() {
// --------------------------------------------------------------------------------------------------------------

     gpsRead();                                // reading GPS data
  
// --------------------------------------------------------------------------------------------------------------
// calculating the course
// --------------------------------------------------------------------------------------------------------------
     
     
     L1 = longitude_initial;
     L2 = longitude;
     b1 = latitude_initial;
     B2 = latitude ;
     
     calculationstep_1 = (L2-L1) / (B2-b1);
     calculationstep_2 = cos(b1 * 0.01745329251) * (calculationstep_1);
     calculationstep_3 = atan(calculationstep_2);
     calculationstep_4 = 57.2957795 * calculationstep_3;
     
   if ((B2 - b1) > 0 &&  (L2 - L1) <= 0)
   {
     coursetemp = calculationstep_4 + 360;
   }
   
   else if((B2 - b1) < 0) 
   {
     coursetemp = calculationstep_4 + 180;
   }
   else coursetemp = calculationstep_4;
   
   if (coursetemp > 180)
   {
     coursetemp1 = coursetemp - 180;
   }
   
   else {coursetemp1 = coursetemp + 180; }
   

  // --------------------------------------------------------------------------------------------------------------
   
  gpsDataPrint();                                 // printing GPS data on serial
  readPinPosition();                              // getting ControlState
                                                  // how is the boat controlled?
  switch (controlState)
  {
   case 2:                                        // FULLCONTROLLED: the user is in control, all values are put through to the esc
        servoRudder.write(pulseIn(rcPinRudder, HIGH));
        servoSpeed.write(pulseIn(rcPinSpeed, HIGH));
        break;
   case 1:                                        // HALFCONTROLLED: user has control of speed, arduino is steering

        ruddercourse();
        servoSpeed.write(pulseIn(rcPinSpeed, HIGH));      
        break;
   default:                                       // NOTCONTROLLED: arduino is in control

        servoSpeed.write(1400);                   // half speed
        ruddercourse();
        Serial.println("Der Arduino hat das Ruder");
        break;  
  }                                               // end of switch 
}                                                 // end loop

// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------
// other methods:
// --------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------
// reading the position of the ControlState switch
// --------------------------------------------------------------------------------------------------------------
void readPinPosition()                              // ControlState (G switch on our radio)
{
  value = pulseIn(rcPinTakeControl, HIGH);          // the PWM value of the reciever is read
  servoValueControl = map(value, servoMin, servoMax, 0, 255); // the PWM values are mapped from 0 to 255
  if (servoValueControl < 30)                                    // 2 -- Switch case
  {
    controlState = NOTCONTROLLED;
    digitalWrite(led, LOW);

  }
  else if (servoValueControl >= 30 && servoValueControl <= 190 ) // 1
  {
    controlState = HALFCONTROLLED;
    digitalWrite(led, HIGH);
   
  }
  else                                                           // 0 
  { 
   controlState = FULLCONTROLLED;
    digitalWrite(led, HIGH);
     
  }
}                                                                // end of method


// --------------------------------------------------------------------------------------------------------------
// GPS read
// --------------------------------------------------------------------------------------------------------------
void gpsRead()
{
                                                                // a timer is set for 1 second to let the GPS read the initial values
  for (unsigned long start = millis(); millis() - start < (timer / 2);)
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      if (gps.encode(c))                                      
        newData = true;
    }
  }
  
  if (newData == true)
  {                                                           
    gps.f_get_position(&latitude, &longitude);                  // latitude / longitude                                                            
    speed = gps.f_speed_kmph();                                 // getting speed in km/h
  }
}                                                               // end of method

// --------------------------------------------------------------------------------------------------------------
// GPS encode
// --------------------------------------------------------------------------------------------------------------

void gpsData() 
 {
   gps.f_get_position(&latitude, &longitude);
   gps.stats(&chars, &sentences, &failed);
   gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age); 
 }                                                              // end of method

// --------------------------------------------------------------------------------------------------------------
// output of GPS data 
// --------------------------------------------------------------------------------------------------------------
void gpsDataPrint()
 {
       // values are put out via the serial port (in our case on a phone via a bluetooth serial dongle thingy)
       Serial.println("____________________");
       Serial.print(latitude_initial, 6); 
       Serial.print("; ");
       Serial.print(longitude_initial, 6);
       Serial.println(" ");
       Serial.print(latitude, 6); 
       Serial.print("; ");
       Serial.print(longitude, 6);
       Serial.println("");
       Serial.print("current GPS course  ");
       Serial.println(course, 6);
       Serial.print("course in home direction  ");
       Serial.println(coursetemp1, 6);
       Serial.print("ControlState ");
       Serial.println(controlState);
       Serial.print("rudder value   ");
       Serial.println(pulseIn(rcPinRudder, HIGH));

 }                                                             // end of method

// --------------------------------------------------------------------------------------------------------------
// calculation 2
// --------------------------------------------------------------------------------------------------------------
// distinguishing if the boat should turn left or right

void ruddercourse()
{
  double grenze = 25;
  goalcourse_border_down = coursetemp1 - grenze;
  goalcourse_border_up = coursetemp1 + grenze;
  if (course > goalcourse_border_down && course < goalcourse_border_up)
  {
  servoRudder.write(1590);
  Serial.println("straight ");
  }
  else if (coursetemp1 > (course / 2))
  {
    servoRudder.write(1300);
    Serial.println("right");  
  } else
  {
    servoRudder.write(1680);
    Serial.println("left");
  }
  
  
  
}


// --------------------------------------------------------------------------------------------------------------
// telling if the user has control
// --------------------------------------------------------------------------------------------------------------

void lenkPrint()
{
  Serial.println("user in control");
}
