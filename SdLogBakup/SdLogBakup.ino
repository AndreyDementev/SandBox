#include <SPI.h>
#include "SdFat.h"
#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h

// Pin numbers on Display SD.
#define SOFT_MOSI_PIN 11     
#define SOFT_MISO_PIN 12 
#define SOFT_SCK_PIN  13     
#define SOFT_CS_PIN   10
// Pin numbers on Shied SD.
#define HARD_CS_PIN   53  
   
#define INT_LOG_FILE_NAME "ASURZLog.csv"
#define OUT_LOG_FILE_NAME "ASURZLog.csv"

// serial output steam
ArduinoOutStream cout(Serial);

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SoftSD;
SdFile SoftFile;

SdFat HardSD;
File HardFile;


void setup() {
  Serial.begin(9600);
  while (!Serial) SysCall::yield();
  cout << "Type any character to start\n";
  while (!Serial.available()) SysCall::yield();

// Открываем хардверный SD
//********************************************************
  cout << "Initializing Outer SD card...";
  if (!HardSD.begin(HARD_CS_PIN, SPI_HALF_SPEED)) {
    cout << "initialization failed!";
    return;
  }
  cout << "initialization done.\n";
  
  HardFile = HardSD.open(OUT_LOG_FILE_NAME, FILE_WRITE);  
  if (HardFile) {
    cout << OUT_LOG_FILE_NAME << "  opened\n";
    cout << "---------------------\n"; 
  } else {
    cout << "error opening " << OUT_LOG_FILE_NAME << "n";
    return;
  }  

// Открываем софтверный SD
//********************************************************
  cout << "Initializing Outer SD card...";
  if (!SoftSD.begin(SOFT_CS_PIN, SPI_HALF_SPEED)) { SoftSD.initErrorHalt(); }
  cout << "initialization done.\n";

  if (!SoftFile.open(INT_LOG_FILE_NAME, O_RDWR | O_CREAT)) {
    SoftSD.errorHalt(F("open failed"));
  }
  cout << INT_LOG_FILE_NAME << "  opened\n";
  cout << "---------------------\n"; 

//  Копируем весь файл из софт SD в хард SD
//********************************************************
  cout << "Copying " << INT_LOG_FILE_NAME << " to " OUT_LOG_FILE_NAME << "\n";  
  cout << "Copying started\n";
  while (SoftFile.available()){
    if (!HardFile.write(SoftFile.read())){
      cout << "File copy error\n";
      break;
    }
  }
  SoftFile.close();
  HardFile.close();
  cout << "Done.";
}
//------------------------------------------------------------------------------
void loop() {}
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS
