#ifndef _RTC_H_
#define _RTC_H_

#include <DS3231.h>


class SysT{
public:
  SysT(DS3231* ds);
  void begin();
  void syncTime();
  Time getTime();
  uint32_t getUnixTime(Time t);
  Time unixToTime(uint32_t ut);
  const char* getDOWstr(Time tm);
  bool getDS3231err();              // возвращает наличие сбоя при обращении к DS3231 
  bool getDS3231status();           // возвращает режим работы часов: 1-реальное время,
                                    // 0-RTC не ответили, время стартовало от 01.04.2020
  uint32_t getTimeAfterLastRead();  // возвращает время в секундах от последнего чтения часов
private:
  DS3231* Ds;
  uint32_t TimeShift;
  uint32_t TimeStamp;
  uint32_t LastRead = 0;
  uint32_t LastSync = 0;
  bool DS3231ReadErr = false;
  bool Switch_Years(uint16_t Y);
  const char* DayNames[7]={"Пн", "Вт", "Ср", "Чт", "Пт", "Сб","Вс"};
  uint8_t DaysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
};

SysT::SysT(DS3231* ds){
  Ds = ds;
}

void SysT::begin(){
  syncTime();
}

//Вычисляем разницу, между UnixTime DS3231 и количеством секунд в millis().
//В дальнейшем при вычислении времени будем брать millis() и прибавлять это значение
//Если произошел сбой при чтении времени из DS3231, то нам вернется 2313941504
//
void SysT::syncTime(){
  TimeStamp = getUnixTime(Ds->getTime());
  if (TimeStamp == 2313941504){             //ошибка чтения времени    
    TimeStamp = 1585735200;                 //1 апреля 2020 10:00:00
    TimeShift = TimeStamp - millis()/1000;  //вычисляем сдвиг времени
    DS3231ReadErr = true;
    return;
  }
  TimeShift = TimeStamp - millis()/1000;    //вычисляем сдвиг времени
  LastSync = TimeStamp;
  DS3231ReadErr = false;
}

Time SysT::getTime(){
  Time tm;
  TimeStamp = millis()/1000 + TimeShift;
  tm = unixToTime(TimeStamp);
  LastRead = TimeStamp;
  return tm;
}

bool SysT::Switch_Years(uint16_t Y){
  bool sw;
  sw = !(Y % 4) && ((Y % 100) || !(Y % 400));
  return sw;
}

uint32_t SysT::getUnixTime(Time t){
  return Ds->getUnixTime(t);
}

Time SysT::unixToTime(uint32_t ut){
  Time tm;
  int monthLength;
  unsigned long time, days;
  byte w, m, y;

  time = ut;
  tm.sec = time % 60;
  time /= 60; // now it is minutes
  tm.min = time % 60;
  time /= 60; // now it is hours
  tm.hour = time % 24;
  time /= 24; // now it is days
  
  tm.year = 1970;
  days = 0;
  while(( days += (365 + Switch_Years(tm.year)) ) <= time)
    tm.year++;
  
  days -= (365 + Switch_Years(tm.year));
  
  time -= days; // now it is days in this year, starting at 0
  
  days = 0;
  for ( tm.mon = 0; tm.mon < 12; tm.mon++ ) {
    if (tm.mon == 1) // february
      monthLength = DaysInMonth[tm.mon] + Switch_Years(tm.year);
    else
      monthLength = DaysInMonth[tm.mon];
    
    if (time >= monthLength)
      time -= monthLength;
    else
      break;
  }
  
  tm.mon++;                    // jan is month 1
  tm.date = (int)time + 1;     // day of month
  
// вычисление дня недели
  m = tm.mon; y = tm.year;
  if (m == 1 || m == 2) { m += 12; --y; }
  w = (tm.date + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
  
  tm.dow = ++w;
  return tm;
}

const char* SysT::getDOWstr(Time tm){
  if ((tm.dow >= 1) && (tm.dow <= 7)) return DayNames[tm.dow - 1];
  return DayNames[1];
}

bool SysT::getDS3231err(){
  TimeStamp = getUnixTime(Ds->getTime());
  if (TimeStamp == 2313941504)  DS3231ReadErr = true;  //ошибка чтения времени
  else                          DS3231ReadErr = false;
  return DS3231ReadErr;
}

bool SysT::getDS3231status(){
  return !DS3231ReadErr; 
}

uint32_t SysT::getTimeAfterLastRead(){
  return millis()/1000 + TimeShift - LastRead;
}



#endif //_RTC_H_
