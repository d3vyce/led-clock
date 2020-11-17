#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "RTClib.h"
#include <Wire.h>
#include <Adafruit_NeoPixel.h>


RTC_DS3231 rtc;

DateTime now;

//Déclaration LED
#define LEDCLOCK_PIN 12 //d6
#define LEDDOWNLIGHT_PIN 14 //d5

#define LEDCLOCK_COUNT 252
#define LEDDOWNLIGHT_COUNT 14

Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRB + NEO_KHZ800);

int clockMinuteColour = 26316; //1677
int clockHourColour = 204; //7712

int clockFaceBrightness = 0;


//Smoothing of the readings from the light sensor so it is not too twitchy
const int numReadings = 12;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long total = 0;                  // the running total
long average = 0;                // the average


//Déclaration Wifi
const char *ssid     = "SSID";
const char *password = "SSID_Password";

char t[32];

const long utcOffsetInSeconds = 3600;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "3.fr.pool.ntp.org", utcOffsetInSeconds);

void setup(){
  
  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  stripClock.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.show();            // Turn OFF all pixels ASAP
  stripClock.setBrightness(200); // Set inital BRIGHTNESS (max = 255)
 

  stripDownlighter.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripDownlighter.show();            // Turn OFF all pixels ASAP
  stripDownlighter.setBrightness(50); // Set BRIGHTNESS (max = 255)

  //smoothing
    // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
  timeClient.begin();
  rtc.begin();

  timeClient.update();
  rtc.adjust(DateTime(2020,00,00,timeClient.getHours(),timeClient.getMinutes(),timeClient.getSeconds()));
}

void loop() {
   now = rtc.now();
  
  if(now.hour() == 5) {
    rtc.adjust(DateTime(2020,00,00,timeClient.getHours(),timeClient.getMinutes(),timeClient.getSeconds()));
  }

  //Read the time
  

  //Display time on the matrice
  displayTheTime();

    //Record a reading from the light sensor and add it to the array
  readings[readIndex] = analogRead(A0); //get an average light level from previouse set of samples
  Serial.print("Light sensor value added to array = ");
  Serial.println(readings[readIndex]);
  readIndex = readIndex + 1; // advance to the next position in the array:

  // if we're at the end of the array move the index back around...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  //now work out the sum of all the values in the array
  int sumBrightness = 0;
  for (int i=0; i < numReadings; i++)
    {
        sumBrightness += readings[i];
    }
  Serial.print("Sum of the brightness array = ");
  Serial.println(sumBrightness);

  // and calculate the average: 
  int lightSensorValue = sumBrightness / numReadings;
  Serial.print("Average light sensor value = ");
  Serial.println(lightSensorValue);


  //set the brightness based on ambiant light levels
  clockFaceBrightness = map(lightSensorValue,50, 1000, 200, 1); 
  //stripClock.setBrightness(clockFaceBrightness); // Set brightness value of the LEDs
  Serial.print("Mapped brightness value = ");
  Serial.println(clockFaceBrightness);
  
  stripClock.show();

   //(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
  stripDownlighter.fill(6750207, 0, LEDDOWNLIGHT_COUNT);
  stripDownlighter.show();

  delay(5000);   //this 5 second delay to slow things down during testing
}

//FONCTION

void displayTheTime(){

  stripClock.clear(); //clear the clock face
  
  int firstMinuteDigit = now.minute() % 10; //work out the value of the first digit and then display it
  displayNumber(firstMinuteDigit, 0, clockMinuteColour);
  
  int secondMinuteDigit = floor(now.minute() / 10); //work out the value for the second digit and then display it
  displayNumber(secondMinuteDigit, 63, clockMinuteColour);  


    int firstHourDigit = now.hour()%10;
  if (now.hour() == 24){
    firstHourDigit = 0;
  }
  displayNumber(firstHourDigit, 126, clockHourColour);

  int secondHourDigit = floor(now.hour() / 10); //work out the value for the third digit and then display it
  if (secondHourDigit != 0){
    displayNumber(secondHourDigit, 189, clockHourColour);
  }
}


void displayNumber(int digitToDisplay, int offsetBy, int colourToUse){
    switch (digitToDisplay){
    case 0:
    digitZero(offsetBy,colourToUse);
      break;
    case 1:
      digitOne(offsetBy,colourToUse);
      break;
    case 2:
    digitTwo(offsetBy,colourToUse);
      break;
    case 3:
    digitThree(offsetBy,colourToUse);
      break;
    case 4:
    digitFour(offsetBy,colourToUse);
      break;
    case 5:
    digitFive(offsetBy,colourToUse);
      break;
    case 6:
    digitSix(offsetBy,colourToUse);
      break;
    case 7:
    digitSeven(offsetBy,colourToUse);
      break;
    case 8:
    digitEight(offsetBy,colourToUse);
      break;
    case 9:
    digitNine(offsetBy,colourToUse);
      break;
    default:
     break;
  }
}
