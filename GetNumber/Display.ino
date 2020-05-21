
#include <Fonts/DSEG7_Classic_Bold_64.h>
#include <FontsRus/FreeSans18.h>    //FreeSans18pt8b
#include <FontsRus/FreeSans14.h>    //FreeSans14pt8b
#include <FontsRus/FreeSans9.h>     //FreeSans9pt8b
#include <FontsRus/FreeSansOblique12.h>     //FreeSansOblique12pt8b

//#include <FontsRus/FreeMono18.h>    //FreeMono18pt8b
//#include <FontsRus/FreeMono14.h>    //FreeMono14pt8b



#define TFT_SIZE_X  480
#define TFT_SIZE_Y  320

#define TFTY(cY) TFT_SIZE_Y - (cY)

// ----Отображение теущего веса крупными цифрми
#define POS_WEIGHT_PLATE {146, 118, 0,0,0,0}
#define COL_WEIGHT_PLATE {BLACK, WHITE}

// Общие настройки панелей
#define TOP_BAR_POS         270              // Положение верхней панели подписей
#define TOP_BAR_FONT        FreeSans18pt8b   // Шрифт верхней панели подписей
#define BOT_BAR_POS         10               // Положение нижней панели подписей
#define BOT_BAR_FONT        FreeSans14pt8b   // Шрифт нижней панели подписей
#define MASK_COLOR          YELLOW           // цвет линий маски
#define MASK_FONT           FreeSans9pt8b    // Шрифт для панелей
#define MASK_TEXT_COLOR     {BLACK, WHITE}   // Цвет нрифта для панелей
#define MESSAGE_FONT        FreeSansOblique12pt8b
#define MSG_ERROR_COLOR     {BLACK, RED}     //Цвет сообщения об ошибке
#define MSG_WARNING_COLOR   {BLACK, YELLOW}  //Цвет сообщения о предупреждении
#define MSG_NOTICE_COLOR    {BLACK, GREEN}   //цвет сообщения об извещении
#define ALERT_FONT          TOP_BAR_FONT     //Шрифт на панеле оповещений


// ----Отображение расхода за период. Верхняя панель
#define POS_DAY_WEIGHT   {30, TOP_BAR_POS, 150,0,0,0}
#define COL_DAY_WEIGHT   {BLACK, WHITE}
#define FNT_DAY_WEIGHT   TOP_BAR_FONT

#define POS_WEEK_WEIGHT  {150, TOP_BAR_POS, 290,0,0,0}
#define COL_WEEK_WEIGHT  {BLACK, WHITE}
#define FNT_WEEK_WEIGHT  TOP_BAR_FONT

#define POS_MONTH_WEIGHT {330, TOP_BAR_POS, 450,0,0,0}
#define COL_MONTH_WEIGHT {BLACK, WHITE}
#define FNT_MONTH_WEIGHT TOP_BAR_FONT

// ----Сопутствующая информация. Нижняя панель
#define POS_DATE_STRING  {20, BOT_BAR_POS, 0,0,0,0}
#define COL_DATE_STRING  {BLACK, WHITE}
#define FNT_DATE_STRING  BOT_BAR_FONT

#define POS_TIME_STRING  {170, BOT_BAR_POS, 0,0,0,0}
#define COL_TIME_STRING  {BLACK, WHITE}
#define FNT_TIME_STRING  BOT_BAR_FONT

#define POS_THEMP_ROOM   {300, BOT_BAR_POS, 0,0,0,0}
#define COL_THEMP_ROOM   {BLACK, WHITE}
#define FNT_THEMP_ROOM   BOT_BAR_FONT

#define POS_THEMP_STAB   {390, BOT_BAR_POS, 0,0,0,0}
#define COL_THEMP_STAB   {BLACK, WHITE}
#define FNT_THEMP_STAB   BOT_BAR_FONT


struct cRect{ int16_t xs, ys, xf, yf;
             uint16_t w, h;};
struct dColor{uint16_t Fon, Text;};

String old_sw = "";


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
//                    DisplayWeight
//********************************************************  
void DisplayWeight(String sw){
  cRect r = POS_WEIGHT_PLATE;
  r.ys = TFT_SIZE_Y - r.ys; //пресчет координат к нижнему левому углу
  r.yf = TFT_SIZE_Y - r.yf; //Допустимые значения от 1 до TFT_SIZE_Y
  dColor c = COL_WEIGHT_PLATE;

  tft.setFont(&DSEG7_Classic_Bold_64);
  for (int i = 0; i < 3; i++){
    if (sw[i] != old_sw[i]) tft.fillRect(r.xs + 12 + i*106, r.ys, 80, -128, BLACK);
    tft.drawChar(r.xs + i*106, r.ys, Text[i], c.Text, c.Fon, 2);
  }    
  old_sw = sw;
}


//********************************************************  
//                    DisplayData
//********************************************************  
void DisplayData(String ds, uint8_t mode){
  cRect r, r1;
  dColor c;
  GFXfont f;
//Serial.print("DisplayData = "); Serial.println(ds);
  switch (mode){
    case DAY_WEIGHT:{
      r = POS_DAY_WEIGHT;
      c = COL_DAY_WEIGHT;
      f = FNT_DAY_WEIGHT;
      break;
    }
    case WEEK_WEIGHT:{
      r = POS_WEEK_WEIGHT;
      c = COL_WEEK_WEIGHT;
      f = FNT_WEEK_WEIGHT;
      break;
    }
    case MONTH_WEIGHT:{
      r = POS_MONTH_WEIGHT;
      c = COL_MONTH_WEIGHT;
      f = FNT_MONTH_WEIGHT;  
      break;
    }
    case DATE_STRING:{
      r = POS_DATE_STRING;
      c = COL_DATE_STRING;
      f = FNT_DATE_STRING;
      break;
    }
    case TIME_STRING:{
      r = POS_TIME_STRING;
      c = COL_TIME_STRING;
      f = FNT_TIME_STRING;
      break;
    }
    case THEMP_ROOM:{
      r = POS_THEMP_ROOM;
      c = COL_THEMP_ROOM;
      f = FNT_THEMP_ROOM;
      break;
    }
    case THEMP_STAB:{
      r = POS_THEMP_STAB;
      c = COL_THEMP_STAB;
      f = FNT_THEMP_STAB;
      break;
    }
    default:{}
  }
  
  r.ys = TFTY(r.ys);   r.yf = TFTY(r.yf); //пресчет к нижнему левому углу
  tft.setFont(&f);
  tft.setTextColor(c.Text, c.Fon);
  tft.setTextSize(1);
  tft.setTextWrap(false);  
  tft.getTextBounds(ds, r.xs, r.ys, &r1.xf, &r1.yf, &r1.w, &r1.h);
  if (r.xf != 0){                     //Если указана правая точка, прижимаем текст к ней 
    r1.xf -= r.xs - r.xf + r1.w;
    r.xs = r.xf - r1.w;       
  }
  tft.setCursor(r.xs, r.ys);
  tft.fillRect(r1.xf, r1.yf, r1.w + 8, r1.h, c.Fon);
  tft.print(ds);
}



//********************************************************  
//                    DisplayAlert
//********************************************************
void DisplayAlert (String alt, uint8_t mode){
   
  cRect r;
  dColor c;
  
  switch (mode){
    case MSG_ERROR:{
      c = MSG_ERROR_COLOR;
      r.xs = 10; r.ys = TFTY(250); r.w = 130; r.h = 65;
     break;
    }
    case MSG_WARNING:{
      c = MSG_WARNING_COLOR;
      r.xs = 10; r.ys = TFTY(175); r.w = 130; r.h = 65;
      break;
    }    
    case MSG_NOTICE:{
      c = MSG_NOTICE_COLOR;
      r.xs = 10; r.ys = TFTY(175); r.w = 130; r.h = 65;
      break;
    }
  }
  
  tft.setFont(&ALERT_FONT);
  tft.setTextColor(c.Fon, c.Text);
  tft.setTextSize(1);
  tft.setTextWrap(false);  
  tft.setCursor(r.xs, r.ys + 40);
  if (alt.length() == 0){
    tft.fillRoundRect(r.xs, r.ys, r.w, r.h, 5, c.Fon);
  }
  else{
    tft.fillRoundRect(r.xs, r.ys, r.w, r.h, 5, c.Text);
    tft.print(alt);
  }
    
}



//********************************************************  
//                    DisplayMessage
//********************************************************
void DisplayMessage(String msg, uint8_t mode){
  
  dColor c;
  switch (mode){
    case MSG_ERROR:{
      c = MSG_ERROR_COLOR;
      break;
    }
    case MSG_WARNING:{
      c = MSG_WARNING_COLOR;
      break;
    }    case MSG_NOTICE:{
      c = MSG_NOTICE_COLOR;
      break;
    }
  }
  
  tft.setFont(&MESSAGE_FONT);
  tft.setTextColor(c.Text, c.Fon);
  tft.setTextSize(1);
  tft.setTextWrap(true);  
  tft.setCursor(2, TFTY(77));
  tft.fillRect(1, TFTY(99), 478, 60, c.Fon);
  tft.print(msg);
  
}

//********************************************************  
//                    DrawMask
//********************************************************  
void DrawMask(){

  dColor c = MASK_TEXT_COLOR;
   
//Горизонтальные
  tft.drawFastHLine(0, TFTY(1),   TFT_SIZE_X, MASK_COLOR);
  tft.drawFastHLine(0, TFTY(38),  TFT_SIZE_X, MASK_COLOR);
  tft.drawFastHLine(0, TFTY(100), TFT_SIZE_X, MASK_COLOR);
  tft.drawFastHLine(0, TFTY(260), TFT_SIZE_X, MASK_COLOR);
  tft.drawFastHLine(0, TFTY(300), TFT_SIZE_X, MASK_COLOR);
  tft.drawFastHLine(0, TFTY(320), TFT_SIZE_X, MASK_COLOR);

//Вертикальные
  tft.drawFastVLine(  0, 0, TFT_SIZE_Y, MASK_COLOR);
  tft.drawFastVLine(TFT_SIZE_X-1, 0, TFT_SIZE_Y, MASK_COLOR);
  //Снизу 
  tft.drawFastVLine(160, TFTY(38), 37, MASK_COLOR);
  tft.drawFastVLine(285, TFTY(38), 37, MASK_COLOR);
  tft.drawFastVLine(375, TFTY(38), 37, MASK_COLOR);
  //Сверху
  tft.drawFastVLine(160, TFTY(320), 60, MASK_COLOR);
  tft.drawFastVLine(305, TFTY(320), 60, MASK_COLOR);
//Подписываем  

  tft.setFont(&MASK_FONT);
  tft.setTextColor(c.Text, c.Fon);
  tft.setTextSize(1);
  tft.setTextWrap(false);  
//  tft.getTextBounds(ds, r.xs, r.ys, &r1.xf, &r1.yf, &r1.w, &r1.h);
  tft.setCursor(70, TFTY(305));
  tft.print("за день");
  tft.setCursor(190, TFTY(305));
  tft.print("за неделю");
  tft.setCursor(365, TFTY(305));
  tft.print("за месяц");  
}




//********************************************************  
//                    DrawGreed
//********************************************************  
void DrawGreed(){
  for(int i=0; i<tft.width(); i+=10){
    if (int(i % 50)){ tft.drawFastVLine(i, 0, tft.height(), RED);}
    else {            tft.drawFastVLine(i, 0, tft.height(), GREEN);}
  }
  for(int i=0; i<tft.height(); i+=10){
    if (int(i % 50)){ tft.drawFastHLine(0, tft.height()-i-1, tft.width(), RED);}
    else {            tft.drawFastHLine(0, tft.height()-i-1, tft.width(), GREEN);}
  }
}
