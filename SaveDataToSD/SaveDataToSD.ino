
#ifndef _SPI_H_
  #include <SPI.h>
#endif
#ifndef SdFat_h
  #include "SdFat.h"
#endif
#include <DS3231.h>

#define _DEBUG_

#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h

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

// serial output stream
ArduinoOutStream cout(Serial);

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

  
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Time    t;

struct DateTime{String Year, Year2, Mon, Day, Hour, Min, Sec;};
DateTime dt;

//**************************************************************************************
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
void GetDate(Time t, DateTime &dt){
  dt.Year  = CorrectDisplay(String(t.year, DEC), 2, "0");
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
  GetDate(rtc.getTime(), dt);
  return dt.Day + "." + dt.Mon + "." + dt.Year;
}

//********************************************************  
//                    GetTime
//********************************************************  
String GetTime(){
  t = rtc.getTime();
  return dt.Hour + ":" + dt.Min + ":" + dt.Sec;
}

//********************************************************  
//                    GetShortDate
//********************************************************  
String GetShortDate(){
  t = rtc.getTime();
  return dt.Year2 + dt.Mon;
}

 
//********************************************************  
//                    GetFileName
//********************************************************  
String GetFileName(){
  return GetShortDate() + FILE_BASE_NAME;
}

//********************************************************  
//                    SaveDataToSD
//********************************************************  
String SaveDataToSD(int StartWeight, int EndWeight) {

  String Result = "";
  String FileName = GetFileName();
  bool SfOpen, Flag = false;

// Если внешний SD не запущен пытаемся перезапустить
//----------------------------------------------------------------------------------------------------------------
  if (!HSDS || !HFS){
    if(HardSD.begin(HARD_CS_PIN, SPI_QUARTER_SPEED)) {
      HSDS = true;
#ifdef _DEBUG_  
      Flag = true;
      cout << ".....................................HardSD initialised after failure\n"; 
      cout << "1 HSDS = " << HSDS << "  HFS = " << HFS << "\n";
#endif       
    }
    else{
      HSDS = false;
#ifdef _DEBUG_  
      cout << ".....................................Initialisation HardSD failed\n";
      cout << "2 HSDS = " << HSDS << "  HFS = " << HFS << "\n";
#endif       
    }
  }
//Если внешний SD не запущен, пропускаем открытие файла
//--------------------------------------------------------  
  if (HSDS){  
//Открываем внешний файл
//--------------------------------------------------------
#ifdef _DEBUG_  
    Serial.print("Opening HardSD\\" + FileName + "...");
#endif
    HardFile = HardSD.open(FileName, FILE_WRITE);
    if (HardFile.size() == 0) HardFile.write(TABLE_HEADER);
    if (!HardFile) {
      HFS = false;
#ifdef _DEBUG_
      cout << "failed\n";
      cout << "3 HSDS = " << HSDS << "  HFS = " << HFS << "\n";
#endif
      Result += "Ошибка открытия HardSD\\" + FileName + "\n";
    }  
    else{
      HFS = true;
      HardFile.seekEnd(0);
#ifdef _DEBUG_
     cout << "opened\n";
     cout << "4 HSDS = " << HSDS << "  HFS = " << HFS << "\n";
#endif
    }  
  }

// Если внутренний SD незапущен пытаемся перезапустить
//----------------------------------------------------------------------------------------------------------------
  if (!SSDS || !SFS){
    if (SoftSD.begin(SOFT_CS_PIN, SPI_QUARTER_SPEED)) {
    SSDS = true;
#ifdef _DEBUG_  
    cout << ".....................................SoftSD initialised after failure\n";
    cout << "1 SSDS = " << SSDS << "  SFS = " << SFS << "\n";
#endif  
    }
    else{
    SSDS = false;
#ifdef _DEBUG_  
      cout << ".....................................Initialisation SoftSD failed\n";    
      cout << "2 SSDS = " << SSDS << "  SFS = " << SFS << "\n";
#endif  
    }
  }

//Если внешний SD не запущен, пропускаем открытие файла
//--------------------------------------------------------  
  if (SSDS){  
//Открываем внутренний файл
//--------------------------------------------------------
 #ifdef _DEBUG_  
    Serial.print("Opening SoftSD\\" + FileName + "...\n");
#endif
    if (Flag){                                      //Если произошло повторное подключение
      SoftSD.begin(SOFT_CS_PIN, SPI_QUARTER_SPEED); //внешнего SD то необходимо переподключить 
      Flag = false;                                 //внутренний SD, иначе файл обнуляется!
    }
      if (!SoftFile.open(FileName.c_str(), O_RDWR | O_APPEND | O_CREAT)){
      SFS = false;
#ifdef _DEBUG_
      cout <<"failed\n";
      cout << "3 SSDS = " << SSDS << "  SFS = " << SFS << "\n";
#endif
      Result += "Ошибка открытия SoftSD\\" + FileName + "\n";
    }
    else{
      SFS = true;
      if (SoftFile.fileSize() == 0) SoftFile.write(TABLE_HEADER);
#ifdef _DEBUG_
      cout << "opened\n";
      cout << "4 SSDS = " << SSDS << "  SFS = " << SFS << "\n";
#endif      
    }
  }

// Проверяем целостность внешнего и внутреннего файлов. Если один из файлов неоткрыт - пропускаем
//----------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG_
  Serial.print("Size of SoftSD\\" + FileName + " = "); cout << SoftFile.fileSize() << "\n";
  Serial.print("Size of HardSD\\" + FileName + " = "); cout << HardFile.size() << "\n";
#endif
  if (HFS && SFS) Result += CheckLogFiles(SoftFile, HardFile, FileName);

// Дописываем к двум логам строку с данными
//--------------------------------------------------------
  String s = GetDate() + ";" + GetTime() + ";" + StartWeight + ";" + EndWeight + ";";
  s +=  String((EndWeight - StartWeight)) + "\n";

  if (SFS){
#ifdef _DEBUG_
  Serial.print("Write to SoftSD\\" + FileName + " <-- " + s);
#endif
    SoftFile.write(s.c_str());
    SoftFile.close();
#ifdef _DEBUG_
    cout << "File clossed\n";
#endif
}

  if (HFS){
#ifdef _DEBUG_
  Serial.print("Write to HardSD\\" + FileName + " <-- " + s);
#endif
    HardFile.write(s.c_str());
    HardFile.close();    
#ifdef _DEBUG_
    cout << "File clossed\n";
#endif
}

#ifdef _DEBUG_
    cout << "SaveDataToSD done----------------------------------------\n";
#endif
  return Result;
}



//********************************************************  
//                  CheckLogFiles
//********************************************************  
String CheckLogFiles(SdFile &SF, File &HF, String FileName){

//  Проверяем равенство размеров файлов на HSD и SSD
//--------------------------------------------------------
  if (SF.fileSize() == HF.size()){
    cout << "Files have the same length. Restore is not necessary\n";
    return "Done";
  }
#ifdef _DEBUG_
  cout << "Files have different length. Restore is necessary\n";
#endif

//                     = Файлы не равны =
//  Сравниваем с EEPROM в часах.Выбираем один по мажоритарной
//  системе. Предполагаем что 2 источника одновременно не испортятся

//--------------------------------------------------------
//  Стираем файл на внешнем SD и создаем его заново (мажорство во 2 версии)
  if (!HF){
    HFS = false;
    return "Ошибка чтения HardSD\\" + FileName + " при синхронизации логов";
  }
#ifdef _DEBUG_
  Serial.println("Closing HardSD\\" + FileName);
#endif
  HF.close();
  
#ifdef _DEBUG_
  Serial.println("Deleting HardSD\\" + FileName);
#endif
  HardSD.remove(FileName.c_str());

#ifdef _DEBUG_
  Serial.println("Opening HardSD\\" + FileName);
#endif
  HF = HardSD.open(FileName, FILE_WRITE);  
  if (!HF) {
    HFS = false;
#ifdef _DEBUG_
    cout << "5 HSDS = " << HSDS << "  HFS = " << HFS << "\n";
    Serial.println("HardSD.open(" + FileName + ") after delete failed");
#endif
    return "Ошибка открытия HardSD\\" + FileName + " при синхронизации логов";
  }   

//  Проверяем свободное место на HSD
//--------------------------------------------------------

//  Копируем файл из SSD в HSD
//--------------------------------------------------------
#ifdef _DEBUG_ 
  Serial.print("Copying SoftSD\\" + FileName + " to HardSD\\" + FileName + "...");  
#endif
  SF.seekSet(0);                                // Копируем с начала файла
  while (SF.available()) HF.write(SF.read());
  
#ifdef _DEBUG_
  cout << "done\n";
#endif
  return "Done";
}

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

#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS
