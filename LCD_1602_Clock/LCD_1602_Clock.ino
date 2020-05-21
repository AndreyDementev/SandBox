//LCD 6402 Clock
//version:1.1

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 6;
const int LOADCELL_SCK_PIN = 7;
HX711 scale;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
uint32_t TimeS;

void TrueDisplay (long Number, byte Quantity, String Filler)
{
  String TextView = String(Number);
  for (int i = TextView.length(); i < Quantity; i++){TextView = Filler + TextView;}
  lcd.print(TextView);
}

void ClockDislay (uint8_t Col, uint8_t Row)
{
  if (int(millis()/1000) - TimeS) {            
    TimeS = millis()/1000;
    
    lcd.setCursor(Col,Row);                   
    TrueDisplay(TimeS/3600%24, 2, "0");
    lcd.print(":");
    TrueDisplay(TimeS/60%60, 2, "0");
    lcd.print(":");
    TrueDisplay(TimeS%60, 2, "0");     
  }
}

void setup()
{
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); 
  lcd.init();
  //lcd.backlight();
}

void loop()
{
  ClockDislay(0,0);
  lcd.setCursor(8,0);                     //обработанный вес
  TrueDisplay(long((scale.read_average(5)-174500)/370),8," ");
  lcd.setCursor(0,1);                     //необработанный вес
  TrueDisplay(long(scale.read_average(5)),16," ");
}
