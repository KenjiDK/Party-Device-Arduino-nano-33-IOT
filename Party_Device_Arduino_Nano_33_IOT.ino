/* Party  Device 
 *  MQ-3 Gas Sensor: VCC -> 5V
 *                   A0 -> A0
 *                   D0 -> D15
 *                   GND -> GND
 *        !please note that the MQ-3 has a preheat time of 24H, so it may be inaccurate!
 *  
 *  Sparfun Sound Detector: VCC -> 3.3V
 *                          Enveloppe -> D16
 *                          I didn't connect the Gate pin and Audio pin for this project.
 *                          GND -> GND
 *                          
 *  RGB Alcohol (common kathode): GND -> GND
 *                                Red -> D2
 *                                Green -> D3
 *                                Blue -> D4
 *                                
 *  RGB Sound (common kathode):   GND -> GND
 *                                Red -> D5
 *                                Green -> D6
 *                                Blue -> D7
 *                                
 *  Small little device for party's to be safe of sound and alcohol... or to break records :D
 *  Feel free to use and modify as you want and correct things that are not accurate 
 *  and please let me know so I can edit the main program
 */


#define BLYNK_MAX_SENDBYTES 256             //sets sendible bytes to 256 (max) for push notifications
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <WiFiNINA.h>                       //Arduino Uno 33 IOT build-in WiFi
#include <BlynkSimpleWiFiNINA.h>

const char* auth = "mH82hZg_1MyMdtfWIcPmBje9isZvv_w4";                //Authentication token from Blynk

char* ssid = "Defender";                       //Your WiFi name
char* pass = "";                               //Your WiFi password

int analog = A0;                              //Analog pin (A0) from MQ-3 sets to pin A0
int envoloppe = A2;                           //Analog pin (enveloppe) from the sound detector sets to pin A2

int waarde = 0;                               //Variable for value of MQ-3
float alcohol = 0;                            //Variable to convert 'waarde' to promille
int geluid = 0;                               //Variable for value of sound detector
float dB = 0;                                 //Variable to convert 'geluid' to decibel

//RGB Alcohol
int alcohol_rood = 2;                         //Define RGB red pin 
int alcohol_groen = 3;                        //Define RGB green pin 
int alcohol_blauw = 4;                        //Define RGB blue pin 

//RGB Sound
int geluid_rood = 6;                          //Define RGB red pin 
int geluid_groen = 7;                         //Define RGB green pin 
int geluid_blauw = 8;                         //Define RGB blue pin 

void setup() {
  Serial.begin(115200);                       //Begins Serial Monitor

  //Sets pinmodes to INPUT or OUTPUT
  pinMode(analog,INPUT);
  pinMode(envoloppe,INPUT);
  pinMode(alcohol_rood,OUTPUT);
  pinMode(alcohol_groen,OUTPUT);
  pinMode(alcohol_blauw,OUTPUT);
  pinMode(geluid_rood,OUTPUT);
  pinMode(geluid_groen,OUTPUT);
  pinMode(geluid_blauw,OUTPUT);

  //Setting up WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  //Begins Blynk server
  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);                    //I use the Blynk server from my professor but if you want you can use the servers of Blynk
  //Blynk.begin(auth, ssid, pass);                                          //Using one of the Blynk servers
}

void loop() {
  Blynk.run();                                          //Starts the Blynk cycle

  //MQ-3 gas sensor
  waarde = analogRead(analog);
  Serial.print(waarde);
  Serial.print("\t");
  alcohol = (waarde)/1000;
  Serial.print(alcohol);

  //Sound sensor
  geluid = analogRead(envoloppe);
  Serial.print("\t");
  Serial.print(geluid^2);
  Serial.print("\t");
  dB = 20 * log10(geluid) + 20;
  Serial.println(dB);

  //Writing the values to the Blynk app
  Blynk.virtualWrite(V1, dB);
  Blynk.virtualWrite(V2, alcohol);

  //3 if functions to use our RGB Sound
  if (dB <= 69) {
    analogWrite(geluid_groen,255);
    analogWrite(geluid_rood,0);
    analogWrite(geluid_blauw,0);
  }
  if (dB >= 70 and dB < 95) {
    analogWrite(geluid_groen,255);
    analogWrite(geluid_rood,255);
    analogWrite(geluid_blauw,0);
    Blynk.notify("Korte termijn gehoorschade!");
  }
  if (dB >= 95) {
    analogWrite(geluid_groen,0);
    analogWrite(geluid_rood,255);
    analogWrite(geluid_blauw,0);
    Blynk.notify("Lange termijn gehoorschade!");
  }

  //3 if functions to use our RGB Alcohol
  if (alcohol < 200) {
    analogWrite(alcohol_groen,255);
    analogWrite(alcohol_rood,0);
    analogWrite(alcohol_blauw,0);
  }
  if (alcohol >= 200 and alcohol < 500) {
    analogWrite(alcohol_groen,255);
    analogWrite(alcohol_rood,255);
    analogWrite(alcohol_blauw,0);
  }
  if (alcohol >= 500) {
    analogWrite(alcohol_groen,0);
    analogWrite(alcohol_rood,255);
    analogWrite(alcohol_blauw,0);
    Blynk.notify("Je mag niet met de auto rijden!");
  }
  delay(1000);                                            //delay of 1s
}
