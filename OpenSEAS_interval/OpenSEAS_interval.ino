// Original version of code developed August 2022, Ryan C. Galusha, WHOI MACHINE Lab
// This code triggers the first sampling event at a user specified time defined in the structure: 'FirstSample' 
// All subsequent sampling events occur at a regular time interval specified by the constant int: 'SampleInterval' 

#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Servo.h>

const int n_samples = 10; // Define number of samples you plan to take (maximum: 10)

struct Schedule {
  int Day
  int Hour;
  int Minute;
};

Schedule FirstSample = {// DAY, HOUR , MIN}; // Define the day of month (1-31) and time of day for the first sample to be triggered
const int SampleInterval = 3600; // Define the regular interval between all remaining samples here (units: seconds)

// Define approximate pump flow rate and sample volumes here for determining pump ON time
const int flowrate = 250 / 60; // ml per sec
const int sample_volume = 100; // ml

//Servo constants & variables
Servo gearbox; // name the servo "gearbox"
const int zero = 5; // calibrated "zero" position for first valve position
const int displacement =  16; // calibrated displacement of servo to reach the next valve position

// Define which pins are attached to components here
const int pumpPin = 6;
const int servoPin = 23;

// Misc variables
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

  // Verify that a valid sample number has been entered
  if (n_samples > 10) {
    Serial.println("Program exceeds the maximum number of allowed samples!");
    Serial.println("Please reprogram 'n_samples' to a value of 10 or less");
    while(1) {
    // LED flash here
    }
  }

  // Initialize component pins
  pinMode(pumpPin, OUTPUT);
  gearbox.attach(servoPin);
  
  // Write the servo position to calibrated "zero" (define above)
  Alarm.delay(5000);
  gearbox.write(zero);
  Alarm.delay(10000);
}

void loop()
{
  if (day() == FirstSample.Day && hour() == FirstSample.Hour && min() == FirstSample.Minute && second() < 3)
  {
    SampleNow(); // Collect the first sample
    Alarm.timerRepeat(SampleInterval, SampleNow); // Timer that triggers all subsequent samples after the first sample
  }
  Alarm.delay(1000);
}

time_t SampleNow() // Function for every sampling event, returns the timestamp of the sample
{
  if (samples_taken < n_samples) {
    Serial.println("Begin sampling routine");

    if (samples_taken == 0) {
      int nextpos = gearbox.read();
      Serial.println(nextpos);
    gearbox.write(nextpos);
    }
    else
    {
      // write the next servo position
      int nextpos = gearbox.read() + displacement;
      Serial.println(nextpos);
    gearbox.write(nextpos);
    }

    // wait for servo to move to next position
    Alarm.delay(2000);

    // turn on pump and wait to deliver volume
    digitalWrite(pumpPin, HIGH);
    Alarm.delay(30000);

    // turn off pump
    digitalWrite(pumpPin, LOW);

    Serial.print("Last sample finished at: "); Serial.println(now());
    samples_taken++;
    Serial.print("Samples taken so far: "); Serial.println(samples_taken);

    return now();
  }
}

time_t getTeensyTime()
{
  return Teensy3Clock.get();
}
