/////////////////////////////////////////////////LIBRARY
#include "DHT.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SFE_BMP180.h>
#include <Adafruit_BMP085.h>

/////////////////////////////////////////////////DEFINE
#define lm35 A1
#define Mq7A A0
#define swtch 2
#define statusLed 4
#define pot A2
#define redLed 3
#define greenLed 6
#define yellowLed 5

double P, T;
char status;

DHT dht(13, DHT22);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
Adafruit_BMP085 bmp;
SFE_BMP180 bmp180;

/////////////////////////////////////////////////SETUP
void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  bmp180.begin();
  pinMode(Mq7A, INPUT);
  pinMode(swtch, INPUT);
  pinMode(pot, INPUT);
  pinMode(statusLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  analogReference(INTERNAL);
}

/////////////////////////////////////////////////FUNCTIONS
float Temperature(){
  float ldrValue = analogRead(lm35);
  float milivolts = 1.1*(ldrValue/1023.0)*1000.0;
  return LM35Calibration(milivolts); 
}

float LM35Calibration(float milivolts){
  return 0.1012226066824530*milivolts-0.906629463;
}
//-------------------------------------------------------
float Humidity(){
  DHT22Calibration(dht.readHumidity());
}

float DHT22Calibration(int dhtvalue){
  return 1.23858*dhtvalue-15.5478;
}
//-------------------------------------------------------
float Pressure(){
  status = bmp180.startTemperature();
  delay(status);
  status = bmp180.getTemperature(T);
  delay(status);
  status = bmp180.startPressure(3);
  delay(status);
  status = bmp180.getPressure(P, T);
  delay(status);
  return (P*0.000986923);
}
//-------------------------------------------------------
float AirQuality(){
  float valueMq7 = analogRead(Mq7A);
  float ppmvalue = ((valueMq7)*1571.42/1023.0);
  int pwm = map((int)ppmvalue, 0, 1571, 0, 255*3);
  if(pwm > 255*2){
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    analogWrite(redLed, pwm-255*2);
  }
  else if(pwm > 255){
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, pwm-255);
    digitalWrite(redLed, LOW);
  }
  else{
    digitalWrite(greenLed, pwm);
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, LOW);
  }
  return ppmvalue;
}
//------------------------------------------------------
int LedAction(){
   digitalWrite(statusLed, HIGH);
   delay(4000);
    
  int contador = 0;
  while(contador <= 4){
    digitalWrite(statusLed, LOW);
    delay(125);
    digitalWrite(statusLed, HIGH);
    delay(125);
    contador++;
  } 
}

int LedAction2(){
   digitalWrite(statusLed, HIGH);
   delay(1000);
    
  int contador = 0;
  while(contador <= 4){
    digitalWrite(statusLed, LOW);
    delay(125);
    digitalWrite(statusLed, HIGH);
    delay(125);
    contador++;
  } 
}
//------------------------------------------------------
int Position(){
  int valuePot = analogRead(pot);
  if(valuePot >= 1023){
    return 1;
  }
  else if(valuePot < 1023){
    if(valuePot < 786){
      if(valuePot < 549){
          if(valuePot < 312){
            return 4;
          }
        }
        else {return 3;}
      }
      else {return 2;}
  }
}
//------------------------------------------------------
int initScreen(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESTACAO");
  lcd.setCursor(0, 1);
  lcd.print("METEOROLOGICA");
  delay(4000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GRUPO");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lucas");
  lcd.setCursor(0, 1);
  lcd.print("Fontenla");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bruno");
  lcd.setCursor(0, 1);
  lcd.print("Bitelli");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Guilherme");
  lcd.setCursor(0, 1);
  lcd.print("Graicer");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Iniciando.");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Iniciando..");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(1000);
}
/////////////////////////////////////////////////LOOP

int initOnce = 1;

void loop() {
  
  if(initOnce){
    initScreen();
    initOnce = 0;
    AirQuality();
    digitalWrite(statusLed, HIGH);
  }
  bool statBot = digitalRead(swtch);
  if(statBot){   
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("MODO RESUMO");
    delay(3000);  
    lcd.clear();
    while(digitalRead(swtch)){      
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(Temperature());
      
      lcd.setCursor(8, 0);
      lcd.print("H:");
      lcd.print(Humidity());
    
      lcd.setCursor(0, 1);
      lcd.print("P:");
      lcd.print(Pressure());
    
      lcd.setCursor(8, 1);
      lcd.print("C:");
      lcd.print(AirQuality());
      LedAction();
    }
  }
  else{  
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("MODO DINAMICO");
    delay(3000);
    
    int changeMode = 1;
    
    while(!digitalRead(swtch)){
      if(changeMode){
        lcd.clear();
        changeMode = 0;
      }
      int pos = Position();
      Serial.println(pos);
      
      if(pos == 1){
        lcd.setCursor(2, 0);
        lcd.print("Temperatura");
        lcd.setCursor(4, 1);
        lcd.print(Temperature());
        lcd.setCursor(9, 1);
        lcd.print("C");
        changeMode = 1;
        LedAction2();
      }
      if(pos == 2){
        lcd.setCursor(4, 0);
        lcd.print("Umidade");
        lcd.setCursor(4, 1);
        lcd.print(Humidity());
        lcd.setCursor(10, 1);
        lcd.print("%");
        changeMode = 1;
        LedAction2();
      }
     if(pos == 4){
        lcd.setCursor(2, 0);
        lcd.print("Nivel de CO");
        lcd.setCursor(3, 1);
        lcd.print(AirQuality());
        lcd.setCursor(9, 1);
        lcd.print("ppm");
        changeMode = 1;
        LedAction2();
     }
     if(pos == 3){
        lcd.setCursor(3, 0);
        lcd.print("Pressao");
        lcd.setCursor(4, 1);
        lcd.print(Pressure());
        lcd.setCursor(8, 1);
        lcd.print("atm");
        changeMode = 1;
        LedAction2();
     }
    }
  }
}
