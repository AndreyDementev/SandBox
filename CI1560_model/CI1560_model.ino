
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
uint32_t t = 0;

byte scaleData[22];

void setup()
{
  Serial.begin(9600);
  lcd.init();                             // initialize the LCD
  lcd.backlight();
  lcd.print("Start");
  Serial.print("Start\n");
  scaleData[0]  = 'U';
  scaleData[1]  = 'S';
  scaleData[2]  = ',';
  scaleData[3]  = 'N';
  scaleData[4]  = 'T';
  scaleData[5]  = ',';
  scaleData[6]  = 3;    //Dev ID
  scaleData[7]  = 0;    //Lamp ST
  scaleData[8]  = ',';
  scaleData[17] = ' ';
  scaleData[18] = 'K';
  scaleData[19] = 'G';
  scaleData[20] = 13;   //CR
  scaleData[21] = 10;   //LF
  
}

void loop()
{
  if (millis() > t)  {
    t = millis() + 300;    
    uint32_t ct = t / 1000;
    scaleData[9]  = '-';  //может быть ' ' или '-'
    scaleData[16] = '0' + (ct % 10); ct /= 10;  // единицы секунд
    scaleData[15] = '0' + (ct % 6);  ct /=6;    // десятки секунд
    scaleData[14] = '0' + (ct % 10); ct /= 10;  // единицы минут
    scaleData[13] = '0' + (ct % 6);  ct /=6;    // десятки минут
    scaleData[12] = '0' + (ct % 10); ct /= 10;  // единицы часов
    scaleData[11] = '0' + (ct % 2);
    scaleData[10] = ' ';

    lcd.setCursor(0,0);
    for (int i = 0; i<9; i++) lcd.write((char)scaleData[i]);
    lcd.setCursor(0,1);
    for (int i = 9; i<22; i++) lcd.write((char)scaleData[i]);
    
    Serial.write(scaleData, 22);
  }
}
