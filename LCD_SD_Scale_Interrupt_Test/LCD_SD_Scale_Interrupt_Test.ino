#include <Wire.h>
#include "HX711.h"

#define BUTTON_PIN            31
#define LOADCELL_DOUT_PIN     33 
#define LOADCELL_SCK_PIN      35
#define CONTACT_BOUNCE_DELAY  20
#define SCALE_FACTOR          370
#define READE_COUNT           3

HX711 scale;

uint32_t TimeS;
long     Weight, WeightStart, WeightEnd;
boolean  ButtonStatus = false;

String CorrectDisplay(long Number, byte Quantity, String Filler){
  String TextView = String(Number);
  for (int i = TextView.length(); i < Quantity; i++){TextView = Filler + TextView;}
  return TextView;
}

/*
void ClockView(uint8_t Col, uint8_t Row){
  lcd.setCursor(Col,Row);                   
  CorrectDisplay(TimeS/3600%60, 2, "0");
  lcd.print(":");
  CorrectDisplay(TimeS/60%60, 2, "0");
  lcd.print(":");
  CorrectDisplay(TimeS%60, 2, "0");     
}
*/

/*  
void ButtonPressed(){
  Serial.println("In ISR");
  digitalWrite(LED_PIN, digitalRead(BUTTON_PIN));
  interrupts();                           // Устройствам I2C для работы требуются прерывания
  lcd.setBacklight(digitalRead(BUTTON_PIN));
  noInterrupts();                         // Возвращаем запрет на прерывания
}
*/

void setup(){
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  pinMode(BUTTON_PIN, INPUT); 
  Serial.begin(115200);
}

void loop(){
  if (ButtonStatus != digitalRead(BUTTON_PIN)){
    ButtonStatus = !ButtonStatus;
    if (ButtonStatus){
      Serial.println("Load Start");
      WeightStart = scale.read_average(READE_COUNT);
    }
    else{
      Serial.print("Load End");
      WeightEnd = scale.read_average(READE_COUNT);
      Serial.print(" --> Load Weight = ");
      Serial.println((WeightEnd-WeightStart)/SCALE_FACTOR);
   }
  }
  
  if (int(millis()/500) - TimeS) { 
    TimeS = millis()/500;
    Weight = scale.read_average(READE_COUNT);
    Serial.println(CorrectDisplay((Weight-175800)/SCALE_FACTOR,4," "));
  }
}
