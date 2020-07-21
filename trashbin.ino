#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <Servo.h>

// sensor 1 untuk mendeteksi orang
#define trigPin1  D3
#define echoPin1  D1
#define trigPin2  D5
#define echoPin2  D6

#define servoPin  D2
SoftwareSerial dfplayer(D7,D8);
Servo servo;
DFRobotDFPlayerMini myDFPlayer;
char auth[] = "YOUR_BLYNK_API_KEY";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

BlynkTimer timer;
long duration, distance; //waktu untuk kalkulasi jarak
bool lastState;
void myTimerEvent()
{
  char buff[100];
  unsigned int ultra1 = measure(trigPin1,echoPin1);
  unsigned int ultra2 = measure(trigPin2,echoPin2);
  sprintf(buff,"Ultra 1 : %d, Ultra 2: %d",ultra1,ultra2);
  Serial.println(buff);
  unsigned int detectPeople = ultra1;
  if(detectPeople<20)
  {
    Serial.println("People detected");
    servo.write(180);
    myDFPlayer.play(1);
    delay(5000);
  }
  else{
      servo.write(0);
      unsigned int level = ultra2;
      Blynk.virtualWrite(V0, level);
      if(level<=3&&lastState==false)
      {
        lastState=true;
        Serial.println("Trashbin is full");
        Blynk.notify("Trashbin is full");
      }
      else if(level<=10) lastState=true;
      else lastState=false; 
  }
}

int measure(byte trigPin,byte echoPin)
{
    digitalWrite(trigPin, LOW);delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration/58.2;
    return distance;
}

void setup()
{
  Serial.begin(9600);
  dfplayer.begin(9600);
  servo.attach(servoPin);
  if (!myDFPlayer.begin(dfplayer)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(30); 
  delay(1000);
  
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(servoPin,OUTPUT);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
