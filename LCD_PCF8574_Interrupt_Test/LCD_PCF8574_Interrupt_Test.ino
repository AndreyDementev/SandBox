#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "HX711.h"

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int LOADCELL_DOUT_PIN = 27; 
const int LOADCELL_SCK_PIN  = 26;
const int LED_PIN           = 13;
const int BUTTON_PIN        = 21;
HX711 scale;

uint32_t TimeS;
boolean ButtonStatus = false;

void CorrectDisplay(long Number, byte Quantity, String Filler){
  String TextView = String(Number);
  for (int i = TextView.length(); i < Quantity; i++){TextView = Filler + TextView;}
  lcd.print(TextView);
}

void ClockView(uint8_t Col, uint8_t Row){
  

  lcd.setCursor(Col,Row);                   
  CorrectDisplay(TimeS/3600%60, 2, "0");
  lcd.print(":");
  CorrectDisplay(TimeS/60%60, 2, "0");
  lcd.print(":");
  CorrectDisplay(TimeS%60, 2, "0");     

}

void ButtonPressed(){
/*  
  Serial.println("In ISR");
  digitalWrite(LED_PIN, digitalRead(BUTTON_PIN));
  interrupts();                           // Устройствам I2C для работы требуются прерывания
  lcd.setBacklight(digitalRead(BUTTON_PIN));
  noInterrupts();                         // Возвращаем запрет на прерывания
*/
}

void setup(){
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  lcd.begin(16, 2);                       // initialize the lcd
  lcd.setBacklight(0);
 
  pinMode(LED_PIN,OUTPUT);
  pinMode(BUTTON_PIN, INPUT); 
  attachInterrupt(2, ButtonPressed, CHANGE);

  Serial.begin(115200);
}

void loop(){
  if (ButtonStatus != digitalRead(BUTTON_PIN)){
    ButtonStatus != ButtonStatus;
    lcd.setBacklight(digitalRead(ButtonStatus));
  }
  if (int(millis()/1000) - TimeS) { 
    TimeS = millis()/1000;
    ClockView(0,0);
  }
  lcd.setCursor(8,0);                     //обработанный вес
  CorrectDisplay(long((scale.read_average(5)-174500)/370),8," ");
  lcd.setCursor(0,1);                     //необработанный вес
  CorrectDisplay(long(scale.read_average(5)),16," ");
}
