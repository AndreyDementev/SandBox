// Print extra info for debug if DEBUG_PRINT is nonzero
#define DEBUG_PRINT 1
#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"

//const uint8_t chipSelect = 53;
#define SPI_SPEED SD_SCK_MHZ(10)
#define SOFT_MOSI_PIN         11 // SD на мониторе  
#define SOFT_MISO_PIN         12 
#define SOFT_SCK_PIN          13     
#define SOFT_CS_PIN           10


// Serial output stream
ArduinoOutStream cout(Serial);

//Sd2Card card;
 
uint32_t cardSizeBlocks;
uint32_t cardCapacityMB;

// cache for SD block
cache_t cache;

// MBR information
uint8_t partType;
uint32_t relSector;
uint32_t partSize;

// Fake disk geometry
uint8_t numberOfHeads;
uint8_t sectorsPerTrack;

// FAT parameters
uint16_t reservedSectors;
uint8_t sectorsPerCluster;
uint32_t fatStart;
uint32_t fatSize;
uint32_t dataStart;

// constants for file system structure
uint16_t const BU16 = 128;
uint16_t const BU32 = 8192;

//  strings needed in file system structures
char noName[] = "NO NAME    ";
char fat16str[] = "FAT16   ";
char fat32str[] = "FAT32   ";

//------------------------------------------------------------------------------
// write cached block to the card
uint8_t writeCache(Sd2Card* card, uint32_t lbn) {
  return card->writeBlock(lbn, cache.data);
}
//------------------------------------------------------------------------------
// initialize appropriate sizes for SD capacity
String initSizes() {
  if (cardCapacityMB <= 6) {
    return "Sd_Format: Card is too small.";
  } else if (cardCapacityMB <= 16) {
    sectorsPerCluster = 2;
  } else if (cardCapacityMB <= 32) {
    sectorsPerCluster = 4;
  } else if (cardCapacityMB <= 64) {
    sectorsPerCluster = 8;
  } else if (cardCapacityMB <= 128) {
    sectorsPerCluster = 16;
  } else if (cardCapacityMB <= 1024) {
    sectorsPerCluster = 32;
  } else if (cardCapacityMB <= 32768) {
    sectorsPerCluster = 64;
  } else {
    // SDXC cards
    sectorsPerCluster = 128;
  }

  cout << F("Blocks/Cluster: ") << int(sectorsPerCluster) << endl;
  // set fake disk geometry
  sectorsPerTrack = cardCapacityMB <= 256 ? 32 : 63;

  if (cardCapacityMB <= 16) {
    numberOfHeads = 2;
  } else if (cardCapacityMB <= 32) {
    numberOfHeads = 4;
  } else if (cardCapacityMB <= 128) {
    numberOfHeads = 8;
  } else if (cardCapacityMB <= 504) {
    numberOfHeads = 16;
  } else if (cardCapacityMB <= 1008) {
    numberOfHeads = 32;
  } else if (cardCapacityMB <= 2016) {
    numberOfHeads = 64;
  } else if (cardCapacityMB <= 4032) {
    numberOfHeads = 128;
  } else {
    numberOfHeads = 255;
  }
  return "";
}
//------------------------------------------------------------------------------
// zero cache and optionally set the sector signature
void clearCache(uint8_t addSig) {
  memset(&cache, 0, sizeof(cache));
  if (addSig) {
    cache.mbr.mbrSig0 = BOOTSIG0;
    cache.mbr.mbrSig1 = BOOTSIG1;
  }
}
//------------------------------------------------------------------------------
// zero FAT and root dir area on SD
String clearFatDir(Sd2Card* card, uint32_t bgn, uint32_t count) {
  clearCache(false);

  if (!card->writeStart(bgn, count)) {
    return "Sd_Format: Clear FAT/DIR writeStart failed";
  }
  for (uint32_t i = 0; i < count; i++) {
    if ((i & 0XFF) == 0) {
      cout << '.';
    }
    if (!card->writeData(cache.data)) {
      return "Sd_Format: Clear FAT/DIR writeData failed";
    }
  }
  if (!card->writeStop()) {
    return "Sd_Format: Clear FAT/DIR writeStop failed";
  }
  cout << endl;
  return "";
}
//------------------------------------------------------------------------------
// return cylinder number for a logical block number
uint16_t lbnToCylinder(uint32_t lbn) {
  return lbn / (numberOfHeads * sectorsPerTrack);
}
//------------------------------------------------------------------------------
// return head number for a logical block number
uint8_t lbnToHead(uint32_t lbn) {
  return (lbn % (numberOfHeads * sectorsPerTrack)) / sectorsPerTrack;
}
//------------------------------------------------------------------------------
// return sector number for a logical block number
uint8_t lbnToSector(uint32_t lbn) {
  return (lbn % sectorsPerTrack) + 1;
}
//------------------------------------------------------------------------------
// format and write the Master Boot Record
String  writeMbr(Sd2Card* card) {
  clearCache(true);
  part_t* p = cache.mbr.part;
  p->boot = 0;
  uint16_t c = lbnToCylinder(relSector);
  if (c > 1023) {
    return "Sd_Format: MBR CHS";
  }
  p->beginCylinderHigh = c >> 8;
  p->beginCylinderLow = c & 0XFF;
  p->beginHead = lbnToHead(relSector);
  p->beginSector = lbnToSector(relSector);
  p->type = partType;
  uint32_t endLbn = relSector + partSize - 1;
  c = lbnToCylinder(endLbn);
  if (c <= 1023) {
    p->endCylinderHigh = c >> 8;
    p->endCylinderLow = c & 0XFF;
    p->endHead = lbnToHead(endLbn);
    p->endSector = lbnToSector(endLbn);
  } else {
    // Too big flag, c = 1023, h = 254, s = 63
    p->endCylinderHigh = 3;
    p->endCylinderLow = 255;
    p->endHead = 254;
    p->endSector = 63;
  }
  p->firstSector = relSector;
  p->totalSectors = partSize;
  if (!writeCache(card, 0)) {
    return "Sd_Format: write MBR";
  }
  return "";
}
//------------------------------------------------------------------------------
// generate serial number from card size and micros since boot
uint32_t volSerialNumber() {
  return (cardSizeBlocks << 8) + micros();
}
//------------------------------------------------------------------------------
// format the SD as FAT16
String makeFat16(Sd2Card* card) {
  String rs;
  uint32_t nc;
  for (dataStart = 2 * BU16;; dataStart += BU16) {
    nc = (cardSizeBlocks - dataStart)/sectorsPerCluster;
    fatSize = (nc + 2 + 255)/256;
    uint32_t r = BU16 + 1 + 2 * fatSize + 32;
    if (dataStart < r) {
      continue;
    }
    relSector = dataStart - r + BU16;
    break;
  }
  // check valid cluster count for FAT16 volume
  if (nc < 4085 || nc >= 65525) return "Sd_Format: Bad cluster count";
  reservedSectors = 1;
  fatStart = relSector + reservedSectors;
  partSize = nc * sectorsPerCluster + 2 * fatSize + reservedSectors + 32;
  if (partSize < 32680) {
    partType = 0X01;
  } else if (partSize < 65536) {
    partType = 0X04;
  } else {
    partType = 0X06;
  }
  // write MBR
  rs = writeMbr(card);
  if (rs != "") return rs;
  clearCache(true);
  fat_boot_t* pb = &cache.fbs;
  pb->jump[0] = 0XEB;
  pb->jump[1] = 0X00;
  pb->jump[2] = 0X90;
  for (uint8_t i = 0; i < sizeof(pb->oemId); i++) {
    pb->oemId[i] = ' ';
  }
  pb->bytesPerSector = 512;
  pb->sectorsPerCluster = sectorsPerCluster;
  pb->reservedSectorCount = reservedSectors;
  pb->fatCount = 2;
  pb->rootDirEntryCount = 512;
  pb->mediaType = 0XF8;
  pb->sectorsPerFat16 = fatSize;
  pb->sectorsPerTrack = sectorsPerTrack;
  pb->headCount = numberOfHeads;
  pb->hidddenSectors = relSector;
  pb->totalSectors32 = partSize;
  pb->driveNumber = 0X80;
  pb->bootSignature = EXTENDED_BOOT_SIG;
  pb->volumeSerialNumber = volSerialNumber();
  memcpy(pb->volumeLabel, noName, sizeof(pb->volumeLabel));
  memcpy(pb->fileSystemType, fat16str, sizeof(pb->fileSystemType));
  // write partition boot sector
  if (!writeCache(card, relSector)) return "Sd_Format: FAT16 write PBS failed";
  // clear FAT and root directory
  rs = clearFatDir(card, fatStart, dataStart - fatStart);
  if (rs != "") return rs;
  clearCache(false);
  cache.fat16[0] = 0XFFF8;
  cache.fat16[1] = 0XFFFF;
  // write first block of FAT and backup for reserved clusters
  if (!writeCache(card, fatStart) || !writeCache(card, fatStart + fatSize)) {
    return "Sd_Format: FAT16 reserve failed";
  }
  return "";
}
//------------------------------------------------------------------------------
// format the SD as FAT32
String makeFat32(Sd2Card* card) {
  String rs;
  uint32_t nc;
  relSector = BU32;
  for (dataStart = 2 * BU32;; dataStart += BU32) {
    nc = (cardSizeBlocks - dataStart)/sectorsPerCluster;
    fatSize = (nc + 2 + 127)/128;
    uint32_t r = relSector + 9 + 2 * fatSize;
    if (dataStart >= r) {
      break;
    }
  }
  // error if too few clusters in FAT32 volume
  if (nc < 65525) return "Sd_Format: Bad cluster count";
  reservedSectors = dataStart - relSector - 2 * fatSize;
  fatStart = relSector + reservedSectors;
  partSize = nc * sectorsPerCluster + dataStart - relSector;
  // type depends on address of end sector
  // max CHS has lbn = 16450560 = 1024*255*63
  if ((relSector + partSize) <= 16450560) {
    // FAT32
    partType = 0X0B;
  } else {
    // FAT32 with INT 13
    partType = 0X0C;
  }
  rs =writeMbr(card);
  if (rs != "") return rs;
  clearCache(true);

  fat32_boot_t* pb = &cache.fbs32;
  pb->jump[0] = 0XEB;
  pb->jump[1] = 0X00;
  pb->jump[2] = 0X90;
  for (uint8_t i = 0; i < sizeof(pb->oemId); i++) {
    pb->oemId[i] = ' ';
  }
  pb->bytesPerSector = 512;
  pb->sectorsPerCluster = sectorsPerCluster;
  pb->reservedSectorCount = reservedSectors;
  pb->fatCount = 2;
  pb->mediaType = 0XF8;
  pb->sectorsPerTrack = sectorsPerTrack;
  pb->headCount = numberOfHeads;
  pb->hidddenSectors = relSector;
  pb->totalSectors32 = partSize;
  pb->sectorsPerFat32 = fatSize;
  pb->fat32RootCluster = 2;
  pb->fat32FSInfo = 1;
  pb->fat32BackBootBlock = 6;
  pb->driveNumber = 0X80;
  pb->bootSignature = EXTENDED_BOOT_SIG;
  pb->volumeSerialNumber = volSerialNumber();
  memcpy(pb->volumeLabel, noName, sizeof(pb->volumeLabel));
  memcpy(pb->fileSystemType, fat32str, sizeof(pb->fileSystemType));
  // write partition boot sector and backup
  if (!writeCache(card, relSector) || !writeCache(card, relSector + 6))
    return "Sd_Format: FAT32 write PBS failed";
  clearCache(true);
  // write extra boot area and backup
  if (!writeCache(card, relSector + 2) || !writeCache(card, relSector + 8))
    return "Sd_Format: FAT32 PBS ext failed";
  fat32_fsinfo_t* pf = &cache.fsinfo;
  pf->leadSignature = FSINFO_LEAD_SIG;
  pf->structSignature = FSINFO_STRUCT_SIG;
  pf->freeCount = 0XFFFFFFFF;
  pf->nextFree = 0XFFFFFFFF;
  // write FSINFO sector and backup
  if (!writeCache(card, relSector + 1)
      || !writeCache(card, relSector + 7)) return "Sd_Format: FAT32 FSINFO failed";
  rs = clearFatDir(card, fatStart, 2 * fatSize + sectorsPerCluster);
  if (rs != "") return rs;
  clearCache(false);
  cache.fat32[0] = 0x0FFFFFF8;
  cache.fat32[1] = 0x0FFFFFFF;
  cache.fat32[2] = 0x0FFFFFFF;
  // write first block of FAT and backup for reserved clusters
  if (!writeCache(card, fatStart) || !writeCache(card,fatStart + fatSize)) {
    return "Sd_Format: FAT32 reserve failed";
  }
  return "";
}

//------------------------------------------------------------------------------
String formatCard(Sd2Card *card, uint8_t chipSelect) {
  String rs;
  cout << endl;
  cout << "Formatting\n";
  pinMode(29, OUTPUT);
  digitalWrite(29,LOW);
  delay(20);
  digitalWrite(29,HIGH);  
  
  Serial.print("chipSelect = "); Serial.println(chipSelect);
  delay(500);
  if (!card->begin(chipSelect, SPI_SPEED)) return "Sd_Format: card.begin failed";
  
  cout << "card.begin done \n";
  cardSizeBlocks = card->cardSize();
  Serial.print("cardSizeBlocks "); Serial.println(cardSizeBlocks);
  if (cardSizeBlocks == 0) return "Sd_Format: cardSize wrong";
  cardCapacityMB = (cardSizeBlocks + 2047)/2048;
  
  rs = initSizes();
  if (rs != "") return rs;
  if (card->type() != SD_CARD_TYPE_SDHC) {
    cout << F("FAT16\n");
    rs = makeFat16(card);
    if (rs != "") return rs;
  } else {
    cout << F("FAT32\n");
    rs = makeFat32(card);
    if (rs != "") return rs;
  }
  cout << F("Format done\n");
  return "";
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void setup() {

  String rs;
  Serial.begin(9600);
  while (!Serial) yield();
  SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SoftSD;   // SD на плате монитора


  rs = formatCard(&SoftSD, 10);
  if (rs != "") {cout << "Doone with error "; 
  Serial.println(rs);}
  else cout << "Done";

}
//------------------------------------------------------------------------------
void loop() {}
