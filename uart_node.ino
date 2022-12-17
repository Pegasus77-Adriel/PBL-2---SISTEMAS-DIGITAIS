/*
Obs: 
  1) LED embutido acende em nivel logico baixo - PIN 2
  2) Não apagar nenhuma função do OTA para não desconfigurar o ESP, não mexer por curiosidade 
  3) B0 e B1 é GPIO 16 e GPIO 5 respectivamente
      
*/

//Bibliotecas
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>


//Definicoes
#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#define TRUE 1
#endif

//Constantes e variaveis
const char* ssid = STASSID;
const char* password = STAPSK;
const int b0 = 16;
const int b1 = 5;
const int rx = 3;
const int tx = 1;
const int LED = LED_BUILTIN;

byte bread[2];
byte bwrite[2] = {0x00,0x00};
float v =0.0;
byte b = 0x00;
int i = 0;

//SoftwareSerial mySerial = SoftwareSerial(rx,tx);

IPAddress local_IP(10, 0, 0, 109);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);


void set_input(){
  pinMode(rx,INPUT);
  pinMode(A0,INPUT);
  }

void set_output(){
  pinMode(LED,OUTPUT);
  pinMode(tx,OUTPUT);  
}

void setup() {
  set_output();
  set_input();
  
  //mySerial.begin(9600);
  Serial.begin(9600);

  
  //Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (!WiFi.config(local_IP, gateway, subnet)) {
    //Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("ESP-10.0.0.109");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    //Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      //Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
     // Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      //Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      //Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      //Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

// Procedimentos e Funções

int analogic_read(int pin){
  int val = analogRead(pin);
  return val;
 
    }

//Cada caracter tem oito bit


void loop(){
  ArduinoOTA.handle();
  
  byte comando =  0x00;
  byte endereco = 0x00;  
  
  while(Serial.available() > 0){
    comando = Serial.read();   //byte de comando
    delay(500);
    endereco = Serial.read();
  
    switch(comando){
      //Node com problema
      case 0x03:
        b = 0x00;
        Serial.write(b);
        break;  
        
      //Analogico    
      case 0x04:{
        int pin = endereco;
        int val = analogic_read(endereco);
        comando = 0x01;
        Serial.write(comando);
        Serial.print(val);   
      }   
        break;
      case 0x05:{
        int pin = endereco;
        pinMode(pin,INPUT);
        int val = digitalRead(pin);
        comando = 0x02;
        Serial.write(comando);
        Serial.write(val);
      }
        break;
      case 0x06:
        //led_blink();
        digitalWrite(LED,LOW);
        comando = 0x50;
        Serial.write(comando);
        break;
      case 0x07:
        //led_blink();
        digitalWrite(LED,HIGH);
        comando = 0x51;
        Serial.write(comando);
        break;
      default:
        digitalWrite(LED,HIGH);
        delay(5000);
        digitalWrite(LED,LOW);
        break;
    }                         
         
  }
      
}

