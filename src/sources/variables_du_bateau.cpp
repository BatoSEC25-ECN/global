#include "headers/variables_du_bateau.h"

// Définition des variables globales

double g_points_gps_de_la_good_zone[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
double g_points_gps_des_destinations[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
int32_t g_numero_de_la_destination_actuelle = 0;

// Position du voilier autonome
double g_latitude_bateau = 0.0;
double g_longitude_bateau = 0.0;
int32_t g_altitude_bateau = 0;

// Accélération
float g_acceleration_x_bateau = 0;
float g_acceleration_y_bateau = 0;
float g_acceleration_z_bateau = 0;

// Vitesse
int32_t g_vitesse_bateau = 0;

// Gîte (inclinaison)
int8_t g_gite_bateau = 0;

// Vent
int32_t g_vitesse_vent = 0;
int32_t g_direction_vent = 0;
int32_t g_angle_vent = 0;

// Cap
int32_t g_cap_actuel_bateau = 0;
int32_t g_cap_voulu_bateau = 0;

// Instance de Grand Voile
Ouverture_Grand_Voile GV;