#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <SoftwareSerial.h>

/*
 * Pinagem
 * 
 * LCD - ESP32
 * - SDA - D21
 * - SCL - D22
 * - VCC - VIN
 * - GND - GND
 * 
 * BUTTON GRAVAR
 * PIN1 - D18
 * 
 * BUTTON - CANCELAR 
 * PIN1 - D19
 * 
 * FINGERPRINT - ESP32
 * GND - GND
 * RX - TX
 * TX - RX
 * VCC - 3V3
 * 
 * MODULO RELE - ESP32
 * IN (5V) - D5
 * VCC - VCC (5V)
 * GND - GND 
 * 
 */



//SoftwareSerial mySerial(2, 3); //Inisialisasi serial RX TX modul fingerprint
HardwareSerial mySerial(0);
LiquidCrystal_I2C lcd(0x27, 20, 4); //Inisialisasi I2C LCD

int doorLock = 8; //pin untuk sinyal doorlock
uint8_t id;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

#define RELE 5

Button buttonGravar = {18, 0, false};
Button buttonCancelar = {19, 0, false};

void IRAM_ATTR isr() {
  buttonGravar.numberKeyPresses += 1;
  buttonGravar.pressed = true;
}


void setup()
{
  Serial.begin(57600);
  while (!Serial);
  delay(100);
  Serial.println("nnAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  lcd.begin();
  lcd.clear();

  msgInit();

  // Fechadura
  pinMode(doorLock, OUTPUT);
  digitalWrite(doorLock, HIGH);

  // Botao
  pinMode(buttonGravar.PIN, INPUT_PULLUP);
  attachInterrupt(buttonGravar.PIN, isr, FALLING);

}

void loop()
{

//  int teste = finger.templateCount;
//  ++teste;
//  Serial.println(teste);
//  return;
  if (buttonGravar.pressed && buttonGravar.numberKeyPresses > 5) {
    Serial.printf("Button 1 has been pressed %u times\n", buttonGravar.numberKeyPresses);
    //buttonGravar.pressed = false;
    id = readnumber();
    if (id == 0) {// ID #0 not allowed, try again!
      Serial.println("ID #: 0");
      return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);

    while (!getFingerprintEnroll());

    buttonGravar.pressed = false;
    buttonGravar.numberKeyPresses = 0;

  } else {
    
    getFingerprintID();
    
  }

  //getFingerprintID();
  delay(50);
}

// Gravacao da digital
uint8_t readnumber(void) {
  uint8_t num = finger.templateCount;
  return ++num;
  
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); 
  Serial.println(id);
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  
  Serial.print("ID "); 
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  
  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); 
  Serial.println(id);
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}

// Leitura da digital
//------------------------------------------------------------
uint8_t getFingerprintID() {

  uint8_t p = finger.getImage();

  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!---------------------------------------------
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) { //jika fingerprint terdeteksi

    Serial.println("Found a print match!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LEITURA CONFIRMADA");
    delay (100);
    //suara1();
    digitalWrite(doorLock, LOW);
    lcd.setCursor(0, 1);
    lcd.print("DESTRAVANDO ");
    for (int i = 5; i > 0; i--)
    {
      lcd.setCursor (13, 1);
      lcd.print(i);
      delay (1000);
    }
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("SEJA BEM VINDO");
    delay(100);
    lcd.setCursor(2, 1);
    lcd.print("PORTA DESTRAVADA");
    delay(1000);

    digitalWrite(doorLock, HIGH);

    delay(3000);

    msgInit();

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

    Serial.println("Communication error");
    return p;

  } else if (p == FINGERPRINT_NOTFOUND) { //jika fingerprint salah

    Serial.println("Did not find a match");
    //suara2();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DIGITAL");
    delay (100);
    lcd.setCursor(0, 1);
    lcd.print("NAO");
    delay (100);
    lcd.setCursor(0, 2);
    lcd.print("ENCONTRADA");
    delay (2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TENTE NOVAMENTE");
    delay(3000);
    msgInit();
    return p;

  } else {

    Serial.println("Unknown error");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("??");
    delay (3000);
    msgInit();
    return p;

  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}
//-----------------------------------------------------------------------------


void msgInit() {

  lcd.clear();

  lcd.setCursor(6, 0);
  lcd.print("PROJETO");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("CIRCUITOS  ELETRICOS");
  delay(100);
  lcd.setCursor(2, 2);
  lcd.print("SEJAM BEM VINDOS");
  delay(100);

}
