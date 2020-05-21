#ifndef _SaveDataToSD_
#define _SaveDataToSD_

#include "/Users/andrey/Рабочая/Сафоновский ЖБИ/Проект Arduino/MyLibraries/Definitions.h"
#include <SPI.h>
#include "SdFat.h"
#include <DS3231.h>

#define _DEBUG_

#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h


//********************************************************  
//                    CorrectDisplay
//********************************************************  
template <typename T>
String CorrectDisplay(T Number, byte Quantity, String Filler){
  String TextView = String(Number);
  if (TextView.length() > Quantity) TextView = TextView.substring(0, Quantity);
  for (int i = TextView.length(); i < Quantity; i++) TextView = Filler + TextView;
  return TextView;
}

//********************************************************  
//                    DateToString
//********************************************************  
void DateToString(Time t){
  dt.Year  = CorrectDisplay(String(t.year, DEC), 4, "0");
  dt.Year2 = CorrectDisplay(String(t.year - 2000, DEC), 2, "0");
  dt.Mon   = CorrectDisplay(String(t.mon, DEC), 2, "0");
  dt.Day   = CorrectDisplay(String(t.date, DEC), 2, "0");
  dt.Hour  = CorrectDisplay(String(t.hour, DEC), 2, "0");
  dt.Min   = CorrectDisplay(String(t.min, DEC), 2, "0");
  dt.Sec   = CorrectDisplay(String(t.sec, DEC), 2, "0");
}

//********************************************************  
//                    GetDate
//********************************************************  
String GetDate(){
//         TmpFileName = GetTmpFileName();
//         SaveLogToSD(TmpFileName.c_str(), "SaveDataToSD:1:rtc.getTime()");  
  DateToString(rtc.getTime());
//         DeleteFileFromSD(TmpFileName.c_str());
  return dt.Day + "." + dt.Mon + "." + dt.Year;
}

//********************************************************  
//                    GetTime
//********************************************************  
String GetTime(){
//         TmpFileName = GetTmpFileName();
//         SaveLogToSD(TmpFileName.c_str(), "SaveDataToSD:2:rtc.getTime()");  
  DateToString(rtc.getTime());
//         DeleteFileFromSD(TmpFileName.c_str());
  return dt.Hour + ":" + dt.Min + ":" + dt.Sec;
}

//********************************************************  
//                    GetShortDate
//********************************************************  
String GetShortDate(){
//         TmpFileName = GetTmpFileName();
//         SaveLogToSD(TmpFileName.c_str(), "SaveDataToSD:3:rtc.getTime()");  
  DateToString(rtc.getTime());
//         DeleteFileFromSD(TmpFileName.c_str());
  return dt.Year2 + dt.Mon;
}

 
//********************************************************  
//                    GetFileName
//********************************************************  
String GetFileName(){
  return GetShortDate() + FILE_BASE_NAME;
}
 
//********************************************************  
//                    GetTmpFileName
//********************************************************  
String GetTmpFileName(){
  uint32_t RndName = random(1)*0xFFFFFFF * millis(); // типа случайное число
  return String(RndName, HEX) + ".tmp";
}

//********************************************************  
//                    SavePrivetToSD
//********************************************************  
bool SavePrivetToSD(PrivatData *pd){
/*  cout << "----------------------------SavePrivetToSD---\n";
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

  for(int i = 0; i<PRIVET_DATA_COUNT; i++) Serial.println(W[i]);

  if (!SoftSD.remove(PRIVET_FILE_NAME)){
    cout << "Ошибка стирания файла \\pvData01.dav перпед записью\n";
    return 0;
  }
  if (!SoftFile.open(PRIVET_FILE_NAME , O_RDWR | O_APPEND | O_CREAT)){
    cout << "Ошибка открытия файла \\pvData01.dav для записи\n";
    SoftFile.close();
    return 0;
  }
  for(int i=0; i<PRIVET_DATA_COUNT; i++)
    for(int j=0; j<sizeof(W[i]); j++){
      SoftFile.write((W[i] >> j*8) & 255);
    }  
  SoftFile.close();*/
  return 1;
}  

//********************************************************  
//                    ReadPrivetFromSD
//********************************************************  
bool ReadPrivetFromSD(PrivatData *pd){
/*  cout << "----------------------------ReadPrivetFromSD---\n";
  long l;
  if (!SoftFile.open(PRIVET_FILE_NAME , O_RDWR | O_APPEND | O_CREAT)){
    cout << "Ошибка открытия файла \\pvData01.dav для чтения\n";
    return 0;
  }
  SoftFile.seekSet(0);
  for(int i=0; i<PRIVET_DATA_COUNT; i++){
    W[i] = 0;
    for(int j=0; j<sizeof(W[i]); j++){
      l = SoftFile.read(); 
      W[i] += l << j*8;
    }
  }
  SoftFile.close();

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

  for(int i = 0; i<PRIVET_DATA_COUNT; i++){
    Serial.println(W[i]);
  }
 
  if (pd->CheckSumm != CheckSumm(pd)){
    Serial.print("Прочитано "); Serial.print(pd->CheckSumm);
    Serial.print(", расчитано ");  Serial.println(CheckSumm(pd));  
    Init(pd);
    SavePrivetToSD(pd);
    return 0;    
  }
  else Serial.println("При загрузке \\pvData01.dav контрольные суммы совпали");*/
  return 1;
}  

//********************************************************  
//                    SaveLogToSD
//********************************************************  
bool SaveLogToSD(const char* FileName, const char* Message){
#ifdef _LOG_SAVE_
  if (SdEjectButton) return 0;                                         // кнопка извлечения SD нажата
    HSD_ON();
    if (!HardSD.begin(HARD_CS_PIN, SPI_SPEED)){
      return 0;
    }
    HardFile = HardSD.open(FileName, FILE_WRITE);
    HardFile.timestamp(T_WRITE, t.year, t.mon, t.date, t.hour, t.min, t.sec);
    
    HardFile.write(GetDate().c_str());
    HardFile.write(" ");
    HardFile.write(GetTime().c_str());
    HardFile.write("   ");
    HardFile.write(Message);
    HardFile.close();
    HSD_OFF();
#endif // _LOG_SAVE_
    return 1;
}

//********************************************************  
//                    SaveLogToSoftSD
//********************************************************  
bool SaveLogToSoftSD(const char* FileName, const char* Message){/*
  SoftSD.begin(SOFT_CS_PIN, SPI_SPEED);
  SoftFile.open(FileName, O_RDWR | O_APPEND | O_CREAT);
  SoftFile.timestamp(T_WRITE, t.year, t.mon, t.date, t.hour, t.min, t.sec);
  SoftFile.write(Message);
  SoftFile.close();*/
    return 1;
}

//********************************************************  
//                    DeleteFileFromSD
//********************************************************
bool DeleteFileFromSD(const char* FileName){/*
  if (!SdEjectButton){  //++++++++++++++++++++++++++++++  кнопка извлечения SD не нажата
    if (!HardSD.begin(HARD_CS_PIN, SPI_SPEED)){
      return 0;
    }
    HardSD.remove(FileName);
    return 1;
  }*/
    return 1;
}

//********************************************************  
//                    DeleteFileFromSoftSD
//********************************************************
bool DeleteFileFromSoftSD(const char* FileName){/*
  SoftSD.begin(SOFT_CS_PIN, SPI_SPEED);
  SoftSD.remove(FileName);*/
    return 1;
}


//********************************************************  
//                    ShakeSD
//********************************************************
void ShakeSD(){
  if (digitalRead(HSD_POWER_PIN) == RELAY_OFF) return;
  pinMode(HSD_POWER_PIN, OUTPUT);
  digitalWrite(HSD_POWER_PIN, RELAY_OFF);
  delay(50);
  digitalWrite(HSD_POWER_PIN,RELAY_ON);
  Serial.println("---------SHAKE HSD POWER");
  delay(50);
}

//********************************************************  
//                    HSD_ON
//********************************************************
void HSD_ON(){
  digitalWrite(HSD_POWER_PIN,RELAY_ON);
  Serial.println("---------HSD POWER ON");
  delay(200);
}

//********************************************************  
//                    HSD_OFF
//********************************************************
void HSD_OFF(){
  digitalWrite(HSD_POWER_PIN,RELAY_OFF);
  Serial.println("---------HSD POWER OFF");
}

//********************************************************  
//                    SaveDataToSD
//********************************************************  
String SaveDataToSD(int StartWeight, int EndWeight, int SumWeight) {
//  cout << "----------------------------SaveDataToSD---\n";

  String Result = "";
  String FileName = GetFileName();
  bool SfOpen;
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
  watchdogReset();
#endif  
  
  
  if (!SdEjectButton){  //++++++++++++++++++++++++++++++  кнопка извлечения SD не нажата
  // Если внешний SD не запущен пытаемся перезапустить
  //----------------------------------------------------------------------------------------------------------------
  HSD_ON();
  
    if(HardSD.begin(HARD_CS_PIN, SPI_SPEED)) {
      HSDS = true;
    }
    else{
      HSDS = false;
      cout << "Не стартовал HSD в SaveDataToSD\n";
    }
    
  //Если внешний SD не запущен, пропускаем открытие файла
  //--------------------------------------------------------  
    if (HSDS){  
      
  //Открываем внешний файл
  //--------------------------------------------------------
      HardFile = HardSD.open(FileName, FILE_WRITE);
      if (HardFile.size() == 0) HardFile.write(TABLE_HEADER);
      if (!HardFile) {
        HFS = false;
        Result += "Ошибка открытия HardSD\\" + FileName + "\n";
      }  
      else{
        HFS = true;
        HardFile.seekEnd(0);
      }  
    }
  }                         //++++++++++++++++++++++++++++++  конец кнопки

// Если внутренний SD незапущен пытаемся перезапустить
//----------------------------------------------------------------------------------------------------------------

  if (SoftSD.begin(SOFT_CS_PIN, SPI_SPEED)){
    SSDS = true; 
  }
  else{
    SSDS = false;
    cout << "Не стартовал SSD в SaveDataToSD\n";
  }


//Если внутренний SD не запущен, пропускаем открытие файла
//--------------------------------------------------------  
  if (SSDS){  
//Открываем внутренний файл
//--------------------------------------------------------

    if (!SoftFile.open(FileName.c_str(), O_RDWR | O_APPEND | O_CREAT)){
      SFS = false;
      Result += "Ошибка открытия SoftSD\\" + FileName + "\n";
    }
    else{
      SFS = true;
      if (SoftFile.fileSize() == 0) SoftFile.write(TABLE_HEADER);   
    }
  }
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
  watchdogReset();
#endif
// Проверяем целостность внешнего и внутреннего файлов. Если один из файлов неоткрыт - пропускаем
//----------------------------------------------------------------------------------------------------------------
  //Serial.print("HFS = "); Serial.print(HFS); Serial.print("   SFS = "); Serial.println(SFS);
  
  if (!SdEjectButton){  //++++++++++++++++++++++++++++++  кнопка извлечения SD не нажата
    if (HFS && SFS) Result += CheckLogFiles(FileName);
  }                     //++++++++++++++++++++++++++++++  конец кнопки
// Дописываем к двум логам строку с данными
//--------------------------------------------------------


  String s = GetDate() + ";" + GetTime() + ";" + StartWeight + ";" + EndWeight + ";";
  s +=  String(SumWeight) + "\n";
//  s +=  String((EndWeight - StartWeight)) + "\n";

  
  if (SFS){
    SoftFile.write(s.c_str());
    SoftFile.close();
  }
  if (!SdEjectButton){  //++++++++++++++++++++++++++++++  кнопка извлечения SD не нажата
    if (HFS){
      HardFile.write(s.c_str());
      HardFile.close();    
    }                   //++++++++++++++++++++++++++++++  конец кнопки
  }  
  HSD_OFF();
  return Result;
}


//********************************************************  
//                  CheckLogFiles
//********************************************************  
String CheckLogFiles(String FileName){

//  Проверяем равенство размеров файлов на HSD и SSD
//--------------------------------------------------------
  if (SoftFile.fileSize() == HardFile.size()) return "Done";


//                     = Файлы не равны =
//  Сравниваем с EEPROM в часах.Выбираем один по мажоритарной
//  системе. Предполагаем что 2 источника одновременно не испортятся

//--------------------------------------------------------
//  Стираем файл на внешнем SD и создаем его заново (мажорство во 2 версии)
  if (!HardFile){
    HFS = false;
    return "Ошибка чтения HardSD\\" + FileName + " при синхронизации логов";
  }
  HardFile.close();
  HardSD.remove(FileName.c_str());
  HardFile = HardSD.open(FileName, FILE_WRITE);  
  if (!HardFile) {
    HFS = false;
    return "Ошибка открытия HardSD\\" + FileName + " при синхронизации логов";
  }   

//  Проверяем свободное место на HSD
//--------------------------------------------------------

//  Копируем файл из SSD в HSD
//--------------------------------------------------------

  SoftFile.seekSet(0);                                // Копируем с начала файла
  while (SoftFile.available()){
    DataByte = SoftFile.read();
    HardFile.write(DataByte);
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
    watchdogReset();
#endif
  }
//  ShakeSD();
  if(HardSD.begin(HARD_CS_PIN, SPI_SPEED)) {
    HSDS = true;
  }
  else{
    HSDS = false;
      cout << "Не стартовал HSD после копирования на него\n";
  }
  return "Done";
}

#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS

#endif //_SaveDataToSD_
