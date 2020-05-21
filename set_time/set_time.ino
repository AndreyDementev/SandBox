
#include <DS3231.h>

// Инициализация DS3231
DS3231  rtc(SDA, SCL);

void setup()
{
  // Setup Serial connection
  Serial.begin(9600);
  // Раскомментировать для Arduino Leonardo
  //while (!Serial) {}
  
  // Инициализация rtc 
  rtc.begin();
  
  //Установка даты и времени
  rtc.setDOW(THURSDAY);     // День недели по английски MONDAY  TUESDAY  WEDNESDAY  THURSDAY  FRIDAY  SATURDAY  SUNDAY
  rtc.setTime(22, 39, 0);     // Установка часов 12:00:00 (24часовой формат)
  rtc.setDate(19, 12, 2019);   // Установка даты число, месяц, год
}

void loop()
{
  // Показываем день недели
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Выводим дату
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Выводим время
  Serial.print(rtc.getTimeStr());
  Serial.print(" -- ");
  
  Serial.print(rtc.getTemp()); //Температура
  Serial.println(" -- ");
  
  // Ждём 1 секунду
  delay (1000);
}
