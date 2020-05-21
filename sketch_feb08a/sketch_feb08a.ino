#include <Wire.h>

#define CLOCK_FREQUENCY 100000  // частота шины I2C
#define SPVSR_ADDRES    7       // адрес супервызера на шине I2C

String scaleString = "";        // считанная строка с CI-1560
long t;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(CLOCK_FREQUENCY);

}

void loop(){
  if (millis() > t){
    t = millis() + 500;
    
    Wire.requestFrom(SPVSR_ADDRES, 10);
    scaleString = "";
    delay(3);
    while (Wire.available()) {
      char inChar = (char)Wire.read();    // получаем новый байт
      scaleString += inChar;              // добавляем его к выходной строке
    }
    Serial.print(scaleString);
  }
}
