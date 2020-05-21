#include "/Users/andrey/Рабочая/Сафоновский ЖБИ/Проект Arduino/MyLibraries/Definitions.h"

int16_t GetNumber(int16_t Num, uint8_t bUp, uint8_t bDown){
  long TimeB;
  String sNum;
  
  DisplayMessage("Управление: \"Тара\" +1, \"Масштаб\" -1\nВыход: \"Тара\" и \"Масштаб\" одновременно", MSG_NOTICE);
  sNum = CorrectDisplay(Num, 3, " ");
  DisplayWeight(sNum);
  
  while(!digitalRead(bDown)){
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
    watchdogReset();             // ждем пока отпустят кнопку масштабирование
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
  }
  
                                        //--------------цикл пока не нажмут две кнопки для выхода
  while(digitalRead(bUp) or digitalRead(bDown)){
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
  watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
    TimeB = millis()/500;
        
                                        //--------------увеличиваем вес
    while(!digitalRead(bUp)){           // считаем на увеличение
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
      watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
      Num++;                                 // сразу добавляем единичку
      while ((millis()/500 - TimeB) <= 1) {        // пауза 0.5 секунды
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
        watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
        if(!digitalRead(bDown)){          // если во время паузы нажмется вторая кнопка - выходим
          Num--;                             // и убираем лишнюю единичку
          break;
        }
        if(digitalRead(bUp)) break;     // если во время паузы кнопка отпускается - выходим
      }
      
      if(!digitalRead(bDown)) break;     // если нажата вторая кнопка - выходим из второго цикла
      sNum = CorrectDisplay(Num, 3, " ");    // отображаем вес
      DisplayWeight(sNum);
      if(digitalRead(bUp))   break;     // если кнопка отпущена - выходим
              
      while(!digitalRead(bUp)){         // пока не отпустят кнопку
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
        watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
        Num++;                               // добавляем быстрые единички. Вторую кнопку не проверяем
        sNum = CorrectDisplay(Num, 3, " ");
        DisplayWeight(sNum);
        delay(100);                                 // пауза 0.1 секунды        
      }
    }
    
    TimeB = millis()/500;
                                        //--------------уменьшаем вес
    while(!digitalRead(bDown)){          // считаем на уменьшение
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
    watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
    Num--;                                 // сразу вычитаем единичку
      while ((millis()/500 - TimeB) <= 1) {        // пауза 0.5 секунды
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
        watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
        if(!digitalRead(bUp)){          // если во время паузы нажмется вторая кнопка - выходим
          Num++;                             // и добавляем нужную единичку
          break;
        }
        if(digitalRead(bDown))break;     // если во время паузы кнопка отпускается - выходим
      }
      
      if(!digitalRead(bUp))  break;     // если нажата вторая кнопка - выходим  из второго цикла
      sNum = CorrectDisplay(Num, 3, " ");    // отображаем вес
      DisplayWeight(sNum);
      if(digitalRead(bDown))  break;     // если кнопка отпущена - выходим
      
      while(!digitalRead(bDown)){        // пока не отпустят кнопку
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
       watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
       Num--;                               // вычитаем быстрые единички. Вторую кнопку не проверяем
        sNum = CorrectDisplay(Num, 3, " ");
        DisplayWeight(sNum);
        delay(100);                                 // пауза 0.1 секунды        
      }       
    }    
  }
                                                   
  DisplayMessage("", MSG_NOTICE);
  while(!digitalRead(bUp) or !digitalRead(bDown)){ //----------------ждем пока отпустят обе кнопки
#ifdef ARDUINO_AVR_MEGA2560
#elif ARDUINO_SAM_DUE
    watchdogReset();
#endif
    digitalWrite(WATCH_PIN_OUT, !digitalRead(WATCH_PIN_OUT));  // for supervisor 
  }
  return Num;
}
