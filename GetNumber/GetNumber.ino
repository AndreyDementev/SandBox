
int16_t GetNumber(int16_t Num, uint8_t But1, But2){
    long TimeB;
    String Text;

    DisplayMessage("Управление: \"Тара\" +1, \"Масштаб\" -1\nВыход: \"Тара\" и \"Масштаб\" одновременно", MSG_NOTICE);
    DisplayWeight(Text);
    while(!digitalRead(But1));             // ждем пока отпустят кнопку масштабирование
    
                                          //--------------цикл пока не нажмут две кнопки для выхода
    while(digitalRead(But2) or digitalRead(But1)){

      TimeB = millis()/500;
          
                                          //--------------увеличиваем вес
      while(!digitalRead(But2)){           // считаем на увеличение
 Serial.println("Нажата тара");
        Num++;                                 // сразу добавляем единичку
        while ((millis()/500 - TimeB) <= 1) {        // пауза 0.5 секунды
// Serial.print("(millis()/500) = "); Serial.println((millis()/500));
         if(!digitalRead(But1)){          // если во время паузы нажмется вторая кнопка - выходим
 Serial.println("Нажат масштаб (в таре)");
            Num--;                             // и убираем лишнюю единичку
            break;
          }
 Serial.println("Проверяем отпускание кнопки)");
          if(digitalRead(But2)) break;     // если во время паузы кнопка отпускается - выходим
        }
        
 Serial.println("На выходе проверяем нажатие второй кнопки");
        if(!digitalRead(But1)) break;     // если нажата вторая кнопка - выходим из второго цикла
        Text = CorrectDisplay(Num, 3, " ");    // отображаем вес
        DisplayWeight(Text);
 Serial.println("На выходе проверяем отпускание кнопки");
        if(digitalRead(But2))   break;     // если кнопка отпущена - выходим
                
 Serial.println("Заходим в быстрый цикл)");
        while(!digitalRead(But2)){         // пока не отпустят кнопку
          Num++;                               // добавляем быстрые единички. Вторую кнопку не проверяем
          Text = CorrectDisplay(Num, 3, " ");
          DisplayWeight(Text);
          delay(100);                                 // пауза 0.1 секунды        
        }
      }
      
      TimeB = millis()/500;
                                          //--------------уменьшаем вес
      while(!digitalRead(But1)){          // считаем на уменьшение
 Serial.println("Нажат масштаб");
        Num--;                                 // сразу вычитаем единичку
        while ((millis()/500 - TimeB) <= 1) {        // пауза 0.5 секунды
          if(!digitalRead(But2)){          // если во время паузы нажмется вторая кнопка - выходим
 Serial.println("Нажата тара (в масштабе)");
            Num++;                             // и добавляем нужную единичку
            break;
          }
          if(digitalRead(But1))break;     // если во время паузы кнопка отпускается - выходим
        }
        
        if(!digitalRead(But2))  break;     // если нажата вторая кнопка - выходим  из второго цикла
        Text = CorrectDisplay(Num, 3, " ");    // отображаем вес
        DisplayWeight(Text);
        if(digitalRead(But1))  break;     // если кнопка отпущена - выходим
        
        while(!digitalRead(But1)){        // пока не отпустят кнопку
          Num--;                               // вычитаем быстрые единички. Вторую кнопку не проверяем
          Text = CorrectDisplay(Num, 3, " ");
          DisplayWeight(Text);
          delay(100);                                 // пауза 0.1 секунды        
        }       
      }    
    }
Serial.println("Вышли из цикла");
                                                     
    DisplayMessage("", MSG_NOTICE);
    while(!digitalRead(But2) or !digitalRead(But1)){} //----------------ждем пока отпустят обе кнопки
    return Num
}
