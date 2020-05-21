//#define _DEBUG_
//#define _LOG_SAVE_
#define _NIGHT_SAVE_SD_
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <stdio.h>
#include <Wire.h>
#include <HX711.h>
#include <DS3231.h>
#include <SPI.h>
#include "SdFat.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
#include <Eeprom24C32_64.h>
#include "EepromConfig.h"

#include "RTC.h"
#include </Users/andrey/Рабочая/Сафоновский ЖБИ/Проект Arduino/MyLibraries/Definitions.h>

#if defined(_GFXFONT_H_)           //are we using the new library?
#define ADJ_BASELINE 11            //new fonts setCursor to bottom of letter
#else
#define ADJ_BASELINE 0             //legacy setCursor to top of letter
#endif

#define TABLE_HEADER      "Date;Time;Start;End;Sum\n" //Заголовок таблицы excel

Time t;

struct TFTBox{
  uint16_t x;  
  uint16_t y;
  uint16_t w;
  uint16_t h;
  uint16_t color;
  uint16_t fontColor;
  GFXfont  *font;
  uint8_t  FontSize;
};

long W[PRIVET_DATA_COUNT];

struct PrivatData{
  uint8_t  Size = PRIVET_DATA_COUNT;
  uint8_t  CheckSumm = 0;
  long ScaleTare = 0;
  long ScaleRate = 0;
  long LastMonth = 0;
  long LastWeek = 0;
  long LastDay = 0;
  long DayWeight = 0;
  long WeekWeight = 0;
  long MonthWeight = 0;
};
PrivatData PD;

class TFTWriteBox{
public:
  TFTBox tb;
  TFTWriteBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t fontColor, GFXfont  *font, uint8_t  FontSize);
};

struct DateTime{String Year, Year2, Mon, Day, Hour, Min, Sec;};
DateTime dt;

    
//**************************************************************************************


//**************************************************************************************
//Объекты
HX711 scale;                                                      // Преобразователь тензодатчика
DS3231 DsRtc(SDA, SCL);                                           // Часы DS3231
SysT rtc(&DsRtc);                                                 // Одновременная работа rtc и millis().

//Adafruit_AM2320 am2320 = Adafruit_AM2320();                       // Датчик температуры - влажности

// software SPI
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SoftSD;  // SD на плате монитора
SdFile SoftFile;
bool SSDS = true; // статус SSD. false - дисковод недоступен
bool SFS = false; // статус открытия файла на внутреннем диске

// hardware SPI
SdFat HardSD;                                                     // SD отдельной платой
File HardFile;
bool HSDS = true; // статус HSD. false - дисковод недоступен
bool HFS = false; // статус открытия файла на внешнем диске

static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);

ArduinoOutStream cout(Serial);   // serial output stream
MCUFRIEND_kbv tft;


//**************************************************************************************
// Переменные
String Text, OldText; //для оптимизации скорости вывода крупного текста
long  Weight3=0, Weight10=0; //DaySummWeight = 0, WeekSummWeight=0, MonthSummWeight=0;
long  Weight, EndWeight, DiffWeight, SampleWeight;
uint32_t TimeS, TimeD;
uint16_t ID;
String sResult;
float Temperature, Humidity;
long sw[10];
uint8_t CurrSec = 0;
String OldTime, NewTime;
String TmpFileName;
byte DataByte;

//защищаемая информация
long MeasureStartTime;
long StartWeight;
//состояния
bool ButtonStatusTare = true;
bool ButtonStatusScale = true;
bool ButtonPneumo = false;
bool PneumoStatus = false;
bool SdEjectButton = false;
bool SdEjectButtonStatus;
bool ThermoStabil = false;
bool AutoLoad = false;
bool MaxWeightStatus = false;
bool AutoLoadStatus = false;
bool WriteWaiting = false;
bool NeedToWrite = false;
bool AutoMode = false;
bool Start = true;


 

//**************************************************************************************
// Объявления функций из других файлов
//**************************************************************************************
template <typename T>
extern String CorrectDisplay(T Number, byte Quantity, String Filler);
extern bool SavePrivetToSD(PrivatData *pd);
extern bool ReadPrivetFromSD(PrivatData *pd);
extern String SaveDataToSD(int StartWeight, int EndWeight, int SumWeight);
extern String GetDate();
extern String GetTime();
extern String GetShortDate();
extern void DisplayWeight(String sw);
extern void DrawGreed();
extern void DrawMask();
extern void DisplayData(String ds, uint8_t mode);
extern void DisplayMessage(String msg, uint8_t mode);
extern void DisplayAlert (String alt, uint8_t mode);
extern int16_t GetNumber(int16_t Num, uint8_t bUp, uint8_t bDown);
extern bool SaveLogToSD(const char* FileName, const char* Message);
extern bool DeleteFileFromSD(const char* FileName);
extern bool SaveLogToSoftSD(const char* FileName, const char* Message);
extern bool DeleteFileFromSoftSD(const char* FileName);
extern String GetTmpFileName();
extern void WritePrivetToEEPROM(PrivatData *pd);
extern bool ReadPrivetFromEEPROM(PrivatData *pd);
extern void ShakeSD(uint8_t Shake_Pin);
template <typename T>
extern void WriteVarToEEPROM(T Var, word Addres);
template <typename T>
extern void ReadVarFromEEPROM(T* Var, word Addres);
extern void ClearSummWeights();
extern void SetCurrentTime();
extern void Debug();


//**************************************************************************************
// Инициализация структуры PrivatData
//--------------------------------------------------------------------------------------
void Init(PrivatData* pd){            
  t = rtc.getTime(); 
  pd->Size = PRIVET_DATA_COUNT;
  pd->CheckSumm = 0;
  pd->ScaleTare = SCALE_TARE_INIT;
  pd->ScaleRate = SCALE_RATE_INIT;
  pd->LastMonth = t.mon;
  pd->LastWeek = t.dow;
  pd->LastDay = t.date;
  pd->DayWeight = 0;
  pd->WeekWeight = 0;
  pd->MonthWeight = 0;
}
//**************************************************************************************
// Расчет контрольной суммы CheckSumm()
//--------------------------------------------------------------------------------------
byte CheckSumm(PrivatData* pd){            
//  Serial.println("----------------------------CheckSumm --- ");
  byte     Sum = 0b01010101; //85 или 0x55
  uint32_t Mask = 0xFF;       // маска на младший байт
  for (int i=0; i < sizeof(pd->ScaleTare); i++)  Sum ^= (pd->ScaleTare >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->Size); i++)       Sum ^= (pd->Size >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->ScaleTare); i++)  Sum ^= (pd->ScaleTare >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->ScaleRate); i++)  Sum ^= (pd->ScaleRate >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->LastMonth); i++)  Sum ^= (pd->LastMonth >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->LastDay); i++)    Sum ^= (pd->LastDay >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->LastWeek); i++)   Sum ^= (pd->LastWeek >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->DayWeight); i++)  Sum ^= (pd->DayWeight >> i*8) & Mask;
  for (int i=0; i < sizeof(pd->WeekWeight); i++) Sum ^= (pd->MonthWeight >> i*8) & Mask;
  return Sum;
//  Serial.print("CheckSumm = "); Serial.println(Sum);
}

//**************************************************************************************
// Сохранение веса и отрисовка
//--------------------------------------------------------------------------------------
void WeightSaveAndDisplay(){
  bool SdEjectButtonstatus;
  SaveLogToSD("Debug.log", "WeightSaveAndDisplay-SaveDataToSD() ----");
  EndWeight = (Weight3-PD.ScaleTare)/PD.ScaleRate;
  DiffWeight = EndWeight - StartWeight;
  
#ifdef _NIGHT_SAVE_SD_
  SdEjectButtonStatus = SdEjectButton;
  SdEjectButton = true;
#endif
  sResult = SaveDataToSD(StartWeight, EndWeight, DiffWeight) + "\n";
  
#ifdef _NIGHT_SAVE_SD_
  SdEjectButton = SdEjectButtonStatus;
#endif
  
  SaveLogToSD("Debug.log", sResult.c_str());
  cout << "----------------------------Data to SD saved\n";
  
                  // Суммируем дневной, недельный и месячный расходы
  SaveLogToSD("Debug.log", "WeightSaveAndDisplay-WritePrivetToEEPROM()\n");
  PD.DayWeight += DiffWeight;
  PD.WeekWeight += DiffWeight;
  PD.MonthWeight += DiffWeight;
  WritePrivetToEEPROM(&PD);
  cout << "----------------------------Summry weights saved\n";
  
  SaveLogToSD("Debug.log", "WeightSaveAndDisplay-DisplayData\n");
  DisplayData(CorrectDisplay(PD.DayWeight, 5,""), DAY_WEIGHT);
  DisplayData(CorrectDisplay(PD.WeekWeight, 6,""), WEEK_WEIGHT);
  DisplayData(CorrectDisplay(PD.MonthWeight, 7,""), MONTH_WEIGHT);   
}

#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
  void watchdogSetup(void) {
}
#endif

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
// Setup()
//--------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial); //yield()
cout << "-----START------\n";
 
  SaveLogToSD("Debug.log", "---Start---\n");
  eeprom.initialize();                                  // EEPROM

  //Раскомментировать для тестирования записи на EEPROM
//  Debug();
//  while(1);

  //Раскомментировать для сброса суммарных весов
//  ClearSummWeights();
//  while(1);

  DsRtc.begin();
  //Раскомментировать для установки часов
//  SetCurrentTime();
//  while(1);
  
  rtc.begin();
  NewTime = GetTime();
  
//  am2320.begin();                                         //термометр/гигрометр
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 64);   //Тензодатчик

  //TFT
  tft.reset();
  ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
cout << "---------tft----------\n";
 
//  // Софтверный SD
//  if (!SoftSD.begin(SOFT_CS_PIN, SPI_SPEED)){
//    SSDS = false;
//    cout << "Не стартовал SSD\n";
//  }
  
  //Кнопки
  pinMode(SD_EJECT_BUTTON_PIN, INPUT);
  pinMode(TARE_BUTTON_PIN, INPUT);
  pinMode(SCALE_BUTTON_PIN, INPUT);
  pinMode(PNEUMO_SUPPLY_PIN, INPUT);
  pinMode(WATCH_PIN_OUT, OUTPUT);
//  pinMode(WATCH_PIN_IN, INPUT);
    
//  pinMode(THERMO_STABIL_PIN, OUTPUT);
  pinMode(     BLOCK_PNEUMO_PIN, OUTPUT);
  digitalWrite(BLOCK_PNEUMO_PIN, RELAY_OFF);
  pinMode(     VALVE_LOCK_PIN, OUTPUT);
  digitalWrite(VALVE_LOCK_PIN, RELAY_OFF);
  pinMode(     PN_BUTON_PRESS_PIN, OUTPUT);
  digitalWrite(PN_BUTON_PRESS_PIN, RELAY_OFF);
  pinMode(     HSD_POWER_PIN, OUTPUT);
  digitalWrite(HSD_POWER_PIN, RELAY_OFF);
  
  SdEjectButton = !digitalRead(SD_EJECT_BUTTON_PIN);
  
  //Читаем служебный файл
  if(!ReadPrivetFromEEPROM(&PD)){
    cout << "Не удалось прочитать файл pvData01.dav\nИнициируем по умолчению\n";
    Init(&PD);  // Если не смогли прочитать инициируем по умолчению
  }

  //Рисуем экран
  Weight = PD.ScaleTare; Weight3 = PD.ScaleTare; Weight10 = PD.ScaleTare;
  DrawMask();
  DisplayData(CorrectDisplay(PD.DayWeight, 5,""), DAY_WEIGHT);
  DisplayData(CorrectDisplay(PD.WeekWeight, 6,""), WEEK_WEIGHT);
  DisplayData(CorrectDisplay(PD.MonthWeight, 7,""), MONTH_WEIGHT);  
  DisplayData(GetDate(), DATE_STRING);
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
  watchdogEnable(5000);                                   //запуск watchdog
#endif
}


long t1, t2 = 0;
 
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void loop() {
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
  watchdogReset();
#endif

//-----------------------------Если первый цикл после загрузки-------------------
  if (Start){     //инициируем массив измерений веса для правильного отображения в 1-ю секунду
    sw[0] = scale.read();
    for(int i=1; i<10; i++) sw[i] = sw[0];
    Start = false;

// ******** НЕОБХОДИМО ПЕРЕДЕЛАТЬ ДЛЯ АВТОПНЕВМОПОДАЧИ **********
// Проверяем необходимость восстановления переменных состояния.
// Критерий - сколько времени прошло от начала пневмоподачи. В другое время не нужно.
// Считаем, что перезагрузка по watchdog пройдет за время меньшее,
// чем цикл пневмоподачи - 8с + 2с на успокоение бункера.
// Если время больше, то это ручная перезагрузка, при которой
// состояния не воccтанавливаются
/*                                          Пока отключено до полного теста работы
    t = rtc.getTime();
    ReadVarFromEEPROM(&MeasureStartTime, _MeasureStartTime); //время начала последней пневмоподачи
    if ((rtc.getUnixTime(t) - MeasureStartTime) <= 10){      //если менее 10с назад - восстанавливаем
      ReadVarFromEEPROM( &PneumoStatus,    _PneumoStatus);
      ReadVarFromEEPROM( &AutoLoad,        _AutoLoad);
      ReadVarFromEEPROM( &MaxWeightStatus, _MaxWeightStatus);
      ReadVarFromEEPROM( &AutoLoadStatus,  _AutoLoadStatus);
      ReadVarFromEEPROM( &WriteWaiting,    _WriteWaiting);
      ReadVarFromEEPROM( &StartWeight,     _StartWeight);
      ReadVarFromEEPROM( &NeedToWrite,     _NeedToWrite);
*/
  }

//--------------------------------Считываем и усредняем вес ----------------------  
//  SaveLogToSD("Debug.log", "Считываем и усредняем вес\n");
  
  for(int i=1; i<10; i++) sw[i] = sw[i-1]; 
  sw[0] = scale.read();

#ifdef _DEBUG_ //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Подменяем считанный вес
sw[0] = SCALE_TARE_INIT + (abs(t1 - 100)+random(0, 20))*SCALE_RATE_INIT;
#endif //_DEBUG_ //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  
  Weight = sw[0];
  Weight3 = (sw[0] + sw[1] + sw[2])/3;
  Weight10 = (sw[0] + sw[1] + sw[2] + sw[3] + sw[4] + sw[5] + sw[6] + sw[7] + sw[8] + sw[9])/10;

  
//--------------------------------Нажата кнопка "Тарирование" ----------------------

  if (!digitalRead(TARE_BUTTON_PIN) && !ButtonStatusTare){
  SaveLogToSD("Debug.log", "Нажата кнопка Тарирование\n");

    PD.ScaleTare = Weight10;
    cout << "----------------------------ButtonTare pressed\n";
    WritePrivetToEEPROM(&PD);
    Serial.print("New Tare = "); Serial.println(PD.ScaleTare);
  }
  ButtonStatusTare = !digitalRead(TARE_BUTTON_PIN);
  
//----------------------------- Нажата кнопка "Масштабирование" ------------------

  if (!digitalRead(SCALE_BUTTON_PIN) && !ButtonStatusScale){
  SaveLogToSD("Debug.log", "Нажата кнопка Масштабирование\n");

    cout << "----------------------------ButtonRate pressed\n";
    //Вводим измеренный вес
    SampleWeight = GetNumber(Text.toInt(), TARE_BUTTON_PIN, SCALE_BUTTON_PIN);
    //Расчитываем коэффициент
    PD.ScaleRate = (Weight10 - PD.ScaleTare)/SampleWeight;             
    if (PD.ScaleRate != 0){
      Serial.print("New Rate = "); Serial.println(PD.ScaleRate);
//      WritePrivetToEEPROM(&PD);
    }
  }
  ButtonStatusScale = !digitalRead(SCALE_BUTTON_PIN);

//------------------------------------ Пневмоподача  ------------------------
//  SaveLogToSD("Debug.log", "Пневмоподача\n");
  
  ButtonPneumo = digitalRead(PNEUMO_SUPPLY_PIN);     //инвертированное значение (включена - 0)

#ifdef _DEBUG_  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Управляем пневмоподачей
t1 < 100 ? ButtonPneumo = 1 : ButtonPneumo = 0;
if (t1 > 200) t1 = 0; t1++;
#endif //_DEBUG_ //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  
  if(AutoMode){
    if(AutoLoad and ButtonPneumo){                   //если идет автозагрузка и нет пневмоподачи
      digitalWrite(PN_BUTON_PRESS_PIN, RELAY_ON);    //включаем реле автопневмоподачи
    }
  }
  

  if (!PneumoStatus and !ButtonPneumo){ //------------начало пневмоподачи-----------------
  SaveLogToSD("Debug.log", "начало пневмоподачи\n");
  
    cout << "----------------------------ButtonPneumo pressed\n";
    PneumoStatus = true;                            //режим пневмоподачи включен
//                                WriteVarToEEPROM(PneumoStatus, _PneumoStatus);
    digitalWrite(VALVE_LOCK_PIN, RELAY_ON);         //включаем замок на затворе
    
//запоминаем время начала пневмоподачи здесь, так как и в режиме авто и в ручном режиме
//мы должны контролировать каждую пневмоподачу
//    MeasureStartTime = rtc.getUnixTime(rtc.getTime());
//                                WriteVarToEEPROM(MeasureStartTime, _MeasureStartTime);
    
    if (AutoMode){
      digitalWrite(PN_BUTON_PRESS_PIN, RELAY_OFF);    //отпускаем реле автопневмоподачи (иначе глючит их реле)
      DisplayAlert(" АВТО", MSG_WARNING);
      AutoLoad = true;          
//                                WriteVarToEEPROM(AutoLoad, _AutoLoad);
      AutoLoadStatus = true;    
//                                WriteVarToEEPROM(AutoLoadStatus, _AutoLoadStatus);
      if (!WriteWaiting){  //если не ждем окончания записи на SD в авторежиме, то запоминаем нач. вес 
        StartWeight = (Weight3-PD.ScaleTare)/PD.ScaleRate;
//                                WriteVarToEEPROM(StartWeight, _StartWeight);
        WriteWaiting = true;    
//                                WriteVarToEEPROM(WriteWaiting, _WriteWaiting);
      } 
    }
    
    else{ // AutoMode
      DisplayAlert(" Пневмо", MSG_NOTICE);      
      StartWeight = (Weight3-PD.ScaleTare)/PD.ScaleRate;
//                                WriteVarToEEPROM(StartWeight, _StartWeight);
      WriteWaiting = true;    
//                                WriteVarToEEPROM(WriteWaiting, _WriteWaiting);
    }
  }                                     //------------начало пневмоподачи
  
 
  if (PneumoStatus and ButtonPneumo){   //-------------конец пневмоподачи------------------
  SaveLogToSD("Debug.log", "конец пневмоподачи\n");

    cout << "----------------------------ButtonPneumo released\n";
    PneumoStatus = false;
    if (AutoMode){
      if (AutoLoad) digitalWrite(PN_BUTON_PRESS_PIN, RELAY_ON); //новая пневмоподача в режиме авто
    }
    else{
//      DisplayAlert("", MSG_NOTICE);
      if(WriteWaiting){
        NeedToWrite = true;
//                                WriteVarToEEPROM(NeedToWrite, _NeedToWrite);
        WriteWaiting = false;      
//                                WriteVarToEEPROM(WriteWaiting, _WriteWaiting);     
      }
    }
  }                                      //-------------конец пневмоподачи 

 
//------------------------------------ Запись веса после автозагрузки ------------------------

  if (NeedToWrite){ //устанавливается для ручного режима по окончании каждой пневмоподачи, 
                    //для авторежима при превышении максимального веса
    if (t2 == 0) t2 = millis();                                 //пауза для успокоения бункера на
    if ((millis() - t2) > SCALE_DELAY_TIME) {                   //SCALE_DELAY_TIME время
      t2 = 0;
  SaveLogToSD("Debug.log", "Запись веса после автозагрузки\n");
  
      AutoLoadStatus = false;   
//                                WriteVarToEEPROM(AutoLoadStatus, _AutoLoadStatus);
      DisplayAlert("", MSG_WARNING);
      digitalWrite(VALVE_LOCK_PIN, RELAY_OFF);                  //выключаем замок на затворе

      WeightSaveAndDisplay();
      
      NeedToWrite = false;      
//                                WriteVarToEEPROM(NeedToWrite, _NeedToWrite); 
    }
  }

//-------------------------------------- Извлечение SD -----------------------
    if(!SdEjectButton && !digitalRead(SD_EJECT_BUTTON_PIN)){
    cout << "----------------------------ButtonEject pressed\n";

#ifdef _NIGHT_SAVE_SD_
      SaveDataToSD(0, 0, 0);                          //синхронизируем внешний SD
#endif

      DisplayMessage("SD карточку можно извлечь.\nНе отпускать кнопку без карточки!", MSG_WARNING);
      SdEjectButton = !SdEjectButton; 
    }
    if(SdEjectButton && digitalRead(SD_EJECT_BUTTON_PIN)){
    cout << "----------------------------ButtonEject released\n";
      DisplayMessage("", MSG_NOTICE);
      SdEjectButton = !SdEjectButton; 
    }

//--------------------------------- Вывод даты на дисплей ----------------
//  SaveLogToSD("Debug.log", "Вывод даты  и времени на дисплей\n");

  if (int(millis()/10000) - TimeD) { 
    TimeD = millis()/10000;
    DisplayData(GetDate(), DATE_STRING); 
  }

//--------------------------------- Вывод времени на дисплей ----------------
  NewTime = GetTime();
  if(OldTime != NewTime){
    DisplayData(NewTime, TIME_STRING);
    OldTime = NewTime;
//    printf("-------Time %s\n", OldTime.c_str());
  }
  
//--------------------------------------------------------------------------
//----------------------------------- Цикл отображение веса ----------------

  if (int(millis()/500) - TimeS) { //период 
    TimeS = millis()/500;
    t = rtc.getTime();
    
    if (rtc.getTimeAfterLastRead() > 14400){          //4 часа никто не работает
      if ((t.hour > 2) & (t.hour < 6)){               //и глубокая ночь
        if (!rtc.getDS3231err()) rtc.syncTime();          //если RTC работают, синхронизируем время
    
#ifdef _NIGHT_SAVE_SD_
        SaveDataToSD(0, 0, 0);                        //синхронизируем внешний SD
#endif
      }
    }


    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));
//    Serial.println("pin 25 = " + (String)digitalRead(WATCH_PIN_OUT));  
//    Serial.println("PN_BUTON_PRESS_PIN = " + (String)digitalRead(PN_BUTON_PRESS_PIN));

//--------------------------------- Термостабилизация ----------------
//    if (am2320.readTemperature() > 36) digitalWrite(THERMO_STABIL_PIN, RELAY_OFF);
//    if (am2320.readTemperature() < 34) digitalWrite(THERMO_STABIL_PIN, RELAY_ON);
    
//--------------------------------------- Вес ------------------------------
    Text = CorrectDisplay((Weight3-PD.ScaleTare)/PD.ScaleRate, 3, " ");
    DisplayWeight(Text);
//------------------------------------ Температуры ------------------------------
//    DisplayData(CorrectDisplay(am2320.readTemperature(), 5,""), THEMP_STAB);
//    DisplayData(CorrectDisplay(DsRtc.getTemp(), 5,""), THEMP_ROOM);
 
//---------------------- Обновляем суммарные веса за ... --------------
    if (t.date != PD.LastDay){
//------------------------------------ Новый день -------------------------------
      PD.LastDay = t.date;
      PD.DayWeight = 0;
      cout << "----------------------------New day\n";
      WritePrivetToEEPROM(&PD);
      DisplayData(CorrectDisplay(PD.DayWeight, 5,""), DAY_WEIGHT);
    }
    if (t.dow != PD.LastWeek){
//----------------------------------- Новая неделя ------------------------------
      PD.LastWeek = t.dow;
      if (PD.LastWeek == 1){
        PD.WeekWeight = 0;
        cout << "----------------------------New week\n";
        WritePrivetToEEPROM(&PD);
        DisplayData(CorrectDisplay(PD.WeekWeight, 6,""), WEEK_WEIGHT);
      }
    }
    if (t.mon != PD.LastMonth){
//------------------------------------ Новый месяц ------------------------------
      PD.LastMonth = t.mon;
      PD.MonthWeight = 0;
      cout << "----------------------------New month\n";
      WritePrivetToEEPROM(&PD);
      DisplayData(CorrectDisplay(PD.MonthWeight, 7,""), MONTH_WEIGHT);    
    }
    
//--------------------- Вес цемента в бункере больше допустимого ----------------
    if ((Weight10-PD.ScaleTare)/PD.ScaleRate > MAX_LOAD_WEIGHT){
      cout << "---------------------------Max weight\n";
      MaxWeightStatus = true;
//                                 WriteVarToEEPROM(MaxWeightStatus, _MaxWeightStatus);
      AutoLoad = false;                               //выключаем автоподачу
//                                 WriteVarToEEPROM(AutoLoad, _AutoLoad);
      if(WriteWaiting){
        NeedToWrite = true;
//                                 WriteVarToEEPROM(NeedToWrite, _NeedToWrite);     
        WriteWaiting = false;
//                                 WriteVarToEEPROM(WriteWaiting, _WriteWaiting);     
      }

      digitalWrite(BLOCK_PNEUMO_PIN, RELAY_ON);       //включаем запрет пневмоподачи
      digitalWrite(PN_BUTON_PRESS_PIN, RELAY_OFF);    //отпускаем кнопку пневмоподачи    
      DisplayAlert(" >" + (String)MAX_LOAD_WEIGHT + "кг", MSG_ERROR);
    }
    else{
      if(MaxWeightStatus){
        DisplayAlert("", MSG_ERROR);
        digitalWrite(BLOCK_PNEUMO_PIN, RELAY_OFF);    //выключаем запрет пневмоподачи
        MaxWeightStatus = false;
//                                 WriteVarToEEPROM(MaxWeightStatus, _MaxWeightStatus);
      }
    }
  }
}

/*

//    if (am2320.readTemperature() > 36) ThermoStabil = true;
//    if (am2320.readTemperature() < 34) ThermoStabil = false;
    cout <<"---------------------------------------------------------------\n";
//    Serial.print("Hum: "); Serial.println(am2320.readHumidity());

Serial.print("Weight3      =  "); Serial.println(Weight3);
Serial.print("Temp. out    =  "); Serial.println(rtc.getTemp());
Serial.print("Temp. stab   =  "); Serial.println(am2320.readTemperature());
Serial.print("Humidity     =  "); Serial.println(am2320.readHumidity());
Serial.print("ThermoStabil =  "); Serial.println(ThermoStabil);

*/
/*
Serial.print("Weight = "); Serial.println(Weight);
Serial.print("scale.read_average(3) = "); Serial.println(scale.read_average(3));
Serial.print("Weight3 = "); Serial.println(Weight3);
Serial.print("PD.ScaleTare = "); Serial.println(PD.ScaleTare);
Serial.print("PD.ScaleRate = "); Serial.println(PD.ScaleRate);
Serial.print("(Weight3-PD.ScaleTare)/PD.ScaleRate = "); Serial.println((Weight3-PD.ScaleTare)/PD.ScaleRate);
Serial.print("Расчитанный вес = "); Serial.println(Text);
*/


/*  
    PD.Size = PRIVET_DDATA_COUNT;
    PD.CheckSumm = 0;
    PD.ScaleTare = 2;
    PD.ScaleRate = 3;
    PD.LastMonth = 4;
    PD.LastWeek = 5;
    PD.LastDay = 6;
    PD.DayWeight   = 7;
    PD.WeekWeight  = 8;
    PD.MonthWeight = 9;
     
    SavePrivetToSD(&PD);
 //   ReadPrivetFromSD(&PD);
    
    Serial.print("Size  "); Serial.println(PD.Size);
    Serial.print("CheckSumm  "); Serial.println(PD.CheckSumm);
    Serial.print("ScaleTare  "); Serial.println(PD.ScaleTare);
    Serial.print("ScaleRate  "); Serial.println(PD.ScaleRate);
    Serial.print("LastMonth  "); Serial.println(PD.LastMonth);
    Serial.print("LastWeek  "); Serial.println(PD.LastWeek);
    Serial.print("LastDay  "); Serial.println(PD.LastDay);
    Serial.print("DayWeight  "); Serial.println(PD.DayWeight);
    Serial.print("WeekWeight  "); Serial.println(PD.WeekWeight);
    Serial.print("MonthWeight  "); Serial.println(PD.MonthWeight);

    while(1){}
*/
/*
  t = rtc.getTime();
  Serial.print("hour = ");  Serial.println(t.hour);
  Serial.print("min  = ");  Serial.println(t.min);
  Serial.print("sec  = ");  Serial.println(t.sec);
  Serial.print("date = ");  Serial.println(t.date);
  Serial.print("mon  = ");  Serial.println(t.mon);
  Serial.print("year = ");  Serial.println(t.year);
  Serial.print("dow  = ");  Serial.println(t.dow);
  */
