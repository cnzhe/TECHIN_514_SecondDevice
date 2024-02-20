#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Pin configuration for DS18B20 temperature sensor
#define ONE_WIRE_BUS 2

// Create OneWire and DallasTemperature instances
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Bluetooth configuration
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Rechargeable battery configuration
#define BATTERY_PIN 34

void setup() {
  Serial.begin(115200);

  // Initialize DS18B20 temperature sensor
  sensors.begin();

  // Initialize Bluetooth
  BLEDevice::init("TemperatureSensor");
  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();

  // Configure battery monitoring
  analogReadResolution(10);
}

void loop() {
  // Read temperature from DS18B20
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // Update Bluetooth characteristic with temperature data
  pCharacteristic->setValue(String(temperature).c_str());
  pCharacteristic->notify();

  // Read battery voltage
  int rawValue = analogRead(BATTERY_PIN);
  float batteryVoltage = (rawValue / 1023.0) * 3.3 * 2;

  // Print temperature and battery voltage to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Battery Voltage: ");
  Serial.print(batteryVoltage);
  Serial.println(" V");

  delay(5000);  // Delay for 5 seconds before the next iteration
}