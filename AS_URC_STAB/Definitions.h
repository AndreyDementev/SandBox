#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#define EEPROM_ADDRESS        0x57
#define EEPROM_PD_ADDRESS     0           // первых 100 байт оставляем под служебную информацию
#define SPI_SPEED SD_SCK_MHZ(3)

// File names
#define INT_LOG_FILE_NAME "ASURZLog.csv"  // On Display
#define OUT_LOG_FILE_NAME "ASURZLog.csv"  // On Shild
#define FILE_BASE_NAME    "SURZ.csv"      // In both times
#define PRIVET_FILE_NAME  "pvData01.dav"  // Служебный файл
#define LOG_FILE          "DataLog.txt"   // Лог-файл

// Определение для тензодатчика
#define SCALE_SAMPLE_WEIGHT   120         // устарело
#define SCALE_RATE_INIT       2235        // тарификация для инициализации
#define SCALE_TARE_INIT       356916      // масштабирование для инициализации
#define READE_COUNT           3           // устарело
#define SCALE_DELAY_TIME      2000        // время задержки измерения веса после пневмоподачи
#define MAX_LOAD_WEIGHT       170         // максимальный вес цемента в бункере

// Параметры отображения на TFT
#define WEIGHT_DIGIT_COUNT    3           // количество цифр для отображения веса

// Определения для TFT
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

// типф выводимой информации
#define DAY_WEIGHT   1
#define WEEK_WEIGHT  2
#define MONTH_WEIGHT 3
#define DATE_STRING  4
#define TIME_STRING  5
#define THEMP_ROOM   6
#define THEMP_STAB   7

// типы сообщений
#define MSG_ERROR    1   //сообщения об ошибке
#define MSG_WARNING  2   //сообщения о предупреждении
#define MSG_NOTICE   3   //сообщения об извещении

#define PRIVET_DATA_COUNT     10 // Количество переменных в структуре PrivatData для хранения на SD
#define RELAY_ON               0 // реле включается нулем
#define RELAY_OFF              1 // а выключается единицей

// Определения пинов

#define HARD_CS_PIN           53 // Внешний SD
#define SOFT_MOSI_PIN         11 // SD на мониторе  
#define SOFT_MISO_PIN         12 
#define SOFT_SCK_PIN          13     
#define SOFT_CS_PIN           10
#define SHAKE_HSD_PIN         27
#define RTC_SDA_PIN           20 //Часы
#define RTC_SCL_PIN           21

#define LOADCELL_DOUT_PIN     33 // Тензодатчик
#define LOADCELL_SCK_PIN      35
// Внешние управляющие сигналы
#define SD_EJECT_BUTTON_PIN   37 // кнопка "ВЫНУТЬ SD"
#define PNEUMO_SUPPLY_PIN     39 // контакт реле пневмоподачи 
#define TARE_BUTTON_PIN       41 // кнопка тарирования
#define SCALE_BUTTON_PIN      43 // кнопка масштабирования
#define BLOCK_PNEUMO_PIN      45 // блокировка кнопки пневмоподачи - реле 1. Нумерация сверху
#define THERMO_STABIL_PIN     47 // на резистор термостабилизации тензопреобразователя - реле 2
#define VALVE_LOCK_PIN        31 // включение замка на затворе бункера с цементом - реле 3
#define PN_BUTON_PRESS_PIN    29 // нажать кнопку пневмоподачи   - реле 4

#endif  //_DEFINITIONS_H_
