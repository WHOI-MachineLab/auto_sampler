#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Servo.h>

// Define sampling schedule here:
const int n_samples = 10; // number of samples you plan to take

struct Time {
  int Hour;
  int Minutes;
  int Seconds;
  //  int dayofweek =
};

Time firstsample = {15 , 32, 0};
// Define sampling interval here:
//const int sample_interval_hrs = 2; // set the regular interval between samples here in hours
const int sample_interval_sec = 60; // convert sample interval to seconds

// Define approximate pump flow rate and sample volumes here for determining pump ON time
const int flowrate = 250 / 60; // ml per sec
const int sample_volume = 100; // ml

//Servo constants & variables
Servo gearbox; // name the servo "gearbox"
const int zero = 5; // calibrated "zero" position for first valve position
const int displacement =  16; // calibrated displacement of servo to reach the next valve position
const int servoPin = 23;

//Pump constants
const int pumpPin = 6;

// Misc variables
int samples_taken = 0; // number of samples taken so far

void setup() {
  // set the Time library to use Teensy internal RTC to keep time
  setSyncProvider(getTeensyTime); // calls on the getTeensyTime function below

  // Verify that RTC has been set
  Serial.begin(115200);
//  while (!Serial);  // Wait for Arduino Serial Monitor to open
//  delay(100);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

// Set Pump pin to output, ensure pump is off
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  
  // Attach Servo to pin, initialize position to calibrated zero
  gearbox.attach(servoPin);
  Alarm.delay(5000);
  gearbox.write(zero);
  Alarm.delay(10000);

  // Single alarm for triggering the first sample routine
  //Alarm.alarmOnce(dayofweek(optional, next occurance if blank), hours, minutes, seconds, function);
  Alarm.alarmOnce(firstsample.Hour, firstsample.Minutes, firstsample.Seconds, SampleTimer);

  // LED pin set to output and turn on LED
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

// Function for getting teensy RTC to sync with computer time
time_t getTeensyTime()
{
  return Teensy3Clock.get();
}

// Sample timer triggered by alarm in void setup()
void SampleTimer() {
  //start the timer for all remaining sample events
  Serial.println("Sample timer triggered successfully");
  Alarm.timerRepeat(sample_interval_sec, SampleNow);

  //trigger the first sample event
  SampleNow();
}

// Function for the routine sampling events, returns the timestamp of the sample
time_t SampleNow()
{
  if (samples_taken < n_samples - 1) {
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
    Alarm.delay(sample_volume / flowrate * 1000);

    // turn off pump
    digitalWrite(pumpPin, LOW);

    Serial.print("Last sample finished at: "); Serial.println(now());
    samples_taken++;
    Serial.print("Samples taken so far: "); Serial.println(samples_taken);

    return now();
  }
}

void loop() {
  digitalClockDisplay();
  Alarm.delay(1000);
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
