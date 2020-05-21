#ifndef _DISPLAYCONFIG_H_
#define _DISPLAYCONFIG_H_

#define TFTY(cY) TFT_SIZE_Y - (cY)

// ----Отображение теущего веса крупными цифрми
#define POS_WEIGHT_PLATE {146, 118, 0,0,0,0}
#define COL_WEIGHT_PLATE {BLACK, WHITE}

// Общие настройки панелей
#define TFT_SIZE_X  480
#define TFT_SIZE_Y  320

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
#define POS_DAY_WEIGHT   {30, TOP_BAR_POS, 145,0,0,0}
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

#endif //_DISPLAYCONFIG_H_
