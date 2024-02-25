#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h" // Download this Library -> https://github.com/schreibfaul1/ESP32-audioI2S
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ThingSpeak.h>



const char* chatgpt_token = "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";
String ans="";
String Start = "Omi is ready to assist you ,Please Ask!";
const char *apiKey = "5IT8F9SJJDLITE7I";
const unsigned long channelNumber = 2435504;
WiFiClient client;
Servo myservoL;
Servo myservoR;
Servo LeftFoot;
Servo RightFoot;

int pos=0;
int flag=0;
int flag1=0;
String res;
#define uart_en 15
#define RXp2 16
#define TXp2 17
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26
//const unsigned long timer1=5000;
unsigned long previous=0;
unsigned long current_time;

// Your WiFi Credentials
const char *ssid = "omkar";       // WiFi SSID Name
const char *password = "omkar000";// WiFi Password ( Keep it blank if your WiFi router is open )

Audio audio;


void setup()
{
  
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
  myservoL.attach(19, 1000, 2000);
  myservoR.attach(21, 1000, 2000);
  LeftFoot.attach(18, 1000, 2000);
  RightFoot.attach(5, 1000, 2000);

  myservoL.write(20);
  myservoR.write(180);
  LeftFoot.write(90);
  RightFoot.write(96);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin( ssid, password);
ThingSpeak.begin(client);
  while (WiFi.status() != WL_CONNECTED)
  {
  delay(500);
        Serial.print(".");
  }
  
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15);
  audio.connecttospeech("Initializing", "en"); // Google TTS
   
    
}

void loop()
{
  bool ledStatus;
  String Answer;
  if (Serial2.available()) {
    Answer = Serial2.readString();
    Answer.trim();
    Serial.println(Answer);
    if(Answer == "a")
    {
      flag=1;
      flag1=0;
     
    }
    else if(Answer == "b")
    {
      audio.connecttospeech(Start.c_str(), "en");
//      Serial.println("Speaker");
      delay(50);
      flag1=1;      
    }
    else if(Answer == "light on")
    {
      ledStatus=1;
      ThingSpeak.writeField(channelNumber, 2, ledStatus, apiKey);
      audio.connecttospeech("Ok, Turning light On!", "en");
      flag=0;
      myservoR.write(180);    
      myservoL.write(20);
      LeftFoot.write(90);
      RightFoot.write(96);
    }
    else if(Answer == "light off")
    {
      
      ledStatus=0;
      ThingSpeak.writeField(channelNumber, 2, ledStatus, apiKey);
      audio.connecttospeech("Ok, Turning light Off!", "en");
      flag=0;
      myservoR.write(180);    
      myservoL.write(20);
      LeftFoot.write(90);
      RightFoot.write(96);
    }
    
    else if (Answer !="b" && Answer !="a" && Answer != "light on" && Answer != "light off")
    {


      HTTPClient https;

    if (https.begin("https://api.openai.com/v1/chat/completions")) {  // HTTPS
      https.addHeader("Content-Type", "application/json"); 
      String token_key = String("Bearer ") + chatgpt_token;
      https.addHeader("Authorization", token_key);
      Serial.print("Q - ");
      Serial.print(Answer);
      String payload = String("{\"model\": \"gpt-3.5-turbo-0125\",\"messages\": [{\"role\": \"user\",\"content\": ") + "\""+ Answer + "\""+ String("}]}");
      
      // start connection and send HTTP header
      int httpCode = https.POST(payload);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();

JsonDocument doc1;
        DeserializationError error = deserializeJson(doc1, payload);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

JsonObject choices_0 = doc1["choices"][0];
int choices_0_index = choices_0["index"]; // 0

const char* choices_0_message_content = choices_0["message"]["content"]; // "I am a virtual assistant ...
  ans = choices_0_message_content;
Serial.println("");
Serial.print("Answer:");
Serial.print(ans);
audio.connecttospeech(ans.c_str(), "en");
flag=0;

    myservoR.write(180);    // tell servo to go to position in variable 'pos'
    myservoL.write(20);
    LeftFoot.write(90);
    RightFoot.write(96);
      }
      else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        audio.connecttospeech("Sorry, i couldn't know, Please ask again!", "en");
        flag=0;

    myservoR.write(180);    // tell servo to go to position in variable 'pos'
    myservoL.write(20);
    LeftFoot.write(90);
    RightFoot.write(96);
      }
      https.end();
    }
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  delay(50);
    ans="";
    Answer="";
    }
    else
    {
      Answer="";
    }
  }
  if(flag1 == 1)
  {
    
    myservoR.write(10);    // tell servo to go to position in variable 'pos'
    delay(10);             // waits 15ms for the servo to reach the position
 
  flag1=0;
  }
  if(flag == 1)
  {

for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoR.write(pos);
    myservoL.write(pos);// tell servo to go to position in variable 'pos'
    LeftFoot.write(pos);
  RightFoot.write(200-pos);
    delay(15);             // waits 15ms for the servo to reach the position
  }
  
  for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservoR.write(pos);    // tell servo to go to position in variable 'pos'
    myservoL.write(pos);
    LeftFoot.write(pos+60);
  RightFoot.write(pos-40);
    delay(15);             // waits 15ms for the servo to reach the position
  }
    
  }
  
  audio.loop();

}

void audio_info(const char *info) {
//  Serial.print("audio_info: "); Serial.println(info);
}
