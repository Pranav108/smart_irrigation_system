// #include <TimerOne.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>
#include <util/crc16.h>

#include "DHT.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

DHT dht(DHTPIN, DHTTYPE);

int moistureSensor = A0; // Moisture sensor pin connected to A0
int moistureValue = 0; // Variable to store moisture sensor reading


long unsigned int lowIn;
long unsigned int pause = 500;
boolean lockLow = true;
boolean takeLowTime;

float h;
float t;
int M;
float sensorValue;  //variable to store sensor value

uint16_t calcCRC(String str)
{
  uint16_t crc=0; // starting value as you like, must be the same before each calculation
  for (int i=0;i<str.length();i++) // for each character in the string
  {
    crc= _crc16_update (crc, str[i]); // update the crc value
  }
  return crc;
}


void temperatureData()
{
  delay(20);

  
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  

  //Serial.println(F("Humidity: "));
  //Serial.println(h);
  //Serial.println(F("  Temperature: "));
  //Serial.println(t); 
}

char str[]="A_T43H20M50W0";
// " A_T20H20M0W0 * 230 ^

int valve = 0;
int moistureThreshold = 700;
void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  
  dht.begin();
  pinMode(A0, INPUT); 
  

}

void loop() {
  temperatureData();
  moistureValue = analogRead(moistureSensor);
  String data = "";
  if(moistureValue >= moistureThreshold)
   {
    valve = 1;
   }
   else
   {
    valve = 0;
   }
  data += String("A") + String("_") + String("T") +  String(int(t)) + "H" +  String(int(h)) + "M" +  String(int(moistureValue)) + "W" +  String(int(valve));

  //Calculating checksum
  uint16_t chksum;
  chksum = calcCRC(data);
  data += "*" + String(chksum) + "^";
  Serial.println(data);
  Serial.println(chksum);

  //Sending Data To Receiver
  const char *sample = data.c_str();
  radio.write(sample, data.length());
  delay(1000);
}