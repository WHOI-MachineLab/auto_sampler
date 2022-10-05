// Original version of code developed August 2022, Ryan C. Galusha, WHOI MACHINE Lab
// This code triggers the first sampling event at a user specified time defined in the structure: 'FirstSample'
// All subsequent sampling events occur at a regular time interval specified by the constant int: 'SampleInterval'

#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Servo.h>

const int n_samples = 10; // Define number of samples you plan to take (maximum: 10)

struct Schedule {
  int Day;
  int Hour;
  int Minute;
};

Schedule FirstSample = {// DAY, HOUR , MIN}; // Define the day of month (1-31) and time of day for the first sample to be triggered
  const int SampleInterval = 3600; // Define the regular interval between all remaining samples here (units: seconds)

  //Servo constants & variables
  Servo gearbox; // name the servo "gearbox"
  const int convertDegrees = 450/180; // ratio for converting from Servo.h library to the actual degrees rotated. See README for more info.
  const int zero = 12.5 * convertDegrees; // calibrated "zero" position for the first valve position
  const int rotationDegrees =  40 * convertDegrees; // calibrated displacement of servo to reach the next valve position

  // Define which pins are attached to components here
  const int pumpPin = 6;
  const int servoPin = 23;

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
      while (1) {
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
    
    int samples_taken = 0; // number of samples taken so far
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
    const int pumpTime = 30000; // Define the length of time the pump will be turned to collect each sample (units: ms)

    if (samples_taken < n_samples) {
      Serial.println("Begin sampling routine");

      if (samples_taken != 0) {
        // Tell the servo to turn to the next position
        int nextpos = gearbox.read() + rotationDegrees;
        Serial.println(nextpos);
        gearbox.write(nextpos);
      }
      Alarm.delay(2000); // wait for servo to move to next position

      digitalWrite(pumpPin, HIGH); // turn on pump
      Alarm.delay(pumpTime); // wait to deliver volume (set this time above)
      digitalWrite(pumpPin, LOW); // turn off pump

      samples_taken++;

      Serial.print("Last sample finished at: "); Serial.println(now());
      Serial.print("Samples taken so far: "); Serial.println(samples_taken);

      return now();
    }
  }
  int valve_postition;
  int sample(int valve_new){
    //you will tell the servo which valve to go to.
    return valve_position;
    };
  }
  void zore(){//let's say you lost power and need to reboot};
  time_t getTeensyTime()
  {
    return Teensy3Clock.get();
  }
