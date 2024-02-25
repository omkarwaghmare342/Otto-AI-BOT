#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>
#define USE_SERIAL Serial
#include <Arduino.h>
#include <HTTPClient.h>



String json_String;

//const char* chatgpt_token = "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";

//JsonDocument doc;
CloudSpeechClient::CloudSpeechClient(Authentication authentication) 
{
  this->authentication = authentication;
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) delay(1000);
  client.setCACert(root_ca);
  if (!client.connect(server, 443)) Serial.println("Connection failed!");
}

String ans;

CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
  WiFi.disconnect();
}

void CloudSpeechClient::PrintHttpBody2(Audio* audio)
{
  String enc = base64::encode(audio->paddedHeader, sizeof(audio->paddedHeader));
  enc.replace("\n", "");  // delete last "\n"
  client.print(enc);      // HttpBody2
  char** wavData = audio->wavData;
  for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
    enc = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
    enc.replace("\n", "");// delete last "\n"
    client.print(enc);    // HttpBody2
  }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-IN\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n";
  int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3; // 4/3 is from base64 encoding
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
  String HttpHeader;
   if (authentication == USE_APIKEY)
  HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
//    else if (authentication == USE_ACCESSTOKEN)
//      HttpHeader = String("POST /v1beta1/speech:syncrecognize HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: Bearer ")
//     + AccessToken + String("\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  client.print(HttpHeader);
  client.print(HttpBody1);
  PrintHttpBody2(audio);
  client.print(HttpBody3);
  String My_Answer="";
  while (!client.available());
   
  while (client.available())
  {
    char temp = client.read();
    My_Answer = My_Answer + temp;
   // Serial.write(client.read());
  }


  int postion = My_Answer.indexOf('{');
//  Serial.println(postion);
  ans = My_Answer.substring(postion);
  Serial.print("Question-- ");
//  Serial.print(ans);
  
JsonDocument doc;
DeserializationError error = deserializeJson(doc, ans);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

JsonObject results_0 = doc["results"][0];

//const char* results_0_alternatives_0_transcript = results_0["alternatives"][0]["transcript"]; // "hello ...
const char* chatgpt_Q = results_0["alternatives"][0]["transcript"];
Serial.print(chatgpt_Q);
Serial2.println(chatgpt_Q);  
}
