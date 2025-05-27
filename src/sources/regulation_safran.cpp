#include "headers/regulation_safran.h"

// Création d'un objet pour le servomoteur
Servo myservo;

int pulse_min = 1070;   // Minimum pulse width in microseconds (µs)
int pulse_max = 1690;  // Maximum pulse width in microseconds (µs)
int pulse_center = (pulse_max - pulse_min) / 2 + pulse_min; // Center position

// Gain du correcteur : P & I (à ajuster pour optimiser la régulation)
float kp = 5; 
float ki = 0.1;

float errors_sum = 0;

// Offset pour la correction (pour que -90° soit 0° et +90° soit 180°)
const int offset = 90;

void initSafran(void){
  // Attache le servomoteur à la broche
  myservo.attach(PIN_SAFRAN);
}

void updateSafran(void){
  /////////////////////////////////////////////////////////////////
  ///// ----- Partie régulation / asservissement de cap ----- /////
  /////////////////////////////////////////////////////////////////
  
  // Calcul de l'erreur entre le cap de consigne et le cap actuel
  int error = g_cap_voulu_bateau - g_cap_actuel_bateau;
  
  // Limitation de l'erreur à 360 degrés (pour éviter des erreurs trop importantes)
  if (error > 180) {
    error -= 360;
  } else if (error < -180) {
    error += 360;
  }
  
  // Terme proportionnel
  int correction = (int)(kp * error) + offset;

  // Terme intégral
  errors_sum += error * DT_SAFRAN;
  correction += (int)(ki * errors_sum);
  
  // Limitation de la correction à la plage de déplacement du servomoteur (0 à 180 degrés)
  if (correction > 180) {
    correction = 180;
  } else if (correction < 0) {
    correction = 0;
  }

  // Affichage des valeurs dans le moniteur série
  /*Serial.print("Azimuth actuel : "); Serial.print(g_cap_actuel_bateau); Serial.println("°");
  Serial.print("Erreur : "); Serial.print(error); Serial.println("°");
  Serial.print("Correction : "); Serial.print(correction); Serial.println("°");*/
  
  // Calcul de la largeur d'impulsion en microsecondes en utilisant un produit en croix
  int pulseWidth = map(correction, 180, 0, pulse_min, pulse_max);

  // Envoi de la correction au servomoteur en microsecondes
  myservo.writeMicroseconds(pulseWidth);
}