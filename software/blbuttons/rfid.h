///////////////////////////////////////////////////////////////////////////////
/// RIFD Implementation ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef __ALARM_RIFD_H__
#define __ALARM_RIFD_H__

#include <SPI.h>
#include <MFRC522.h>

//SPI definitions for ESP32 - Hardwired

#define RST_PIN  22      //RST     - required for mfrc522 config
#define SDA_PIN  21      //SDA(SS) - required for mfrc522 config

#define SCK_PIN  18      //SCK  - harcoded in lib, not use them
#define MOSI_PIN 23      //MOSI - harcoded in lib, not use them
#define MISO_PIN 19      //MISO - harcoded in lib, not use them

extern MFRC522 mfrc522;

enum class KEY_STATUS: int {
    NONE    = 0,  // no card read
    VALID   = 1,  // valid card (RFID) read (key in the table of valid keys)
    INVALID = 2   // invalid key (not registered key)
};
#endif