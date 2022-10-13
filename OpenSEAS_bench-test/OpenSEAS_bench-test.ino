// Developed from original version of OpenSeas_interval October 6, 2022, Ryan C. Galusha, WHOI MACHINE Lab
// Initializes the valve by returning to a "home position" before sampling at a single user specified valve port. This processing of homing and sampling is repeated at the same valve position to allow for a determination on the repeatability of the servo's movement.

#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Servo.h>

const int pumpTime = 30000; // Define the length of time the pump will be turned to collect each sample (units: ms)

//Servo constants & variables
Servo gearbox; // name the servo "gearbox"
const int convertDegrees = 450 / 180; // ratio for converting from Servo.h library to the actual degree position. See README for more info.
int servo_position = gearbox.read();

// Calibrate the degree positions for each valve here. These values are very rough estimates, so fine tuning degrees of rotation for the valve positions will be necessary.
const int SamplePort4 = 12 * convertDegrees;
const int SamplePort5 = 48 * convertDegrees;
const int SamplePort6 = 84 * convertDegrees;
const int SamplePort7 = 120 * convertDegrees;
const int SamplePort8 = 156 * convertDegrees;
const int SamplePort9 = 192 * convertDegrees;
const int SamplePort10 = 228 * convertDegrees;
const int SamplePort1 = 264 * convertDegrees;
const int SamplePort2 = 300 * convertDegrees;
const int SamplePort3 = 336 * convertDegrees;

const int PortToTest = SamplePort4; // Copy/paste the name of the valve port name you want to test

                       // Define which pins are attached to components here
                       const int pumpPin = 6;
const int servoPin = 23;

int samples_taken = 0; // number of samples taken so far

void setup()
{
  // LED pin set to output (for diagnostics)
  pinMode(13, OUTPUT);

  // set the Time library to use Teensy internal clock to keep time
  setSyncProvider(getTeensyTime); // see "void getTeensyTime()" function below

  // Verify that real time clock has been set
  Serial.begin(115200);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
    // LED flash here
  } else {
    Serial.println("RTC has set the system time");
    // Long LED on
  }

  // Initialize component pins
  pinMode(pumpPin, OUTPUT);
  gearbox.attach(servoPin);

  Alarm.delay(5000);
  home(); // return servo to the "home valve position" (position 4), three LED flashes indicate servo has returned to home position
}

void loop()
{
  while(Serial.available() == 0){delay(10)}
  SampleTest(PortToTest); // Run the sample test function, included as a loop
  home(); // return home, then repeat (for multiple trials to check for repeatability)
  while(Serial.available() != 0){
    Serial.read();
    delay(1);
    }
}

int SampleTest(int sample_port) // Sample routine for a user defined valve position
{
  Serial.print("Begin sample test");

  // int servo_position = gearbox.read(); // Read the starting valve position
  gearbox.write(sample_port); // Write the desired sample port position

  Alarm.delay(10000); // wait for servo to move to next position

  digitalWrite(pumpPin, HIGH); // turn on pump
  Alarm.delay(pumpTime); // wait to deliver volume (set this time above)
  digitalWrite(pumpPin, LOW); // turn off pump

  samples_taken++;
  Serial.print("Last sample finished at: "); Serial.println(now());

  // int servo_position = gearbox.read();
  return servo_position;
}

void home() // Function for returning the valve position back to the "home valve postion" (position 4)
{
  gearbox.write(0);
  Alarm.delay(10000);
  gearbox.write(SamplePort4);
  Alarm.delay(3000);
  Serial.println("The servo has returned to the home position");
  int servo_position = gearbox.read();

  // Flash LED
  digitalWrite(13, HIGH), Alarm.delay(100), digitalWrite(13, LOW), Alarm.delay(100), digitalWrite(13, HIGH), Alarm.delay(100), digitalWrite(13, LOW), Alarm.delay(100), digitalWrite(13, HIGH), Alarm.delay(1000), digitalWrite(13, LOW);
}

time_t getTeensyTime()
{
  return Teensy3Clock.get();
}
