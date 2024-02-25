#include "Audio.h"
#include "CloudSpeechClient.h"
#include <WiFi.h>
#define touch 23
#define Led 21
#define RXp2 16
#define TXp2 17

void setup() {
   
  
  pinMode(touch,INPUT_PULLUP);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
  pinMode(Led,OUTPUT);
  delay(100);
}

void loop() {
if(digitalRead(touch)==1)
{
//  Serial2.println("\r\nOmi is ready to assist you! How can I make your day better,Please Ask!\r\n");
//  delay(3500);
  Serial2.print("\r\nb\r\n");
  delay(5000);
  digitalWrite(Led,HIGH);
//  Serial2.print("\r\nc\r\n");
  Serial.println("\r\nRecord start!\r\n");
  Audio* audio = new Audio(ICS43434);
  //Audio* audio = new Audio(M5STACKFIRE);
  audio->Record();
  digitalWrite(Led,LOW);
  Serial.println("Recording Completed. Now Processing...");
  Serial2.print("\r\na\r\n");
  CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY);
  cloudSpeechClient->Transcribe(audio);
  delete cloudSpeechClient;
  delete audio;
}
  if (digitalRead(touch) == 0)
  {
    delay(1);   
  }
}
