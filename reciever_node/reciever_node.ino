#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>
#include <util/crc16.h>
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

volatile int temperature = 0;
volatile int humidity = 0;
volatile int gas_level = 0;
volatile int motion_detect = 0;
bool valA=0;
bool valB=0;

uint16_t calcCRC(String str)
{
  uint16_t crc=0; // starting value as you like, must be the same before each calculation
  for (int i=0;i<str.length();i++) // for each character in the string
  {
    crc= _crc16_update (crc, str[i]); // update the crc value
  }
  return crc;
}
void process_string(String str)
{
  String str_data = "";
  uint16_t final_chk;
  String chksum = "";
  //Serial.println("Size : ");
  //Serial.println(sizeof(str));
  int i=0;
  while(i<str.length() && str[i]!='*'){
    str_data+=str[i];
    ++i;
  }
  ++i;
  while(i<str.length() && str[i]!='^'){
    chksum+=str[i];
    ++i;
  }
//    else
//    {
//      
//      for(int j=i+1;j<str.length();j++)
//      {
//        if(str[j]!='^')
//        {
//          chksum += str[j];
//        }
//        else
//        {
//          break;
//        }
//      }
//      break;
//    }
  //Serial.println("Printing Data : ");
  //Serial.println(str_data);
  //Serial.println("Printing Checksum : ");
  //Serial.println(chksum);
  final_chk = calcCRC(str_data);
  // if(final_chk == chksum.toInt())
  // {
  int len=str_data.length();
  if(str_data[0]=='A'){
    if(str_data[len-1]=='1'){
      valA=1;
    }
    else{
      valA=0;
    }
  }
  else{
    if(str_data[len-1]=='1'){
      valB=1;
    }
    else{
      valB=0;
    }
  }
  // if(valA==1 || valB==1){
  //   Serial.println("Main motor is on");
  // }
  // else{
  //   Serial.println("main motor is off");
  // }
  Serial.println(str_data);
  
  
  // }
  
}
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    //Serial.println(sizeof(text));
    //Serial.println(text);
    String data_str = "";

    int i=0;
    while(1)
    {
      if(text[i]!='^')
      {
        data_str += text[i];
        i++;  
      }
      else
      {
        data_str += text[i];
        break;  
      }
      
    }
    //Serial.println(data_str);
    process_string(text);
  }
}