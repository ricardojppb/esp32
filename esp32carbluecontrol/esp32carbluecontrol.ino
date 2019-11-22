#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define ENA 12 // cinza   
#define ENB 13 //laranja
#define IN1 27 // roxo
#define IN2 26 // azul
#define IN3 33 // verde
#define IN4 25 // amarelo
int speedCar = 1023;  // 0 - 1023.
float speed_Coeff = 1.5;
String command;

const int freq = 10000;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int resolution = 10;

BluetoothSerial SerialBT;

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  digitalWrite(IN1, LOW); // turn OFF
  digitalWrite(IN2, LOW); // turn OFF
  digitalWrite(IN3, LOW); // turn OFF
  digitalWrite(IN4, LOW); // turn OFF

  ledcSetup(pwmChannelA, freq, resolution);//Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits.
  ledcAttachPin(ENA, pwmChannelA);//Atribuimos o pino 23 ao canal 0
  //ledcWrite(pwmChannelA, speedCar);//Escrevemos um duty cycle de 25% no canal 0.

  ledcSetup(pwmChannelB, freq, resolution);//Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits.
  ledcAttachPin(ENB, pwmChannelB);//Atribuimos o pino 23 ao canal 0.
  //ledcWrite(pwmChannelB, speedCar);//Escrevemos um duty cycle de 25% no canal 0.

  SerialBT.begin("RicardoCarControl"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available()) {

    Serial.println(SerialBT.read());

    command = SerialBT.read();

    if (command == "70") up();
    else if (command == "66") down();
    else if (command == "76") left();
    else if (command == "82") right();
    else if (command == "73") up_right();
    else if (command == "71") up_left();
    else if (command == "74") down_right();
    else if (command == "72") down_left();
    //    else if (command == "49") speedCar = 100;
    //    else if (command == "50") speedCar = 200;
    //    else if (command == "51") speedCar = 300;
    //    else if (command == "52") speedCar = 400;
    //    else if (command == "53") speedCar = 500;
    //    else if (command == "54") speedCar = 600;
    //    else if (command == "55") speedCar = 600;
    //    else if (command == "56") speedCar = 800;
    //    else if (command == "57") speedCar = 900;
    //    else if (command == "113") speedCar = 1000;
    else if (command == "83") parar();

  }
  delay(50);
}

void up() {

  digitalWrite(IN1, HIGH); // turn on
  digitalWrite(IN2, LOW); // turn on
  ledcWrite(pwmChannelA, 1023);
  // wait
  digitalWrite(IN3, HIGH); // turn off
  digitalWrite(IN4, LOW); // turn off
  ledcWrite(pwmChannelB, 1023);
  delay(10);
  Serial.println("frente");

}

void down() {

  digitalWrite(IN1, LOW); // turn on
  digitalWrite(IN2, HIGH); // turn on
  ledcWrite(pwmChannelA, 1023);
  //delay(500);              // wait
  digitalWrite(IN3, LOW); // turn off
  digitalWrite(IN4, HIGH); // turn off
  ledcWrite(pwmChannelB, 1023);
  delay(10);
  Serial.println("traz");

}

void right() {

  digitalWrite(IN1, HIGH); // turn on
  digitalWrite(IN2, LOW); // turn on
  ledcWrite(pwmChannelA, 1023);
  //delay(500);              // wait
  digitalWrite(IN3, LOW); // turn off
  digitalWrite(IN4, HIGH); // turn off
  ledcWrite(pwmChannelB, 1023);
  delay(10);
  Serial.println("esquerda");

}

void left() {

  digitalWrite(IN1, LOW); // turn on
  digitalWrite(IN2, HIGH); // turn on
  ledcWrite(pwmChannelA, 1023);
  //delay(500);              // wait
  digitalWrite(IN3, HIGH); // turn off
  digitalWrite(IN4, LOW); // turn off
  ledcWrite(pwmChannelB, 1023);
  delay(10);
  Serial.println("direita");

}

void up_left() {

  digitalWrite(IN1, LOW); // turn on
  digitalWrite(IN2, HIGH); // turn on
  ledcWrite(pwmChannelA, 700);
  //delay(500);              // wait
  digitalWrite(IN3, HIGH); // turn off
  digitalWrite(IN4, LOW); // turn off
  ledcWrite(pwmChannelB, 1023);
  delay(10);
  Serial.println("frente-direita");

}

void up_right() {

  digitalWrite(IN1, HIGH); // turn on
  digitalWrite(IN2, LOW); // turn on
  ledcWrite(pwmChannelA, 700);
  //delay(500);              // wait
  digitalWrite(IN3, LOW); // turn off
  digitalWrite(IN4, HIGH); // turn off
  ledcWrite(pwmChannelB, 1023);
  delay(10);
  Serial.println("frente-esquerda");
}

void down_left() {

  digitalWrite(IN1, HIGH); // turn on
  digitalWrite(IN2, LOW); // turn on
  ledcWrite(pwmChannelA, 1023 );
  //delay(500);              // wait
  digitalWrite(IN3, LOW); // turn off
  digitalWrite(IN4, HIGH); // turn off
  ledcWrite(pwmChannelB, 700);
  delay(10);
  Serial.println("traz-direita");
}

void down_right() {

  digitalWrite(IN1, LOW); // turn on
  digitalWrite(IN2, HIGH); // turn on
  ledcWrite(pwmChannelA, 1023);
  //delay(500);              // wait
  digitalWrite(IN3, HIGH); // turn off
  digitalWrite(IN4, LOW); // turn off
  ledcWrite(pwmChannelB, 700);
  delay(10);
  Serial.println("traz-esquerda");
}

void parar() {

  digitalWrite(IN1, LOW); // turn on
  digitalWrite(IN2, LOW); // turn on
  ledcWrite(pwmChannelA, 0);
  //delay(500);              // wait
  digitalWrite(IN3, LOW); // turn off
  digitalWrite(IN4, LOW); // turn off
  ledcWrite(pwmChannelB, 0);
  delay(10);
  Serial.println("parar");
}
