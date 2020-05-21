
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
#include "Definitions.h"
#include <Eeprom24C32_64.h>

#if defined(_GFXFONT_H_)           //are we using the new library?
#define ADJ_BASELINE 11            //new fonts setCursor to bottom of letter
#else
#define ADJ_BASELINE 0             //legacy setCursor to top of letter
#endif

//#define _STATISTIC_              // Включается режим накопления на SD отладочной информации

#ifndef _STATISTIC_
  #define TABLE_HEADER      "Date;Time;Start;End;Sum\n" //Заголовок таблицы excel
#else
  #define TABLE_HEADER      "Date;Time;Themp;Humin;Weight\n" //Заголовок таблицы excel
#endif


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
DS3231  rtc(SDA, SCL);                                            // Часы
Adafruit_AM2320 am2320 = Adafruit_AM2320();                       // Датчик температуры - влажности

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
long  Weight, StartWeight, EndWeight, DiffWeight, SampleWeight;
bool  ButtonStatusTare = true;
bool  ButtonStatusScale = true;
bool  ButtonPneumo = false;
bool  PneumoStatus = false;
bool  SdEjectButton = false;
bool  ThermoStabil = false;
bool  AutoLoad = false;
uint32_t TimeS, TimeD;
uint16_t ID;
String sResult;
float Temperature, Humidity;
long sw[10];
uint8_t CurrSec = 0;
String OldTime, NewTime;
String TmpFileName;
bool MaxWeightStatus = false;


 

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
extern void SavePrivetToEEPROM(PrivatData *pd);
extern bool ReadPrivetFromEEPROM(PrivatData *pd);
extern void ShakeSD(uint8_t Shake_Pin);

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

void watchdogSetup(void) {
  }


//**************************************************************************************
// Setup()
//--------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial) yield();

  eeprom.initialize();
  
  am2320.begin();                                         //термометр/гигрометр
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 64);   //Тензодатчик
  rtc.begin();                                            //Часы

  //TFT
  tft.reset();
  ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);

 
  // Софтверный SD
  if (!SoftSD.begin(SOFT_CS_PIN, SPI_SPEED/*SPI_EIGHTH_SPEED*/)){
    SSDS = false;
    cout << "Не стартовал SSD\n";
  }
  
  //Кнопки
  pinMode(SD_EJECT_BUTTON_PIN, INPUT);
  pinMode(TARE_BUTTON_PIN, INPUT);
  pinMode(SCALE_BUTTON_PIN, INPUT);
  pinMode(PNEUMO_SUPPLY_PIN, INPUT);
  pinMode(THERMO_STABIL_PIN, OUTPUT);
  pinMode(BLOCK_PNEUMO_PIN, OUTPUT);
  pinMode(VALVE_LOCK_PIN, OUTPUT);
  digitalWrite(BLOCK_PNEUMO_PIN, true);
  digitalWrite(VALVE_LOCK_PIN, true);

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

  watchdogEnable(4000);                                   //запуск watchdog  
}


long t1, t2 = 0;
 
//**************************************************************************************
void loop() {
  watchdogReset();

//--------------------------------Считываем и усредняем вес ----------------------  
  for(int i=1; i<10; i++) sw[i] = sw[i-1]; 
  sw[0] = scale.read();

/*
// Отладка.Хулиганим здесь. Задаем вес
sw[0] = SCALE_TARE_INIT + (abs(t1 - 100)+random(0, 20))*SCALE_RATE_INIT;
*/
  
  Weight = sw[0];
  Weight3 = (sw[0] + sw[1] + sw[2])/3;
  Weight10 = (sw[0] + sw[1] + sw[2] + sw[3] + sw[4] + sw[5] + sw[6] + sw[7] + sw[8] + sw[9])/10;
  
//--------------------------------Нажата кнопка "Тарирование" ----------------------
  if (!digitalRead(TARE_BUTTON_PIN) && !ButtonStatusTare){
    PD.ScaleTare = Weight10;
    cout << "----------------------------ButtonTare pressed\n";
    SavePrivetToEEPROM(&PD);
    Serial.print("New Tare = "); Serial.println(PD.ScaleTare);
  }
  ButtonStatusTare = !digitalRead(TARE_BUTTON_PIN);
  
//----------------------------- Нажата кнопка "Масштабирование" ------------------
  if (!digitalRead(SCALE_BUTTON_PIN) && !ButtonStatusScale){
    cout << "----------------------------ButtonRate pressed\n";
    //Вводим измеренный вес
    SampleWeight = GetNumber(Text.toInt(), TARE_BUTTON_PIN, SCALE_BUTTON_PIN);
    //Расчитываем коэффициент
    PD.ScaleRate = (Weight10 - PD.ScaleTare)/SampleWeight;             
    if (PD.ScaleRate != 0){
      Serial.print("New Rate = "); Serial.println(PD.ScaleRate);
      SavePrivetToEEPROM(&PD);
    }
  }
  ButtonStatusScale = !digitalRead(SCALE_BUTTON_PIN);

  
//------------------------------------ Пневмозагрузка SD ------------------------
  ButtonPneumo = digitalRead(PNEUMO_SUPPLY_PIN);   //инвертированное значение (включена - 0)
  if(AutoLoad and ButtonPneumo){                   //если авторежим и нет пневмоподачи
    digitalWrite(PN_BUTON_PRESS_PIN, RELAY_ON);    //включаем реле автопневмоподачи
  }
/*
// Отладка.Хулиганим здесь. Включаем пневмоподачу
if (t1 < 100) ButtonPneumo = 1;
else ButtonPneumo = 0;
if (t1 > 200) t1 = 0;
t1++;*/

  if (!PneumoStatus and !ButtonPneumo){             //момент нажатия пневмоподачи
    cout << "----------------------------ButtonPneumo pressed\n";
    PneumoStatus = true;                            //режим пневмозагрузки включен
    AutoLoad = true;                                //включаем или подтверждаем режим автозагрузки
    digitalWrite(VALVE_LOCK_PIN, RELAY_ON);         //включаем замок на затворе
    digitalWrite(PN_BUTON_PRESS_PIN, RELAY_OFF);    //отпускаем реле автопневмоподачи (иначе глючит их реле)
    DisplayAlert(" Пневмо", MSG_NOTICE);
    DisplayAlert(" АВТО", MSG_WARNING);
    StartWeight = (Weight3-PD.ScaleTare)/PD.ScaleRate;
    cout << "Button pressed. AutoLoad = " << AutoLoad << endl;
  }
  if (PneumoStatus and ButtonPneumo){
    cout << "----------------------------ButtonPneumo released\n";
    if (t2 == 0) t2 = millis();                                 //пауза для успокоения бункера
    if ((millis() - t2) > SCALE_DELAY_TIME) {                   //
      t2 = 0;
      if (AutoLoad) digitalWrite(PN_BUTON_PRESS_PIN, RELAY_ON); //новая пневмоподача в режиме авто
      digitalWrite(VALVE_LOCK_PIN, RELAY_OFF);                  //выключаем замок на затворе

       cout << "Button released. AutoLoad = " << AutoLoad << endl;
       cout << "PN_BUTON_PRESS_PIN        = " << digitalRead(PN_BUTON_PRESS_PIN) << endl;

      PneumoStatus = false;
      DisplayAlert("", MSG_NOTICE);
      
      EndWeight = (Weight3-PD.ScaleTare)/PD.ScaleRate;
      DiffWeight = EndWeight - StartWeight;     
      sResult = SaveDataToSD(StartWeight, EndWeight, DiffWeight);
      cout << "----------------------------Data to SD saved\n";
      
                      // Суммируем дневной, недельный и месячный расходы
      PD.DayWeight += DiffWeight;
      PD.WeekWeight += DiffWeight;
      PD.MonthWeight += DiffWeight;
      SavePrivetToEEPROM(&PD);
      cout << "----------------------------Summry weights saved\n";
      DisplayData(CorrectDisplay(PD.DayWeight, 5,""), DAY_WEIGHT);
      DisplayData(CorrectDisplay(PD.WeekWeight, 6,""), WEEK_WEIGHT);
      DisplayData(CorrectDisplay(PD.MonthWeight, 7,""), MONTH_WEIGHT);
    }
  }

//-------------------------------------- Извлечение SD -----------------------
    if(!SdEjectButton && !digitalRead(SD_EJECT_BUTTON_PIN)){
    cout << "----------------------------ButtonEject pressed\n";
      DisplayMessage("SD карточку можно извлечь.\nНе отпускать кнопку без карточки!", MSG_WARNING);
    }
    if(SdEjectButton && digitalRead(SD_EJECT_BUTTON_PIN)){
    cout << "----------------------------ButtonEject released\n";
      DisplayMessage("", MSG_NOTICE);
    }
    SdEjectButton = !digitalRead(SD_EJECT_BUTTON_PIN); 

//--------------------------------- Вывод даты на дисплей ----------------
  if (int(millis()/10000) - TimeD) { 
    TimeD = millis()/10000;
    DisplayData(GetDate(), DATE_STRING); 
  }

//--------------------------------- Вывод времени на дисплей ----------------
  NewTime = GetTime();
  if(OldTime != NewTime){
    DisplayData(NewTime, TIME_STRING);
    OldTime = NewTime;
  }
  
//--------------------------------------------------------------------------
//----------------------------------- Цикл отображение веса ----------------

  if (int(millis()/500) - TimeS) { //период 
    TimeS = millis()/500;   

//--------------------------------- Термостабилизация ----------------
    if (am2320.readTemperature() > 36) digitalWrite(THERMO_STABIL_PIN, RELAY_OFF);
    if (am2320.readTemperature() < 34) digitalWrite(THERMO_STABIL_PIN, RELAY_ON);
    
//--------------------------------------- Вес ------------------------------
    Text = CorrectDisplay((Weight3-PD.ScaleTare)/PD.ScaleRate, 3, " ");
    DisplayWeight(Text);
//------------------------------------ Температуры ------------------------------
    DisplayData(CorrectDisplay(am2320.readTemperature(), 5,""), THEMP_STAB);
    DisplayData(CorrectDisplay(rtc.getTemp(), 5,""), THEMP_ROOM);
 
//---------------------- Обновляем суммарные веса за ... --------------
    t = rtc.getTime();
    if (t.date != PD.LastDay){
//------------------------------------ Новый день -------------------------------
      PD.LastDay = t.date;
      PD.DayWeight = 0;
      cout << "----------------------------New day\n";
      SavePrivetToEEPROM(&PD);
      DisplayData(CorrectDisplay(PD.DayWeight, 5,""), DAY_WEIGHT);
    }
    if (t.dow != PD.LastWeek){
//----------------------------------- Новая неделя ------------------------------
      PD.LastWeek = t.dow;
      if (PD.LastWeek == 1){
        PD.WeekWeight = 0;
        cout << "----------------------------New week\n";
        SavePrivetToEEPROM(&PD);
        DisplayData(CorrectDisplay(PD.WeekWeight, 6,""), WEEK_WEIGHT);
      }
    }
    if (t.mon != PD.LastMonth){
//------------------------------------ Новый месяц ------------------------------
      PD.LastMonth = t.mon;
      PD.MonthWeight = 0;
      cout << "----------------------------New month\n";
      SavePrivetToEEPROM(&PD);
      DisplayData(CorrectDisplay(PD.MonthWeight, 7,""), MONTH_WEIGHT);    
    }
    
//--------------------- Вес цемента в бункере больше допустимого ----------------
    if (((Weight10-PD.ScaleTare)/PD.ScaleRate > MAX_LOAD_WEIGHT) and !MaxWeightStatus){
      cout << "---------------------------Max weight\n";
      MaxWeightStatus = true;
      AutoLoad = false;                               //выключаем автоподачу      
      digitalWrite(BLOCK_PNEUMO_PIN, RELAY_ON);       //включаем запрет пневмоподачи
      digitalWrite(PN_BUTON_PRESS_PIN, RELAY_OFF);    //отпускаем кнопку пневмоподачи    
      DisplayAlert(" >" + (String)MAX_LOAD_WEIGHT + "кг", MSG_ERROR);
    }
    else{
      if(MaxWeightStatus){
        DisplayAlert("", MSG_ERROR);
        digitalWrite(BLOCK_PNEUMO_PIN, RELAY_OFF);    //выключаем запрет пневмоподачи
        MaxWeightStatus = false;
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
