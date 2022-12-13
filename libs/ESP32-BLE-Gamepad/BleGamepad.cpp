#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include <hardwareSerial.h>
#include "BleConnectionStatus.h"
#include "BleGamepad.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

static const uint8_t _hidReportDescriptor[] = {

  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x04, // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  REPORT_ID(1),        0x01, //     REPORT_ID (1)
  // ------------------------------------------------- Buttons (1 to 32)
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x80, //     USAGE_MAXIMUM (Button 128)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x80, //     REPORT_COUNT (128)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ; 16 bytes
  // ------------------------------------------------- X/Y position, Z/rZ position
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  USAGE(1),            0x32, //     USAGE (Z)
  USAGE(1),            0x35, //     USAGE (rZ)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  //REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
  //LOGICAL_MINIMUM(2),  0x01, 0x80, // LOGICAL_MINIMUM (-32767)
  //LOGICAL_MAXIMUM(2),  0xff, 0x7f, // LOGICAL_MAXIMUM (32767)
  //REPORT_SIZE(1),      0x10, //     REPORT_SIZE (16)
  REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
 
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;4 bytes (X,Y,Z,rZ) (8 bytes)

  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x33, //     USAGE (rX) Left Trigger
  USAGE(1),            0x34, //     USAGE (rY) Right Trigger
  USAGE(1),            0x36, //     USAGE (analog5) Slider
  USAGE(1),            0x37, //     USAGE (analog6) Dial
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  //REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
  //LOGICAL_MINIMUM(2),  0x01, 0x80, // LOGICAL_MINIMUM (-32767)
  //LOGICAL_MAXIMUM(2),  0xff, 0x7f, // LOGICAL_MAXIMUM (32767)
  //REPORT_SIZE(1),      0x10, //     REPORT_SIZE (16)
  REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;4 bytes rX, rY (8 bytes)

  USAGE_PAGE(1),   	   0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1), 		       0x39, //     USAGE (Hat switch)
  USAGE(1), 		       0x39, //     USAGE (Hat switch)
  USAGE(1), 		       0x39, //     USAGE (Hat switch)
  USAGE(1), 		       0x39, //     USAGE (Hat switch)
  LOGICAL_MINIMUM(1),  0x01, //		LOGICAL_MINIMUM (1)
  LOGICAL_MAXIMUM(1),  0x08, //     LOGICAL_MAXIMUM (8)
  REPORT_SIZE(1), 	   0x04, //		REPORT_SIZE (4)
  REPORT_COUNT(1), 	   0x04, //		REPORT_COUNT (4)
  HIDINPUT(1), 		     0x02, //		INPUT (Data, Variable, Absolute) ;2 byte Hat1, Hat2, Hat3, Hat4
 
  END_COLLECTION(0),         //   END_COLLECTION
  END_COLLECTION(0)          // END_COLLECTION
};

BleGamepad::BleGamepad(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel):
  hid(0)
{
  this->resetButtons();
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->batteryLevel = batteryLevel;
  this->connectionStatus = new BleConnectionStatus();
}

void BleGamepad::resetButtons() {
  memset(&_buttons,0,sizeof(_buttons));
}

void BleGamepad::print64(uint64_t num, int base) {
   uint32_t low = num % 0xFFFFFFFF; 
   uint32_t high = (num >> 32) % 0xFFFFFFFF;

  Serial.print(high,base);
  Serial.print("|");
  Serial.print(low,base); 
}

void BleGamepad::begin(void)
{
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleGamepad::end(void)
{
}

void BleGamepad::setAxes(int16_t x, int16_t y, int16_t a1, int16_t a2, int16_t a3, int16_t a4, int16_t a5, int16_t a6, signed char hat1, signed char hat2, signed char hat3, signed char hat4)
{
  if (this->isConnected())
  {
    


    uint8_t m[26]; // 34
    memset(&m,0,sizeof(m));

    memcpy(&m, &_buttons, sizeof(_buttons));
    /*
    m[0] =  _buttons[0];
    m[1] = (_buttons[0] >> 8);
	  m[2] = (_buttons[0] >> 16);
	  m[3] = (_buttons[0] >> 24);
    m[4] = (_buttons[0] >> 32); 
    m[5] = (_buttons[0] >> 40);
    m[6] = (_buttons[0] >> 48);
    m[7] = (_buttons[0] >> 56);

    m[8]  = _buttons[1];
	  m[9]  = (_buttons[1] >> 8);
	  m[10] = (_buttons[1] >> 16);
	  m[11] = (_buttons[1] >> 24);
    m[12] = (_buttons[1] >> 32); 
    m[13] = (_buttons[1] >> 40);
    m[14] = (_buttons[1] >> 48);
    m[15] = (_buttons[1] >> 56);
    */
   // test high res
    /*
    m[16] = x;
    m[17] = (x >>8);
    m[18] = y;
    m[19] = (y >>8);

    m[20] = a1;
    m[21] = (a1 >>8);
    m[22] = a2;
    m[23] = (a2 >>8);

	  m[24] = a3; // 20
    m[25] = (a3 >>8);
	  m[26] = a4;
    m[27] = (a4 >>8);
    m[28] = a5;
    m[29] = (a5 >>8);
	  m[30] = a6;
    m[31] = (a6 >>8);
    */
    m[16] = x;
    m[17] = y;
    m[18] = a1; // z
    m[19] = a2; // rx
    m[20] = a3; // ry
    m[21] = a4; // rz
    m[22] = a5; // slider
    m[23] = a6; // dial
    m[24] = hat1 | (hat2 << 4); // 1 and 2
    m[25] = hat3 | (hat4 << 4); // 3 and 4 
    
    //memset(&m,0, sizeof(m));
    this->inputGamepad->setValue(m, sizeof(m));
    this->inputGamepad->notify();
  }
}

// indexed button (1..128)
void BleGamepad::press(uint8_t b)
{

  char index = (b-1) / 8;
  char bit = (b-1) % 8;
  uint8_t bitmask = (1 << bit);

  uint8_t result = _buttons[index] | bitmask;
  if (result != _buttons[index]) {
    _buttons[index] = result;
    //this->setAxes(0,0,0,0,0,0,0,0,0,0,0,0);
  }
  /*
  Serial.print(b); 
  Serial.print(" ");
  this->print64(bitmask, BIN);
  Serial.print(" ");
  this->print64(result, BIN);
  Serial.print(" ");
  this->print64(_buttons[0], BIN);
  Serial.println(" ");
  */
}

// indexed button (1..128)
void BleGamepad::release(uint8_t b)
{
  char index = (b-1) / 8;
  char bit = (b-1) % 8;
  uint8_t bitmask = (1 << bit);

  uint64_t result = _buttons[index] & ~bitmask;
  //if (result != _buttons[index]) {
    _buttons[index] = result;
    //this->setAxes(0,0,0,0,0,0,0,0,0,0,0,0);
  //}
}

// indexed button (1..128)
bool BleGamepad::isPressed(uint8_t b)
{
  char index = (b-1) / 8;
  char bit = (b-1) % 8;
  uint8_t bitmask = (1 << bit);

  if ((bitmask & _buttons[index]) > 0)
    return true;
  return false;
}

bool BleGamepad::isConnected(void) {
  return this->connectionStatus->connected;
}

void BleGamepad::setBatteryLevel(uint8_t level) {
  this->batteryLevel = level;
}

void BleGamepad::taskServer(void* pvParameter) {
  BleGamepad* BleGamepadInstance = (BleGamepad *) pvParameter; //static_cast<BleGamepad *>(pvParameter);
  BLEDevice::init(BleGamepadInstance->deviceName);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(BleGamepadInstance->connectionStatus);

  BleGamepadInstance->hid = new BLEHIDDevice(pServer);
  BleGamepadInstance->inputGamepad = BleGamepadInstance->hid->inputReport(1); // <-- input REPORTID from report map
  BleGamepadInstance->connectionStatus->inputGamepad = BleGamepadInstance->inputGamepad;

  BleGamepadInstance->hid->manufacturer()->setValue(BleGamepadInstance->deviceManufacturer);

  BleGamepadInstance->hid->pnp(0x01,0x02e5,0xabcd,0x0110);
  BleGamepadInstance->hid->hidInfo(0x00,0x01);

  BLESecurity *pSecurity = new BLESecurity();

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  BleGamepadInstance->hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  BleGamepadInstance->hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(BleGamepadInstance->hid->hidService()->getUUID());
  pAdvertising->start();
  BleGamepadInstance->hid->setBatteryLevel(BleGamepadInstance->batteryLevel);

  ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}