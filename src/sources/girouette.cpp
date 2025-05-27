#include "headers/girouette.h"

volatile float speed = 0.0;

void initGirouette(void){
  Wire1.setSCL(PIN_GIROUETTE_SCL);
  Wire1.setSDA(PIN_GIROUETTE_SDA);
  Wire1.begin(I2C_ADDRESS_GIROUETTE);
  Wire1.onReceive(receiveEvent);
  Serial.println("🛠️ Pico 2W prêt en I2C Esclave !");
}

void receiveEvent(int howMany){
  if (howMany >= 4) {
    Wire1.read(); // Ignorer le 1er octet (command byte / register)
    uint8_t b0 = Wire1.read(); // MSB
    uint8_t b1 = Wire1.read(); // LSB
    uint8_t b2 = Wire1.read(); // Speed * 10

    g_angle_vent = (int32_t)(((uint16_t)b0 << 8) | b1); // Angle du vent en degrés par rapport au bateau
    g_direction_vent = (g_angle_vent + g_cap_actuel_bateau) % 360; // Calcul de la direction du vent par rapport au nord

    speed = b2 / 10.0;
  } else {
    while (Wire1.available()) Wire1.read(); // Nettoyage si trame partielle
  }
}

void printGirouette(void){
  Serial.print("🧭 Angle relatif vent : ");
  Serial.print(g_angle_vent);
  Serial.print(" °  | 🧭 Direction vent : ");
  Serial.print(g_direction_vent);
  Serial.print(" °  | 💨 Vitesse vent : ");
  Serial.print(speed, 1);
  Serial.println(" m/s");
}