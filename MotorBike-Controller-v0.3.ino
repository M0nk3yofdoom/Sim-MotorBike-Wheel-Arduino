#include <AnalogSmooth.h>

#include <HX711.h>
#include <Joystick.h>
#define calibration_factor 241.26//334.879272//7700//4286578688 // Do your calibration first. default 2300 <-- I haven't calibrated it yet. am still confused about the messed up numbers but this maxes out at 1100 which is in line with our brake calibration in the program
//use scale.set_offset(4294957625); and scale.set_scale(0.033436);
//17740 is maximum output of brake.... look to set top at 17600 (0.058125)

//clutch 165 ->61

//throttle 179 -862
#define brake_ratio 0.058125
#define DOUT  4 //<-- swapped these pins... just making sure things work
#define CLK  5

HX711 scale;

/*


For the true false flags, use this list. Its all in  Joystick.h 
    bool includeXAxis = false,
    bool includeYAxis = false,
    bool includeZAxis = false, //clutch
    bool includeRxAxis = false,
    bool includeRyAxis = false,
    bool includeRzAxis = false, 
    bool includeRudder = false,
    bool includeThrottle = true, //twist trim pot
    bool includeAccelerator = false,
    bool includeBrake = true, //front brake load cell
    bool includeSteering = false);
  */
  /*
Buttons on the right = 1 press
Buttons on the left = 2 press, 3 latch

6 digital connections?
scale amp also digital?
clutch is analog?
throttle is analog?

How many threads do I need to ground? All 6 buttons have one pin ground and one pin on board.
  */

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 6, 3,
  false, false, true, false, false, false,
  false, true, true, true, false);


  
    
// Variable
int throttle = A0;
// int brake = 2; / no need for this as we read directly from scale
int clutch = A1; //how to invert clutch?
long brake = 0;
int throttleValue = 0;
int maxClutch = 200; //needed to invert clutch
int clutchValue = 0; //inverting clutch value in code because whoops on the wiring.




// Define pin connections for the switches <---I put this here.
int switchPin1 = 3;  // First switch connected to D2
int switchPin2 = 6;  // Second switch connected to D3
int switchPin3 = 7;  // Third switch connected to D4
int switchPin4 = 8;  // First switch connected to D5
int switchPin5 = 9;  // Second switch connected to D6
int switchPin6 = 10;  // Third switch connected to D7

// init joystick libary
void setup() {


  // Set up the button pins, with internal pull-up resistors enabled <---I added this
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
  pinMode(switchPin3, INPUT_PULLUP);
  pinMode(switchPin4, INPUT_PULLUP);
  pinMode(switchPin5, INPUT_PULLUP);
  pinMode(switchPin6, INPUT_PULLUP);

  // Ranges are 1023 by default
   Joystick.setBrakeRange(0, 1023); 
  Joystick.setThrottleRange(0, 1023); 
  Joystick.setZAxisRange(0, 1023); //clutch pedal
  
  Joystick.begin();
  Serial.begin(115200);
  
  scale.begin(DOUT, CLK);
  //scale.set_scale();
  scale.set_scale(calibration_factor); //deleted?
  //scale.set_offset(4294957625); //testing this.... I added it. 
  scale.tare(); 

  
}

void loop() {
    Serial.println (brake); //<-- still has issues with quality of data.
    Serial.println (throttleValue);
//    Serial.println (clutchValue);
//    Serial.println (maxClutch - clutchValue); //actual value sent to clutch
// Read the state of the switches <-- me
  bool switchState1 = digitalRead(switchPin1) == LOW;  // Assuming active LOW
  bool switchState2 = digitalRead(switchPin2) == LOW;  // Assuming active LOW
  bool switchState3 = digitalRead(switchPin3) == LOW;  // Assuming active LOW
  bool switchState4 = digitalRead(switchPin4) == LOW;  // Assuming active LOW
  bool switchState5 = digitalRead(switchPin5) == LOW;  // Assuming active LOW
  bool switchState6 = digitalRead(switchPin6) == LOW;  // Assuming active LOW

// Print switch states to the serial monitor
/*  Serial.print("Switch 1: ");
  Serial.println(switchState1 ? "ON" : "OFF");
  Serial.print("Switch 2: ");
  Serial.println(switchState2 ? "ON" : "OFF");
  Serial.print("Switch 3: ");
  Serial.println(switchState3 ? "ON" : "OFF");
  Serial.print("Switch 4: ");
  Serial.println(switchState4 ? "ON" : "OFF");
  Serial.print("Switch 5: ");
  Serial.println(switchState5 ? "ON" : "OFF");
  Serial.print("Switch 6: ");
  Serial.println(switchState6 ? "ON" : "OFF");
*/



  throttleValue = analogRead(throttle);

  Joystick.setThrottle(throttleValue);
  //Joystick.setThrottle(analogRead(throttle));
  delay(1);

//  brake = scale.get_units(); // if the value is inverted put a - sign in front like -scale.get
  
  
// If the value starts below 0 set it to 0. Or if its above 50 set it to 0
// This fixes the slight drift and sets it to 0 if it starts below 0
/*  if (brake < 0 or brake < 8) {
    brake = 0;
  }
    else
    brake = scale.get_units() ;
*/ 
  brake = scale.get_units(); 
  
  Joystick.setBrake(brake*brake_ratio); //abs might negate negatives?


  delay(1);



   clutchValue = analogRead(clutch);
  
//check that this is correct and helpful.
/*     if (clutchValue < 185) {
    clutchValue = 180;
  }

    else
    clutchValue = analogRead(clutch) ;
  */  
  Joystick.setZAxis(maxClutch - clutchValue);
 
 // Joystick.setZAxis(analogRead(clutch));
 
//button stack
Joystick.setButton(0, (switchState1 ? 1 : 0));
Joystick.setButton(1, (switchState2 ? 1 : 0));
Joystick.setButton(2, (switchState3 ? 1 : 0));
Joystick.setButton(3, (switchState4 ? 1 : 0));
Joystick.setButton(4, (switchState5 ? 1 : 0));
Joystick.setButton(5, (switchState6 ? 1 : 0));


  delay(1);
}
