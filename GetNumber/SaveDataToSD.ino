
#ifndef _SPI_H_
  #include <SPI.h>
#endif
#ifndef SdFat_h
  #include "SdFat.h"
#endif
#include <DS3231.h>

#define _DEBUG_

#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
/*
// Pin numbers on Display SD.
#define SOFT_MOSI_PIN 11     
#define SOFT_MISO_PIN 12 
#define SOFT_SCK_PIN  13     
#define SOFT_CS_PIN   10
// Pin numbers on Shied SD.
#define HARD_CS_PIN   53  
// File names
#define INT_LOG_FILE_NAME "ASURZLog.csv"  // On Display
#define OUT_LOG_FILE_NAME "ASURZLog.csv"  // On Shild
#define FILE_BASE_NAME    "SURZ.csv"      // In both times
#define TABLE_HEADER      "Date;Time;Start;End;Weight\n" //Заголовок таблицы excel
*/
// serial output stream
// ArduinoOutStream cout(Serial);
/*
// software SPI
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SoftSD;
SdFile SoftFile;
bool SSDS = true; // статут SSD. false - дисковод недоступен
bool SFS = false; // статус открытия файла на внутреннем диске
// hardware SPI
SdFat HardSD;
File HardFile;
bool HSDS = true; // статут HSD. false - дисковод недоступен
bool HFS = false; // статус открытия файла на внешнем диске

  
Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
*/

/*
//Time    t;

//struct DateTime{String Year, Year2, Mon, Day, Hour, Min, Sec;};
//DateTime dt;


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
//  DateToString(rtc.getTime());
  return dt.Day + "." + dt.Mon + "." + dt.Year;
}

//********************************************************  
//                    GetTime
//********************************************************  
String GetTime(){
//  DateToString(rtc.getTime());
  return dt.Hour + ":" + dt.Min + ":" + dt.Sec;
}

//********************************************************  
//                    GetShortDate
//********************************************************  
String GetShortDate(){
//  DateToString(rtc.getTime());
  return dt.Year2 + dt.Mon;
}

 
//********************************************************  
//                    GetFileName
//********************************************************  
String GetFileName(){
  return GetShortDate() + FILE_BASE_NAME;
}

//********************************************************  
//                    SavePrivetToSD
//********************************************************  
bool SavePrivetToSD(PrivatData *pd){

  cout << "----------------------------SavePrivetToSD---\n";
  pd->CheckSumm = CheckSumm(pd);
  W[0] = PRIVET_DDATA_COUNT;
  W[1] = pd->CheckSumm;
  W[2] = pd->ScaleTare;
  W[3] = pd->ScaleRate;
  W[4] = pd->LastMonth;
  W[5] = pd->LastWeek;
  W[6] = pd->LastDay;
  W[7] = pd->DayWeight;
  W[8] = pd->WeekWeight;
  W[9] = pd->MonthWeight;

  for(int i = 0; i<PRIVET_DDATA_COUNT; i++){
    Serial.println(W[i]);
  }

  Serial.print("CheckSumm = "); Serial.println(W[1]);
  if (!SoftSD.remove(PRIVET_FILE_NAME)){
    cout << "Ошибка стирания файла \\pvData01.dav перпед записью\n";
    return 0;
  }
  if (!SoftFile.open(PRIVET_FILE_NAME , O_RDWR | O_APPEND | O_CREAT)){
    cout << "Ошибка открытия файла \\pvData01.dav для записи\n";
    SoftFile.close();
    return 0;
  }
//  Serial.print("pd.CheckSumm = "); Serial.print(pd->CheckSumm); Serial.println(" перед записью");  
  for(int i=0; i<PRIVET_DDATA_COUNT; i++)
    for(int j=0; j<sizeof(W[i]); j++){
      SoftFile.write((W[i] >> j*8) & 255);
    }  
  SoftFile.close();


  return 1;
}  

//********************************************************  
//                    ReadPrivetFromSD
//********************************************************  
bool ReadPrivetFromSD(PrivatData *pd){
  cout << "----------------------------ReadPrivetFromSD---\n";
//  Serial.print("pd.Size = "); Serial.print(pd->Size); Serial.println(" в ReadPrivetFromSD");
  long l;
  
  if (!SoftFile.open(PRIVET_FILE_NAME , O_RDWR | O_APPEND | O_CREAT)){
    cout << "Ошибка открытия файла \\pvData01.dav для чтения\n";
    return 0;
  }
  
  
  SoftFile.seekSet(0);
  for(int i=0; i<PRIVET_DDATA_COUNT; i++){
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

  for(int i = 0; i<PRIVET_DDATA_COUNT; i++){
    Serial.println(W[i]);
  }
 
  if (pd->CheckSumm != CheckSumm(pd)){
    Serial.print("Прочитано "); Serial.print(pd->CheckSumm);
    Serial.print(", расчитано ");  Serial.println(CheckSumm(pd));  
    Init(pd);
    SavePrivetToSD(pd);
    return 0;    
  }
  else Serial.println("При загрузке \\pvData01.dav контрольные суммы совпали");
  return 1;
}  

//********************************************************  
//                    SaveDataToSD
//********************************************************  
String SaveDataToSD(int StartWeight, int EndWeight, int SumWeight) {
//  cout << "----------------------------SaveDataToSD---\n";

  String Result = "";
  String FileName = GetFileName();
  bool SfOpen, Flag = false;
  
if (!SdEjectButton){  //++++++++++++++++++++++++++++++  кнопка извлечения SD не нажата
// Если внешний SD не запущен пытаемся перезапустить
//----------------------------------------------------------------------------------------------------------------
//  if (!HSDS){
    if(HardSD.begin(HARD_CS_PIN, SPI_QUARTER_SPEED)) {
//      HSDS = true;
      Flag = true; 
    }
    else{
//      HSDS = false;
    }
//  }
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

// Если внутренний SD незапущен пытаемся перезапустить
//----------------------------------------------------------------------------------------------------------------
//  if (!SSDS){
    if (SoftSD.begin(SOFT_CS_PIN, SPI_QUARTER_SPEED)){
//      SSDS = true; 
    }
//    else SSDS = false;
}                         //++++++++++++++++++++++++++++++  конец кнопки

//Если внутренний SD не запущен, пропускаем открытие файла
//--------------------------------------------------------  
  if (SSDS){  
//Открываем внутренний файл
//--------------------------------------------------------

//    if (Flag){                                      //Если произошло повторное подключение
//      SoftSD.begin(SOFT_CS_PIN, SPI_QUARTER_SPEED); //внешнего SD то необходимо переподключить 
//      Flag = false;                                 //внутренний SD, иначе файл обнуляется!
//    }
    if (!SoftFile.open(FileName.c_str(), O_RDWR | O_APPEND | O_CREAT)){
      SFS = false;
      Result += "Ошибка открытия SoftSD\\" + FileName + "\n";
    }
    else{
      SFS = true;
      if (SoftFile.fileSize() == 0) SoftFile.write(TABLE_HEADER);   
    }
  }

// Проверяем целостность внешнего и внутреннего файлов. Если один из файлов неоткрыт - пропускаем
//----------------------------------------------------------------------------------------------------------------
  //Serial.print("HFS = "); Serial.print(HFS); Serial.print("   SFS = "); Serial.println(SFS);
  
if (!SdEjectButton){  //++++++++++++++++++++++++++++++  кнопка извлечения SD не нажата
  if (HFS && SFS) Result += CheckLogFiles(SoftFile, HardFile, FileName);
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
  return Result;
}


//********************************************************  
//                  CheckLogFiles
//********************************************************  
String CheckLogFiles(SdFile &SF, File &HF, String FileName){

//  Проверяем равенство размеров файлов на HSD и SSD
//--------------------------------------------------------
  if (SF.fileSize() == HF.size()) return "Done";


//                     = Файлы не равны =
//  Сравниваем с EEPROM в часах.Выбираем один по мажоритарной
//  системе. Предполагаем что 2 источника одновременно не испортятся

//--------------------------------------------------------
//  Стираем файл на внешнем SD и создаем его заново (мажорство во 2 версии)
  if (!HF){
    HFS = false;
    return "Ошибка чтения HardSD\\" + FileName + " при синхронизации логов";
  }
  HF.close();
  HardSD.remove(FileName.c_str());
  HF = HardSD.open(FileName, FILE_WRITE);  
  if (!HF) {
    HFS = false;
    return "Ошибка открытия HardSD\\" + FileName + " при синхронизации логов";
  }   

//  Проверяем свободное место на HSD
//--------------------------------------------------------

//  Копируем файл из SSD в HSD
//--------------------------------------------------------

  SF.seekSet(0);                                // Копируем с начала файла
  while (SF.available()) HF.write(SF.read());
  return "Done";
}
/*
//********************************************************  
//                  Setup()
//******************************************************** 
void setup() {
#ifdef _DEBUG_ 
  Serial.begin(9600);
  while (!Serial) SysCall::yield();
  cout << "Initializing RTC...\n";
#endif

  rtc.begin();

#ifdef _DEBUG_
  Serial.println(GetDate());
  Serial.println(GetTime());
  Serial.println(GetShortDate());
  Serial.println(GetFileName());
#endif


// Открываем хардверный SD
//--------------------------------------------------------
#ifdef _DEBUG_
  cout << "Initializing Outer SD card...";
#endif   
  if (!HardSD.begin(HARD_CS_PIN, SPI_QUARTER_SPEED)) {
#ifdef _DEBUG_ 
    cout << "HardSD.begin failed\n";
#endif
  HSDS = false;
  }
#ifdef _DEBUG_ 
  else{
    cout << "initialization HardSD done.\n";
  }
#endif

// Открываем софтверный SD
//--------------------------------------------------------
#ifdef _DEBUG_
  cout << "Initializing Inner SD card...";
#endif  
  if (!SoftSD.begin(SOFT_CS_PIN, SPI_HALF_SPEED)) {
#ifdef _DEBUG_ 
    cout << "SoftSD.begin failed\n";
#endif
SSDS = false;
  }
#ifdef _DEBUG_ 
  else{
    cout << "initialization SoftSD done.\n";
  }
#endif
}



String s;
int Start, End;

//********************************************************  
//                  Loop()
//******************************************************** 
void loop() {
  for (int i=0; i<10; i++){
    Start = random(0,30);
    End   = random(130,160);
    s = SaveDataToSD(Start, End);
    delay(5000);
  }
  cout << "Записано!\n";
  while (1);
}
*/
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS

  
/*  for(int i=0; i<PRIVET_DDATA_COUNT; i++){
    for(int j=0; j<sizeof(W[i]); j++){
      Serial.print((W[i] >> j*8) & 255, HEX); Serial.print(", ");
    }
    Serial.println();      
  } */ 
  /*  
  for(int i=0; i<PRIVET_DDATA_COUNT; i++){
    for(int j=0; j<sizeof(W[i]); j++){
      Serial.print((W[i] >> j*8) & 255, HEX); Serial.print(", ");
    }
    Serial.println();    
  }  
*/ 
