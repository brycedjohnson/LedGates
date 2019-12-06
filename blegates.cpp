#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>
#include <FastLED.h>
#include <WiFi.h>
#include "gates.h"
#include "persist.h"

BLEServer* pServer = NULL;
BLECharacteristic* pCharType = NULL;
BLECharacteristic* pCharBrightness = NULL;
BLECharacteristic* pCharSkip = NULL;
BLECharacteristic* pCharSpeed = NULL;
BLECharacteristic* pCharOffset = NULL;
BLECharacteristic* pCharIP = NULL;

#define SERVICE_UUID "12eaeae5-8035-4488-99d3-48c87c04a000"
#define TYPE_UUID "0a297a8b-23aa-459b-ace0-9ce2723a6001"
#define BRIGHTNESS_UUID "0a297a8b-23aa-459b-ace0-9ce2723a6002"
#define SKIP_UUID "0a297a8b-23aa-459b-ace0-9ce2723a6003"
#define SPEED_UUID "12eaeae5-8035-4488-99d3-48c87c04a004"
#define OFFSET_UUID "12eaeae5-8035-4488-99d3-48c87c04a005"
#define IP_UUID "12eaeae5-8035-4488-99d3-48c87c04a006"

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    int32_t value = atoi(pCharacteristic->getValue().c_str());

    Serial.printf("New value: %d\n", value);

    std::string uuid = pCharacteristic->getUUID().toString();
    if (uuid == TYPE_UUID) {
      setType(value);
    } else if (uuid == BRIGHTNESS_UUID)  {
      setBrightness(value);
    } else if (uuid == SPEED_UUID) {
      setSpeed(value);
    } else if (uuid == SKIP_UUID) {
      setSkip(value);      
    } else if (uuid == OFFSET_UUID) {
      setOffset(value);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    char stringbuf[10]; 
    std::string uuid = pCharacteristic->getUUID().toString();

    if (uuid == TYPE_UUID) {
      sprintf(stringbuf, "%d",  getType()); 
      Serial.printf("Type Read: %s\n", stringbuf);
      pCharacteristic->setValue(stringbuf);
    } else if (uuid == BRIGHTNESS_UUID) {
      sprintf(stringbuf, "%d",  getBrightness()); 
      Serial.printf("Brightness Read: %s\n", stringbuf);
      pCharacteristic->setValue(stringbuf);
    } else if (uuid == SPEED_UUID) {
      sprintf(stringbuf, "%d",  getSpeed()); 
      Serial.printf("Speed Read: %s\n", stringbuf);
      pCharacteristic->setValue(stringbuf);
    } else if (uuid == SKIP_UUID) {
      sprintf(stringbuf, "%d",  getSkip()); 
      Serial.printf("Skip Read: %s\n", stringbuf);
      pCharacteristic->setValue(stringbuf);      
    } else if (uuid == OFFSET_UUID) {
      sprintf(stringbuf, "%d",  getOffset()); 
      Serial.printf("Offset Read: %s\n", stringbuf);
      pCharacteristic->setValue(stringbuf);
    } else if (uuid == IP_UUID) {
      pCharacteristic->setValue(WiFi.localIP().toString().c_str());
    }
  }
};

void bleInit(void)
{
  BLEDevice::init("ESP_LEDGates");
  pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharType = pService->createCharacteristic(
                      TYPE_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharType->setCallbacks(new MyCallbacks());

  pCharBrightness = pService->createCharacteristic(
                      BRIGHTNESS_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharBrightness->setCallbacks(new MyCallbacks());

  pCharSkip = pService->createCharacteristic(
                      SKIP_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharSkip->setCallbacks(new MyCallbacks());

  pCharSpeed = pService->createCharacteristic(
                      SPEED_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharSpeed->setCallbacks(new MyCallbacks());

  pCharOffset = pService->createCharacteristic(
                      OFFSET_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharOffset->setCallbacks(new MyCallbacks());


  pCharIP = pService->createCharacteristic(
                      IP_UUID,
                      BLECharacteristic::PROPERTY_READ   
                    );
  pCharIP->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();


  Serial.println("Starting BLE work!");

}

void bleSetIp(char* IP) 
{
  pCharIP->setValue(IP);
}

