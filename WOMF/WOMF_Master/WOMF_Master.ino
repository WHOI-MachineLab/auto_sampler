#include <TimeLib.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

Servo gearbox;

//CHANGE ONLY THIS INFO
int hour_interval=0; //define the sample intervals (time between samples)
int minute_interval=5; //(must be at least five minutes)
const int hour_till_sample=0; //define when you want sampling to start (change hours and/or minutes)
const int minute_till_sample=2;
//

const int servo_pin = 23;
const int pump_pin=13;
float sample_coordinate[10] = {5.3,20.96,36.7,52.4,68.1,83.8,99.6,115.3,131,146.7};
int sample_schedule[2][10];
int i=0;

//This is the SD card variable block
File myFile;
char filename[24];
String newFileName;
String dateStamp;
const int chipSelect = BUILTIN_SDCARD;
String header = "port start end";

//This is the EEPROM variable block
unsigned int address = 0;
int eeprom_value;
int term_address;
int n = 1;

void setup() {
  // put your setup code here, to run once:

 //Reading and updating EEPROM for naming file
  eeprom_read();
  eeprom_update();
  Serial.print("EEPROM ID is: ");
  Serial.println(eeprom_value);

// put your setup code here, to run once:
  gearbox.attach(servo_pin);
  //set time library to use internal RTC to keep time
  setSyncProvider(getTeensyTime);
 // Verify that RTC has been set
  Serial.begin(9600);

 if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  get_sample_schedule(hour_interval,minute_interval);

getDateName();
  createDateName();

//Initialize SD card, create a file, and write header line
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  } else {
    Serial.println("initialization done.");
  }
  newFileName.toCharArray(filename, 24);
  myFile = SD.open(filename, FILE_WRITE);
  myFile.println(header);
  Serial.print("Header line = ");Serial.println(header);
  myFile.close();
  Serial.print("File name is: ");
  Serial.println(newFileName);

  pinMode(pump_pin, HIGH);

  Serial.println(sample_schedule[1][i]);
  Serial.println(minute());
}



void loop() {
    while((hour() != sample_schedule[0][i])){
    
        Serial.println("waiting for sampling");
        delay(60000);
    }
  //while(minute() <= 4){} // avoid messing up Aaron's hydrophone? 
  if (minute()==sample_schedule[1][i]){
    Serial.println("It is time, turn the servo and sample");
    Serial.print("Sampling port ");
    Serial.println(i+1);
    myFile = SD.open(filename, FILE_WRITE);
    myFile.print(i+1);
    myFile.print(ts());myFile.print(",");
    turn_valve(sample_coordinate[i]);
    pump_sample(5,0); //pump_sample(minute, second);
    myFile.print(ts());myFile.println();
    i = i+1;
    if(i >= 10){
    Serial.println("Sampling is done, putting teensy to idle");
    while(1){}
    }
  delay(1000);
  }else{
    Serial.println("waiting for sampling");
    delay(5000);
  }
}


void pump_sample(int pump_min, int pump_sec){
  unsigned long millis_now = millis();
  int target = millis_now + pump_min*60*1000 + pump_sec*1000;
  digitalWrite(pump_pin, HIGH);
  while(millis() <= target){
    Serial.print("...");
    delay(5000);
    }
  digitalWrite(pump_pin, LOW);
  }

void turn_valve(float degree){
  gearbox.write(degree);;
  delay(15000);
  }

void get_sample_schedule(int hr_interval,int min_interval) { //constructs the sample schedule that the WOMF will run by
  const int start_time[2] = {hour()+hour_till_sample,minute()+minute_till_sample};
  if (start_time[1]>=60){
    sample_schedule[0][0] = start_time[0]+1;
    sample_schedule[1][0] = start_time[1]-60;
  }else{
    sample_schedule[0][0] = start_time[0];
    sample_schedule[1][0] = start_time[1];  
  }
  
  int hour_marker = sample_schedule[0][0];
  int minute_marker = sample_schedule[1][0];
  Serial.print(hour_marker);
  Serial.print(":");
  Serial.print(minute_marker);
  for(int counter=1; counter<=9; counter++) {
    if(hour_marker + hr_interval >=24) {
      hour_marker = hour_marker + hr_interval;
      sample_schedule[0][counter] = {hour_marker - 24};
    }else{
      hour_marker = hour_marker + hr_interval;
      sample_schedule[0][counter] = {hour_marker};
    }
    if(minute_marker + min_interval >=60) {
     
      minute_marker = minute_marker + minute_interval;
      minute_marker=minute_marker-60;
      sample_schedule[0][counter]={sample_schedule[0][counter]+1};
      sample_schedule[1][counter]={minute_marker};

      hour_marker = hour_marker + 1;
    }else{
      minute_marker = minute_marker + min_interval;
      //sample_schedule[0][counter]={hour_marker};
      sample_schedule[1][counter]= {minute_marker};
    }
    Serial.print(",");
    //Serial.print(now());
    Serial.print(sample_schedule[0][counter]);
    Serial.print(":");
    Serial.print(sample_schedule[1][counter]);
    }
  Serial.println();
  }
  
void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  Serial.print(minute());
  Serial.print(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void getDateName() { //this function grabs date stamp for file name.
  dateStamp = String(month()) + "-" + String(day());
}

void createDateName() { //this function creates new name for each file.
  newFileName = String("WOMF" + dateStamp + "-" + String(eeprom_value) + ".txt");
}

String ts() { //this function grabs local time for timestamp.
  String rn;
  rn = String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(month()) + "/" + String(day()) + "/" + String(year());
  return rn;
}

void eeprom_read() {
  Serial.println("reading eeprom");
  while (address < EEPROM.length() & EEPROM.read(address) != 0) {
    eeprom_value = eeprom_value + EEPROM.read(address);
    address = address + 1;
  }
  term_address = address - 1;
}

void eeprom_update() {
  int last_byte = EEPROM.read(term_address);
  if (last_byte > 255) {
    EEPROM.write(term_address, last_byte + 1);
  }
  else {
    EEPROM.write(term_address + 1, EEPROM.read(term_address + 1) + 1);
  }
  Serial.println("updating done");
}

time_t getTeensyTime(){
  return Teensy3Clock.get();
  }
