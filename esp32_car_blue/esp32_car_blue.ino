
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define ENA   2          // Enable/speed motors Right        GPIO14(D5)
#define ENB   4
#define MOT_1F 5
#define MOT_1R 18
#define MOT_2F 19
#define MOT_2R 21
int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;


BLECharacteristic *pCharacteristic;
BLEDescriptor *pDescriptor;
bool deviceConnected = false;
bool deviceNotifying = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "963d05a8-0a56-11ea-8d71-362b9e155667" // UART service UUID
#define CHARACTERISTIC_UUID_RX "963d05a8-0a56-11ea-8d71-362b9e155667"
#define CHARACTERISTIC_UUID_TX "963d05a8-0a56-11ea-8d71-362b9e155667"

void up() {
  
  digitalWrite(MOT_1F, HIGH); // turn on
  digitalWrite(MOT_2F, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1F, LOW); // turn off
  digitalWrite(MOT_2F, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void down() {
  digitalWrite(MOT_1R, HIGH); // turn on
  digitalWrite(MOT_2R, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1R, LOW); // turn off
  digitalWrite(MOT_2R, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void left() {
  digitalWrite(MOT_1R, HIGH); // turn on
  digitalWrite(MOT_2F, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1R, LOW); // turn off
  digitalWrite(MOT_2F, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void right() {
  
  digitalWrite(MOT_1F, HIGH); // turn on
  digitalWrite(MOT_2R, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1F, LOW); // turn off
  digitalWrite(MOT_2R, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void up_right() {
  
  digitalWrite(MOT_1F, HIGH); // turn on
  digitalWrite(MOT_2R, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1F, LOW); // turn off
  digitalWrite(MOT_2R, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void up_left() {
  digitalWrite(MOT_1R, HIGH); // turn on
  digitalWrite(MOT_2F, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1R, LOW); // turn off
  digitalWrite(MOT_2F, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void down_right() {
  
  digitalWrite(MOT_1F, HIGH); // turn on
  digitalWrite(MOT_2R, HIGH); // turn on
  //analogWrite(ENA, speedCar);
  delay(500);              // wait
  digitalWrite(MOT_1F, LOW); // turn off
  digitalWrite(MOT_2R, LOW); // turn off
  //analogWrite(ENB, speedCar/speed_Coeff);

}

void down_left() {
  digitalWrite(MOT_1R, HIGH); // turn on
  digitalWrite(MOT_2F, HIGH); // turn on
  //analogWrite(ENA, speedCar/speed_Coeff);
  delay(500);              // wait
  digitalWrite(MOT_1R, LOW); // turn off
  digitalWrite(MOT_2F, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

void stop() {
  digitalWrite(MOT_1R, LOW); // turn on
  digitalWrite(MOT_2F, LOW); // turn on
  //analogWrite(ENA, speedCar/speed_Coeff);
  delay(500);              // wait
  digitalWrite(MOT_1R, LOW); // turn off
  digitalWrite(MOT_2F, LOW); // turn off
  //analogWrite(ENB, speedCar);

}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        if (rxValue == "up") {
          up();
        }
        if (rxValue == "down") {
          down();
        }
        if (rxValue == "right") {
          right();
        }
        if (rxValue == "left") {
          left();
        }

        Serial.println();
        Serial.println("*********");
      }
    }
};


class MyDisCallbacks: public BLEDescriptorCallbacks {
    void onWrite(BLEDescriptor *pDescriptor) {
      uint8_t* rxValue = pDescriptor->getValue();

      if (pDescriptor->getLength() > 0) {
        if (rxValue[0] == 1) {
          //deviceNotifying=true;
        } else {
          deviceNotifying = false;
        }
        Serial.println("*********");
        Serial.print("Received Descriptor Value: ");
        for (int i = 0; i < pDescriptor->getLength(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};


void setup() {

  Serial.begin(115200);
  pinMode(MOT_1F, OUTPUT);
  pinMode(MOT_1R, OUTPUT);
  pinMode(MOT_2F, OUTPUT);
  pinMode(MOT_2R, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);


  // Create the BLE Device
  BLEDevice::init("UART Ricardo");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pDescriptor = new BLE2902();
  pCharacteristic->addDescriptor(pDescriptor);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pDescriptor->setCallbacks(new MyDisCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  if (deviceConnected && deviceNotifying) {
    Serial.printf("*** Sent Value: %d ***\n", txValue);
    pCharacteristic->setValue(&txValue, 1);
    pCharacteristic->notify();
    txValue++;
  }
  delay(1000);
}
