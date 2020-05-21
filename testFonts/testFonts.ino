/*
 * generate testcard similar to BMP
 */

#include <Adafruit_GFX.h>
#if defined(_GFXFONT_H_)           //are we using the new library?
#include <c:\Program Files\Arduino\libraries\Adafruit-GFX-Library-master\Fonts\DSEG7_Classic_Bold_128.h>
#include <c:\Program Files\Arduino\libraries\Adafruit-GFX-Library-master\Fonts\DSEG7_Classic_Bold_64.h>
#define ADJ_BASELINE 11            //new fonts setCursor to bottom of letter
#else
#define ADJ_BASELINE 0             //legacy setCursor to top of letter
#endif
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#define BLACK   ~0x0000
#define BLUE    ~0x001F
#define RED     ~0xF800
#define GREEN   ~0x07E0
#define CYAN    ~0x07FF
#define MAGENTA ~0xF81F
#define YELLOW  ~0xFFE0
#define WHITE   ~0xFFFF

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

#define GREY      RGB(127, 127, 127)
#define DARKGREY  RGB(64, 64, 64)
#define TURQUOISE RGB(0, 128, 128)
#define PINK      RGB(255, 128, 192)
#define OLIVE     RGB(128, 128, 0)
#define PURPLE    RGB(128, 0, 128)
#define AZURE     RGB(0, 128, 255)
#define ORANGE    RGB(255,128,64)
 
#include <stdio.h>

uint16_t ID;
uint8_t hh, mm, ss; //containers for current time

uint8_t conv2d(const char* p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9') v = *p - '0';
    return 10 * v + *++p - '0';
}

void setup(void)
{
    Serial.begin(9600);
    tft.reset();
    ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    //    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setFont(&DSEG7_Classic_Bold_64);

    hh = conv2d(__TIME__);
    mm = conv2d(__TIME__ + 3);
    ss = conv2d(__TIME__ + 6);

/*    for(int i=0; i<tft.width(); i+=10){
      if (int(i % 50)){ tft.drawFastVLine(i, 0, tft.height(), RED);}
      else {            tft.drawFastVLine(i, 0, tft.height(), GREEN);}
    }
    for(int i=0; i<tft.height(); i+=10){
      if (int(i % 50)){ tft.drawFastHLine(0, i, tft.width(), RED);}
      else {            tft.drawFastHLine(0, i, tft.width(), GREEN);}
    }

    tft.setCursor(0, 50);
    tft.setTextSize(1);
    tft.print("007887009");
   
    int weight = -246;
    if (weight < 0){
      tft.setCursor(0, 250);
      tft.print("-");
    }*/
    tft.setTextSize(2);
    tft.setCursor(30, 250);
    tft.print("146.3");
    
/*    tft.setFont(&DSEG7_Classic_Bold_128);
    tft.setCursor(0, 319);
    tft.setTextSize(1);
    tft.print("246.0");
*/    
}

void loop(void)
{
/*    tft.fillRect(108, 39, 108, 18, BLACK);    

    tft.print(tft.readID(), HEX);
    //    tft.setFont(NULL);
    //    tft.setTextSize(2);
    while (1) {}
        if (++ss > 59) {
            ss = 0;
            mm++;
            if (mm > 59) {
                mm = 0;
                hh++;
                if (hh > 23) hh = 0;
            }
        }
        char buf[20];
        sprintf(buf, "%02d:%02d:%02d", hh, mm, ss);
        tft.fillRect(108, 10 * 18 + 3, 6 * 18, 18, BLACK);
        tft.setCursor(128, 187 + ADJ_BASELINE);
        tft.print(buf);
        delay(1000);
    }
*/
}
