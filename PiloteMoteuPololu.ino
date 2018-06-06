#include "DualVNH5019MotorShield.h"
#include <SoftwareSerial.h>

#define pinRx 11
#define pinTx 3

DualVNH5019MotorShield md;
SoftwareSerial mySerial(pinRx, pinTx); // RX, TX

// Ne pas utiliser A0 2 4 6 9      A1 7 8 10 12
//INPUT 0 1 2 3 4 5 6 7 8 9 10 11 12 13

//INPUT

const int _IN_LED_G = 0; //->D8
const int _IN_LED_D = 1; //->D6
const int _IN_MOT_SENS = 2; //->D2

const int BUILTIN_LED=13; 

//OUTPUT
const int _OUT_LED_G = 7;
const int _OUT_LED_D = 8;
const int _OUT_MOT_ACK = 5; //->D1

const int VITESSE=100; //TODO a calibrer
String readString = "";
float motDuree;

void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}
void setup()
{
  Serial.begin(115200);

  pinMode(_OUT_LED_G,OUTPUT);  //Allume LED G
  pinMode(_OUT_LED_D,OUTPUT);  //Allume LED D
  pinMode(_OUT_MOT_ACK,OUTPUT);  // Ack lancer moteur

  pinMode(_IN_MOT_SENS,INPUT);  //Init IN Sens moteur
  pinMode(_IN_LED_G,INPUT);  //Init IN LED Gauche
  pinMode(_IN_LED_D,INPUT);  //Init IN LED Droite

  digitalWrite(BUILTIN_LED, HIGH);
  digitalWrite(_OUT_MOT_ACK, LOW);
  while (!Serial) {
    ;
  }
  Serial.println("Dual VNH5019 Motor Shield");
  mySerial.begin(115200);
  while (!mySerial) {
    ;
  }
  md.init();
  readString = "";
}

void loop()
{
  //Lecture ports entrée
  int LedG = digitalRead(_IN_LED_G);
//  if(LedG)
//    Serial.println("LED G");
  int LedD = digitalRead(_IN_LED_D);
//  if(LedD)
//    Serial.println("LED D");
  int motSens = digitalRead(_IN_MOT_SENS);
  int motOn=0;
 
  //Allumage des pointeurs  
  digitalWrite(_OUT_LED_G,LedG); 
  digitalWrite(_OUT_LED_D,LedD);
  while(mySerial.available())
  {
    delay(10); //delay to allow byte to arrive in input buffer
    char c = mySerial.read();
    readString += c;
//    Serial.println(c);
  }
  if (readString.length() > 5)
  {
    Serial.println(readString);
    motDuree=readString.toFloat();
    readString = "";
    Serial.println(motDuree,4);
    motOn=1;
  }
   
 // Serial.println(motOn);

  if(motOn)
  {
    //Avancer moteur
    Serial.println("Mot ON"); 
    motOn=0;
    digitalWrite(_OUT_MOT_ACK, HIGH);
//    digitalWrite(BUILTIN_LED, HIGH);

//    motDuree = 1000;
    
    if(motSens==0)
    {
      Serial.println("sens 0");
      md.setM1Brake(0);
      md.setM1Speed(-VITESSE);
      stopIfFault();
      delay(motDuree*1000.);
      md.setM1Speed(0);
    }
    else
    {
      Serial.println("sens 1");
      md.setM1Brake(0);
      md.setM1Speed(VITESSE);
      stopIfFault();
      delay(motDuree*1000.);
      md.setM1Speed(0);
    }
    digitalWrite(_OUT_MOT_ACK,LOW);
  }
  else
  {
//    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(_OUT_MOT_ACK, LOW);
//    Serial.println("Mot Off");
    md.setM1Speed(0);
  }
}
