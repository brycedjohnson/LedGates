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
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }

  
    std::string uuid = pCharacteristic->getUUID().toString() 
    if (uuid == TYPE_UUID) {
      setType(pCharacteristic->getValue().toInt());
    } else if (uuid == BRIGHTNESS_UUID)  {
      setBrightness(pCharacteristic->getValue().toInt());
    } else if (uuid == SPEED_UUID) {
      setSpeed(pCharacteristic->getValue().toInt());
    } else if (uuid == OFFSET_UUID) {
      setOffset(pCharacteristic->getValue().toInt());
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    pCharType->setValue(0);

    std::string uuid = pCharacteristic->getUUID().toString() 
    if (uuid == TYPE_UUID) {
      pCharType->setValue(getType());
    } else if (uuid == BRIGHTNESS_UUID)  {
      pCharType->setValue(getBrightness());
    } else if (uuid == SPEED_UUID) {
      pCharType->setValue(getSpeed());
    } else if (uuid == OFFSET_UUID) {
      pCharType->setValue(getOffset));
    }
  }
};

class IpCallback : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    pCharacteristic->setValue(WiFi.localIP().toString().c_str());
  }
};

void bleInit(void) {
  BLEDevice::init("ESP_LEDGates");
  pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharType = pService->createCharacteristic(
                      TYPE_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharType->setValue(0);
  pCharType->setCallbacks(new MyCallbacks());

  pCharBrightness = pService->createCharacteristic(
                      BRIGHTNESS_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharBrightness->setValue(0);
  pCharBrightness->setCallbacks(new MyCallbacks());

  pCharDistance = pService->createCharacteristic(
                      DISTANCE_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharDistance->setValue(0);
  pCharDistance->setCallbacks(new MyCallbacks());

  pCharSpeed = pService->createCharacteristic(
                      SPEED_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharSpeed->setValue(0);
  pCharSpeed->setCallbacks(new MyCallbacks());

  pCharOffset = pService->createCharacteristic(
                      OFFSET_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharOffset->setValue(0);
  pCharOffset->setCallbacks(new MyCallbacks());


  pCharIP = pService->createCharacteristic(
                      IP_UUID,
                      BLECharacteristic::PROPERTY_READ   
                    );
  pCharIP->setValue("0.0.0.0");
  pCharIP->setCallbacks(new IpCallback());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();


  Serial.println("Starting BLE work!");

}

void bleSetIp(char* IP) 
{
  pCharIP->setValue(IP);

}
