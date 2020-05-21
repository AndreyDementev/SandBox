#include "EepromConfig.h"
#include <Eeprom24C32_64.h>



//********************************************************  
//                    WriteVarToEEPROM
//********************************************************
template <typename T>
void WriteVarToEEPROM(T Var, word Addres){
  for(int i=0; i<sizeof(Var); i++){
    eeprom.writeByte(Addres + i, (Var >> i*8) & 255);
    delay(10);
  }
}


//********************************************************  
//                    ReadVarFromEEPROM
//********************************************************
template <typename T>
void ReadVarFromEEPROM(T *Var, word Addres){
  T t = 0;
  for(int i=sizeof(t); i>0; i--){
    t = t << 8;
    t += eeprom.readByte(Addres + i - 1);
  }
  *Var = t;
/*  *Var = 0;
  for(int i=sizeof(t); i>0; i--){
    *Var = *Var << 8;
    *Var += eeprom.readByte(Addres + i - 1);
  }*/
}

  
//********************************************************  
//                    WritePrivetToEEPROM
//********************************************************  
void WritePrivetToEEPROM(PrivatData *pd){
  byte b, a = 0;
  cout << "----------------------------SavePrivetToEEPROM---\n";
  pd->CheckSumm = CheckSumm(pd);
  W[0] = PRIVET_DATA_COUNT;
  W[1] = pd->CheckSumm;
  W[2] = pd->ScaleTare;
  W[3] = pd->ScaleRate;
  W[4] = pd->LastMonth;
  W[5] = pd->LastWeek;
  W[6] = pd->LastDay;
  W[7] = pd->DayWeight;
  W[8] = pd->WeekWeight;
  W[9] = pd->MonthWeight;

  for(int i=0; i<PRIVET_DATA_COUNT; i++){
    Serial.println(W[i]);
    for(int j=0; j<sizeof(W[i]); j++){
      b = (W[i] >> j*8) & 255;
      eeprom.writeByte(EEPROM_PD_ADDRESS + a, b);
      a++;
      delay(10);
    }
  }  
}  

//********************************************************  
//                    ReadPrivetFromEEPROM
//********************************************************  
bool ReadPrivetFromEEPROM(PrivatData *pd){
  byte a = 0;
  cout << "----------------------------ReadPrivetFromEEPROM---\n";
  long l;
  for(int i=0; i<PRIVET_DATA_COUNT; i++){
    W[i] = 0;
    for(int j=0; j<sizeof(W[i]); j++){
      l = eeprom.readByte(EEPROM_PD_ADDRESS + a); 
      a++;
      W[i] += l << j*8;
    }
    Serial.println(W[i]);
  }
  
  pd->Size        = W[0];
  pd->CheckSumm   = W[1];
  pd->ScaleTare   = W[2];
  pd->ScaleRate   = W[3];
  pd->LastMonth   = W[4];
  pd->LastWeek    = W[5];
  pd->LastDay     = W[6];
  pd->DayWeight   = W[7];
  pd->WeekWeight  = W[8];
  pd->MonthWeight = W[9];  

 
  if (pd->CheckSumm != CheckSumm(pd)){
    Serial.print("Прочитано "); Serial.print(pd->CheckSumm);
    Serial.print(", расчитано ");  Serial.println(CheckSumm(pd));  
    Init(pd);
    WritePrivetToEEPROM(pd);
    return 0;    
  }
  else Serial.println("При загрузке PRIVET DATA from EEPROM контрольные суммы совпали");
  return 1;
}  
