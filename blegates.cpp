#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>
#include <sstream>
#include <FastLED.h>
#include <WiFi.h>
#include "gates.h"
#include "persist.h"

BLEServer* pServer = NULL;
BLECharacteristic* pCharType = NULL;
BLECharacteristic* pCharBrightness = NULL;
BLECharacteristic* pCharDistance = NULL;
BLECharacteristic* pCharSpeed = NULL;
BLECharacteristic* pCharOffset = NULL;
BLECharacteristic* pCharIP = NULL;

#define SERVICE_UUID "12eaeae5-8035-4488-99d3-48c87c04a000"
#define TYPE_UUID "0a297a8b-23aa-459b-ace0-9ce2723a6001"
#define BRIGHTNESS_UUID "0a297a8b-23aa-459b-ace0-9ce2723a6002"
#define DISTANCE_UUID "0a297a8b-23aa-459b-ace0-9ce2723a6003"
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
    } else if (uuid == OFFSET_UUID) {
      setOffset(value);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    std::string uuid = pCharacteristic->getUUID().toString();

    if (uuid == TYPE_UUID) {
      Serial.printf("Type Read: %d\n", getType());
      //pCharacteristic->setValue(ss.str());
    } else if (uuid == BRIGHTNESS_UUID) {
      Serial.printf("Brightness Read: %d\n", getBrightness());
      //pCharacteristic->setValue(ss.str());
    } else if (uuid == SPEED_UUID) {
      Serial.printf("Speed Read: %d\n", getSpeed());
      //pCharacteristic->setValue(ss.str());
    } else if (uuid == OFFSET_UUID) {
      Serial.printf("Offset Read: %d\n", getOffset());
      //pCharacteristic->setValue(ss.str());      
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

  pCharDistance = pService->createCharacteristic(
                      DISTANCE_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharDistance->setCallbacks(new MyCallbacks());

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

