#include <Wire.h>

#define CLOCK_FREQUENCY 100000  // частота шины I2C
#define SVSR_ADDRES    7       // адрес супервызера на шине I2C

// считанная строка с CI-1560
byte scaleData[23] = {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-',
                      '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\r', '\n'};
uint32_t t;

#pragma pack(push,1)
union CI1560_dataFrame
{
    byte b[24];
    char c[23] = {'0','S','T',',','N','T',',','_','_',',','-','0','0','1','7','5',',','5',' ','K','G','\r','\n'};
    struct
    {
        char frameSt;
        char quakeSt[2];
        byte z1;
        char weightSt[2];
        byte z2;
        byte devID;
        byte lampSt;
        byte z3;
        char Weight[8];
        byte z4;
        char Tail[4];
    };
};
#pragma pack(pop)
CI1560_dataFrame dF;


void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(CLOCK_FREQUENCY);

//  ltoa(36455667, dF.Weight, 10);
  dF.z4 = 0;
  Serial.println(atof(dF.Weight));
  Serial.println(dF.Weight);
  Serial.println(dF.c);
  
  while(1);
}

void loop(){
  if (millis() > t){
    t = millis() + 500;
    
    Wire.requestFrom(SVSR_ADDRES, 23);
    scaleData[0] = *"";
    delay(3);
    while (Wire.available()) {
//      char inChar = (char)Wire.read();    // получаем новый байт
//      scaleString += inChar;              // добавляем его к выходной строке
    }
    Serial.write(scaleData, 23);
  }
}

#pragma pack(push,1)
struct MyBitStruct
{
    uint16_t a:4;
    uint16_t b:4;
    uint16_t c;
};
#pragma pack(pop)
