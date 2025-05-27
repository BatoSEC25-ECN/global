#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <stdlib.h>
#include <math.h>
#include "variables_du_bateau.h"

// Définition des énumérations pour l'orientation du vent et la zone du bateau
typedef enum {
    ORIENTATION_GAUCHE,
    ORIENTATION_DROITE
} OrientationVent;

typedef enum {
    ZONE_GOOD,
    ZONE_BAD
} ZoneBateau;

// Déclaration des fonctions de navigation
int mettre_a_jour_strategie_navigation();
int bateau_a_destination();
int bateau_face_au_vent();
OrientationVent get_orientation_vent();
int position_final_accessible_en_direct();
int calculer_cap_final();
ZoneBateau get_zone_du_bateau();
int get_cap_pour_entrer_en_good_zone();
void set_nouvelle_destination();
double deg2rad(double degrees);
double rad2deg(double radians);
double calculer_distance_entre_2pts_GPS(double latA, double longA, double latB, double longB);

#endif // NAVIGATION_H
