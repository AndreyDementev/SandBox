
#include <DS3231.h>
#include <RTCDue.h>

RTCDue DueRtc(RC);
DS3231 DsRtc(SDA1, SCL1);
Time t;

const char* daynames[]={"Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб"};

void setup() {
  Serial.begin(9600);
  
  DueRtc.begin();
  DsRtc.begin();

  t = DsRtc.getTime();

  DueRtc.setTime(t.hour, t.min, t.sec);
  DueRtc.setDate(t.date, t.mon, t.year);
}

void loop() {
  // Print date...
  Serial.print(daynames[DueRtc.getDayofWeek()]);
  Serial.print(" ");
  Serial.print(DueRtc.getDay());
  Serial.print("/");
  Serial.print(DueRtc.getMonth());
  Serial.print("/");
  Serial.print(DueRtc.getYear());
  Serial.print("\t");
  
  // ...time...
  Serial.print(DueRtc.getHours());
  Serial.print(":");
  Serial.print(DueRtc.getMinutes());
  Serial.print(":");
  Serial.println(DueRtc.getSeconds());

  Serial.println();
  delay(300);
}
