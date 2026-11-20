#include "DFRobot_BloodOxygen_S.h"

#define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked

#ifdef I2C_COMMUNICATION
#define I2C_ADDRESS 0x57
TwoWire I2C_2 = TwoWire(1);
DFRobot_BloodOxygen_S_I2C MAX30102(&I2C_2, I2C_ADDRESS);
#endif


#include "Wire.h"
#define I2C_DEV_ADDR 0x55

// =====================================
// TABLE DES CODES ERREUR
// =====================================

#define ERR_NONE              0
#define ERR_WIFI_LOST         1
#define ERR_SENSOR_DISCONNECT 2
#define ERR_SENSOR_READ       3
#define ERR_TEMP_LIMIT        4
#define ERR_LOW_VOLTAGE       5
#define ERR_SERIAL_LOST       6
#define ERR_LOW_MEMORY        7
#define ERR_I2C               8
#define ERR_WATCHDOG           9
#define ERR_UNKNOWN           10

// Code erreur actuel
uint8_t errorCode = ERR_NONE;


// =====================================
// FONCTION DE RECEPTION I2C
// =====================================

void onReceive(int len) {

  Serial.printf("Commande reçue du Master : ");

  while (Wire.available()) {
    Serial.write(Wire.read());
  }

  Serial.println();
}


// =====================================
// FONCTION D'ENVOI I2C
// =====================================

void onRequest() {

  Wire.write(errorCode);

  Serial.print("Code erreur transmis : ");
  Serial.println(errorCode);
}


// =====================================
// INITIALISATION
// =====================================

void setup() {

  Serial.begin(115200);
  I2C_2.begin(25, 26, 100000);
  while (false == MAX30102.begin()) {
    Serial.println("init fail!");
    delay(1000);
  }
  Serial.println("init success!");
  Serial.println("start measuring...");
  MAX30102.sensorStartCollect();

  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

  Wire.begin((uint8_t)I2C_DEV_ADDR);

  Serial.println("ESP32 Slave initialise");
}


// =====================================
// BOUCLE PRINCIPALE
// =====================================

void loop() {


MAX30102.getHeartbeatSPO2();
  Serial.print("SPO2 is : ");
  Serial.print(MAX30102._sHeartbeatSPO2.SPO2);
  Serial.println("%");
  Serial.print("heart rate is : ");
  Serial.print(MAX30102._sHeartbeatSPO2.Heartbeat);
  Serial.println("Times/min");
  Serial.print("Temperature value of the board is : ");
  Serial.print(MAX30102.getTemperature_C());
  Serial.println(" ℃");
  //The sensor updates the data every 4 seconds
  delay(4000);
  //Serial.println("stop measuring...");
  //MAX30102.sensorEndCollect();


  // SIMULATION : changement de code toutes les 5 secondes
  static unsigned long previousMillis = 0;

  if (millis() - previousMillis >= 5000) {

    previousMillis = millis();

    errorCode++;

    if (errorCode > ERR_UNKNOWN) {
      errorCode = ERR_NONE;
    }

    Serial.print("Nouvelle erreur : ");
    Serial.println(errorCode);
  }
}
