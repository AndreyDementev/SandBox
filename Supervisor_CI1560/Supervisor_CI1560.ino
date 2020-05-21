#include <Wire.h>

#define _DEBUG_


#define RELAY_ON          0     // реле включается нулем
#define RELAY_OFF         1     // а выключается единицей
#define BUTTON_ON         0     // кнопка нажата - вход закорочен на землю
#define BUTTON_OFF        1     // кнопка отпущена - вход подтянут к питанию

// определение пинов NANO
#define LED_PIN           13
#define WATCH_PIN_IN      7     // соединен с 25 пин дуе. Считываем с него сигналы испарвности DUE
#define WATCH_PIN_OUT     8     // соединен с 27 пин дуе. Передаем на него сигналы нашей исправности
#define STOP_WATCH_PIN    9     // Подключен к кнопке. BUTTON_ON выключает наблюдение за DUE
#define RELAY_PIN         6     // дуе подключена к нормально-замкнутому контакту этого реле
                                // чтобы ее перезагрузить, реле нужно установить RELAY_ON
                                
#define TIME_OUT_DUE      7       // период ожидания сигнала дуе (сек)

// настройки шины I2C
#define CLOCK_FREQUENCY   100000  // частота шины I2C
#define SVSR_ADDRES       7       // адрес супервайзера на шине I2C

// результаты приема данных CI-1560A
#define FRAME_SUCCESSESFUL  '0'   // кадр данных CI-1560A принят успешно
#define FRAME_NOT_COMPLETE  '1'   // ошибка приема. Маркер конца данных пришел раньше чем положено
#define FRAME_NO_MARKER     '2'   // ошибка приема. Данные пришли а маркера конца данных нет


long count = TIME_OUT_DUE;
bool WatchIn;                   // запомненное состояние входного сигнала DUE
bool  ReadComplete = false;     // признак окончания чтения во входящий массив
byte iD[23];                    // массив входящих данных  с CI-1560. [0] хранит число введенных байт

// завершенный массив данных с CI-1560 (его можно отправлять в любой момент). [0] = результат приема пакета данных
byte scaleData[23] = {'-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','\r','\n'};


uint32_t t = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin(SVSR_ADDRES);
  Wire.setClock(CLOCK_FREQUENCY);
  Wire.onRequest(requestEvent);
  Serial.print("Start");

#ifdef _DEBUG_
  Serial.print(" debug");
#endif
  Serial.print("\n");
  
  iD[0] = 0;
  pinMode(LED_PIN, OUTPUT);       digitalWrite(LED_PIN, HIGH);
  pinMode(WATCH_PIN_OUT, OUTPUT); digitalWrite(WATCH_PIN_OUT, HIGH);
  pinMode(RELAY_PIN, OUTPUT);     digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(WATCH_PIN_IN, INPUT);   digitalWrite(WATCH_PIN_IN, HIGH);
  pinMode(STOP_WATCH_PIN, INPUT); digitalWrite(STOP_WATCH_PIN, HIGH);
}

void loop(){
  
  if (digitalRead(WATCH_PIN_IN) == WatchIn){          // ищем сигналы от дуе
    WatchIn = !WatchIn;
    count = TIME_OUT_DUE;                             // если нашли - перезапускаем счетчик
  }
  if (millis() > t){                                  // заходим сюда раз в секунту
    t = millis() + 1000;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));     // моргаем светодиодом
    count--;

    Serial.write(scaleData, 23);
 
    if (count < 1){                                   // время истекло. Перезагрузка DUE
#ifndef _DEBUG_
      if (!digitalRead(STOP_WATCH_PIN))                // наблюдение отключено, не перезагружаем
        restartDUE();
#endif //_DEBUG_
      count = TIME_OUT_DUE;
    } 
  }
}

//-----------------------------------------------------------------------------------------------
void restartDUE(){
  digitalWrite(RELAY_PIN, RELAY_ON);                    // отключаем питание DUE
  digitalWrite(LED_PIN, HIGH);                          // сигнал светодиодом
  delay(500);                                           // даем разрядиться конденсаторам
  digitalWrite(LED_PIN, LOW);                           // сигнал светодиодом
  digitalWrite(RELAY_PIN, RELAY_OFF);                   // включает питание
  digitalWrite(WATCH_PIN_OUT, LOW);                     // нажимаем ресет, а то бывает не запускается
  delay(250);
  digitalWrite(WATCH_PIN_OUT, HIGH);                    // отпускаем ресет
}

//-----------------------------------------------------------------------------------------------
void requestEvent() 
{
  Wire.write(scaleData, 23);              // I2C. На запрос мастера отправляем данные CI-1560A
}


//-----------------------------------------------------------------------------------------------
void serialEvent() {
  byte ib;
  
  while (Serial.available())
  {
    ib = Serial.read();                     // получаем новый байт
    Serial.println(ib, HEX);                // печатаем его для контроля
    iD[0]++;                                // iD[0] - количество принятых байт. Сейчас указывает
                                            // на следующий пустой элемент массива.
    
    if (iD[0] > 22)                         // конец пакета а маркер не найден. Это значит сбой передачи
    {
      if (scaleData[0] == FRAME_NO_MARKER)  // проверяем режим поиска маркер
      { 
        if ((ib == 10) & (iD[22] == 13))    // маркер найден
        {
          iD[0] = 0;                        // начинаем новый пакет. FRAME_NO_MARKER оставляем
          return;
        }
      }
      scaleData[0] = FRAME_NO_MARKER;       // переходим (продолжаем) режим поиска маркера
      iD[0]  = 22;                          // 22 - чтобы попасть на следующем цикле в поиск маркера.
      iD[22] = ib;                          // запоминаем последний байт
      return;                               // выходим из обработчика.
    }
    
    iD[iD[0]] = ib;                         // записываем его в массив
    
    if ((ib == 10) & (iD[iD[0]-1] == 13))   // ищем маркер конца данных (13, 10)
    {                     
      if (iD[0] == 22)                      // проверяем размер принятого пакета
      {
        scaleData[0] = FRAME_SUCCESSESFUL;  // пакет успешно принят
        for (int i=1; i<23; i++) 
          scaleData[i] = iD[i];             // копируем данные в выходной массив
        iD[0] = 0;                          // Начинаем новый пакет 
      }
      else
      {
        scaleData[0] = FRAME_NOT_COMPLETE;  // данных пришло меньше положенного
        iD[0] = 0;                          // Начинаем новый пакет 
      }
    }
     
  }
}

/*
 * Формат кадра RS232 CI-1560A  22 байта
 *   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22
 * | U| S| ,| G| S| ,|ID|LS| ,|          DATA         |  | K| G|CR|LF|
 *   S  T     N  T
 *   O  L
 * 
 *   1,2 - US (unstable) | ST (stable) | OL (overload)
 *   4,5 - GS (gross) | NT (net)
 *   7   - Device ID
 *   8   - Lamp status bite
 *   18  - символ Space (32) (в описании Blank???)
 * 21,22 - символы CR, LF (13, 10)
 * 
 * Описание применения:
 * https://owen.ru/forum/showthread.php?t=29851&s=c3e11f3f039bafb3c2af96d09aaf37f1&p=294963&viewfull=1#post294963
 */
