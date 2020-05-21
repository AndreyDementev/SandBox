//LCD 1602 Clock
//version:1.1

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
uint32_t TimeS = 0;

String TrueDisplay (long Number, byte Quantity, String Filler)
{
  String TextView = String(Number);
  for (int i = TextView.length(); i < Quantity; i++){TextView = Filler + TextView;}
  lcd.print(TextView);
  return TextView;
}

String ClockDislay (uint8_t Col, uint8_t Row)
{
  String s;
  uint32_t t;
  t = millis()/1000;
  lcd.setCursor(Col,Row);
  s = TrueDisplay(t/3600%60, 2, "0");
  lcd.print(":");
  s += ":" + TrueDisplay(t/60%60, 2, "0");
  lcd.print(":");
  s += ":" + TrueDisplay(t%60, 2, "0");
  return s;
}

void setup()
{
  Serial.begin(9600);
  lcd.init();                             // initialize the LCD
  lcd.backlight();
  Serial.print("Start\n");
}

void loop()
{
  if (millis() > TimeS)
  {
    TimeS = millis() + 1000;
    Serial.print(ClockDislay(0,0) + char(13) + char(10));
  }
}
