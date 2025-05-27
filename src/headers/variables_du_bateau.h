#ifndef VARIABLES_DU_BATEAU_H
#define VARIABLES_DU_BATEAU_H

#include <stdio.h>
#include <cstdint>

#include "OuvertureGrandVoile.h"

// Variables internes navigations
#define ANGLE_SECURITE_MINIMUM      65  // Angle sûr par rapport au vent (pour prendre de la vitesse) et atteindre le final
#define ANGLE_FACE_VENT             45  // Angle face au vent ou le bateau est dit arreté (en degrés)
#define ANGLE_REMONTE_VENT          55  // Angle pour remonter le vent (en degrés)
#define TEMPS_GEL_SEC              1      // Durée du "gel" en secondes
#define DISTANCE_SEUIL             5      // Distance seuil pour considérer que le bateau est arrivé à destination
                                          // (en mètres)
#define M_PI 3.14159265358979323846

extern double g_points_gps_de_la_good_zone[4][2]; // 4 points GPS de la zone autorisée
extern double g_points_gps_des_destinations[4][2]; // 4 points GPS des destinations
extern int32_t g_numero_de_la_destination_actuelle; // Destination actuelle 

// Variables globales pour tout le projet bateau autonome

// Variables globales pour la position du voilier autonome
extern double g_latitude_bateau; // exemple 47.0000000
extern double g_longitude_bateau;
extern int32_t g_altitude_bateau;

// Variables globales pour l'accélération
extern float g_acceleration_x_bateau; // m/sec carré
extern float g_acceleration_y_bateau; // m/sec carré
extern float g_acceleration_z_bateau; // m/sec carré

// Variable globale pour la vitesse
extern int32_t g_vitesse_bateau; // en m/sec

// Variable globale pour la gîte (inclinaison)
extern int8_t g_gite_bateau; // en degrés horizontale = 0° penche à droite 90° penche à gauche -90°

// Variables globales pour le vent
extern int32_t g_vitesse_vent; // en m/sec
extern int32_t g_direction_vent; // en degrés par rapport au nord (0° = nord, 90° = est, 180° = sud, 270° = ouest)
extern int32_t g_angle_vent; // en degrés par rapport au bateau (0° = face au vent, 90° = vent de tribord, 180° = vent arrière, 270° = vent de bâbord)

// Variables globales pour le cap
extern int32_t g_cap_actuel_bateau; // en degrés par rapport au nord (0° = nord, 90° = est, 180° = sud, 270° = ouest)
extern int32_t g_cap_voulu_bateau;  // en degrés par rapport au nord (0° = nord, 90° = est, 180° = sud, 270° = ouest)

extern Ouverture_Grand_Voile GV; // Instance de Grand Voile

#endif // VARIABLES_DU_BATEAU_H