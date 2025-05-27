#include <Arduino.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "headers/variables_du_bateau.h"
#include "headers/pins.h"

#include "headers/navigation.h"

#include "headers/OuvertureGrandVoile.h"

#include "headers/imu.h"

#include "headers/gps-rtk.h"

#include "headers/regulation_safran.h"

#include "headers/girouette.h"

// ---------- prototypes des fonctions threads ----------
void Navigation(void *pvParameters);
void GPS(void *pvParameters);
void IMU(void *pvParameters);
void Girouette(void *pvParameters);
void Safran(void *pvParameters);
void Voile(void *pvParameters);
void Log(void *pvParameters);

// ---------- déclaration des tâches ----------
TaskHandle_t navigationTask, GPSTask, IMUTask, girouetteTask, safranTask, voileTask, logTask;


void setup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Initialisation du bateau");

  Wire.begin(); // Initialisation I2C master (imu et gps) /!\ Wire1 utilisé par la girouette en I2C esclave

  g_latitude_bateau = 46.0000000;
  g_longitude_bateau = -1.50000000;
  g_cap_actuel_bateau = 0;
  g_direction_vent = 180;
  
  // Définition des zones autorisées et des destinations
  // haut à droite
  g_points_gps_de_la_good_zone[0][0] = 48.0000000; g_points_gps_de_la_good_zone[0][1] = -2.0000000;
  // haut à gauche
  g_points_gps_de_la_good_zone[1][0] = 48.0000000; g_points_gps_de_la_good_zone[1][1] = -1.0000000;
  // bas à droite
  g_points_gps_de_la_good_zone[2][0] = 46.0000000; g_points_gps_de_la_good_zone[2][1] = -2.0000000;
  // bas à gauche
  g_points_gps_de_la_good_zone[3][0] = 46.0000000; g_points_gps_de_la_good_zone[3][1] = -1.0000000;

  g_points_gps_des_destinations[0][0] = 47.250946; g_points_gps_des_destinations[0][1] = -1.550389;
  g_points_gps_des_destinations[1][0] = 47.250946; g_points_gps_des_destinations[1][1] = -1.550389;
  g_points_gps_des_destinations[2][0] = 47.250946; g_points_gps_des_destinations[2][1] = -1.550389;
  g_points_gps_des_destinations[3][0] = 47.250946; g_points_gps_des_destinations[3][1] = -1.550389;

  // Lancement des tâches
  xTaskCreate(Navigation, "Navigation", 256, nullptr, 1, &navigationTask);
  xTaskCreate(GPS, "GPS", 256, nullptr, 1, &GPSTask);
  xTaskCreate(IMU, "IMU", 256, nullptr, 1, &IMUTask);
  xTaskCreate(Girouette, "Girouette", 256, nullptr, 1, &girouetteTask);
  xTaskCreate(Safran, "Safran", 256, nullptr, 1, &safranTask);
  xTaskCreate(Voile, "Voile", 256, nullptr, 1, &voileTask);
  xTaskCreate(Log, "Log", 256, nullptr, 1, &logTask);
}


void loop() { // loop has to be empty
}


// ---------- THREADS ----------

// Tâche Navigation
void Navigation(void *pvParameters)
{
  while (1)
  {
    mettre_a_jour_strategie_navigation();
    vTaskDelay(100);
  }
}

// Tâche GPS-RTK
void GPS(void *pvParameters)
{
  initGPS(); // Initialisation du GPS
  while (1)
  {
    getGPS(); // Récupération des données GPS
    vTaskDelay(10);
  }
}

// Tâche IMU
void IMU(void *pvParameters)
{
  while (1)
  {
    getIMU();
    vTaskDelay(100);
  }
}

// Tâche Girouette
void Girouette(void *pvParameters)
{
  initGirouette(); // Initialisation de la girouette
  while (1)
  {
    // printGirouette(); // Affichage des données de la girouette
    vTaskDelay(100);
  }
}

// Tâche Asservissement Safran
void Safran(void *pvParameters)
{
  initSafran(); // Initialisation du safran
  while (1)
  {
    updateSafran(); // Exécution de la régulation du safran
    vTaskDelay(DT_SAFRAN);
  }
}

// Tâche Asservissement Voile
void Voile(void *pvParameters)
{
  while (1)
  {
    GV.System_GV(g_angle_vent, g_gite_bateau); // Exécution du système GV
    vTaskDelay(100);
  }
}

// Tâche Log
void Log(void *pvParameters)
{
  while (1)
  {
    Serial.printf("cap actuel : %d\n",g_cap_actuel_bateau);
    Serial.printf("cap voulu : %d\n",g_cap_voulu_bateau);
    Serial.printf("direction absolue vent : %d\n",g_direction_vent);
    Serial.printf("angle relatif vent : %d\n",g_angle_vent);
    Serial.printf("latitude : %d\n",g_latitude_bateau);
    Serial.printf("longitude : %d\n",g_longitude_bateau);
    Serial.printf("angle GV : %d\n",GV.getAngle_GV());
    Serial.printf("gite : %d\n\n\n\n",g_gite_bateau);
    vTaskDelay(1000);
  }
}