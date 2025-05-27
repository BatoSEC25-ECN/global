#include "headers/navigation.h"

int mettre_a_jour_strategie_navigation() {
    if (!bateau_a_destination()) {
        // On n'est pas encore arrivé à destination
        if (bateau_face_au_vent()) {
            // On est face au vent
            if (get_orientation_vent() == ORIENTATION_GAUCHE) {
                // On doit remonter le vent à gauche
                g_cap_voulu_bateau = ((g_direction_vent - 90) + 360) % 360;

            } else {
                // On doit remonter le vent à droite
                g_cap_voulu_bateau = (g_direction_vent + 90) % 360;

            }
        } else if (position_final_accessible_en_direct()) {
            // La position finale est accessible directement
            g_cap_voulu_bateau = calculer_cap_final();
        } else {
            // Le bateau n'est pas face au vent 
            // La position finale n'est pas accessible en ligne droite
            if (get_zone_du_bateau() == ZONE_GOOD) {
                // On est dans une zone autorisée
                // On doit remonter le vent
                if (get_orientation_vent() == ORIENTATION_GAUCHE) {
                    g_cap_voulu_bateau = ((g_direction_vent - ANGLE_REMONTE_VENT) + 360) % 360;
                
                } else {
                    g_cap_voulu_bateau = (g_direction_vent + ANGLE_REMONTE_VENT) % 360;

                }
            } else {
                // On est dans une zone défavorable
                g_cap_voulu_bateau = get_cap_pour_entrer_en_good_zone();
            }
        }
    } else {
        // On est arrivé à destination
        set_nouvelle_destination();
    }
    return 0;
}

double deg2rad(double degrees) {
    return degrees / 180.0 * M_PI ;
}

double rad2deg(double radians) {
    return radians / M_PI * 180.0;
}

double calculer_distance_entre_2pts_GPS(double latA, double longA, double latB, double longB) {
    // Rayon de la terre en mètres (sphère IAG-GRS80)
    const double RT = 6378137.0;

    // angle en radians entre les 2 points
    double S = acos(sin(deg2rad(latA)) * sin(deg2rad(latB)) + cos(deg2rad(latA)) * cos(deg2rad(latB)) * cos(fabs(deg2rad(longB) - deg2rad(longA))));
    
    // distance entre les 2 points, comptée sur un arc de grand cercle
    return S * RT;
}


int bateau_a_destination() {
    // Coordonnées de la destination actuelle
    double latitude_destination = g_points_gps_des_destinations[g_numero_de_la_destination_actuelle][0];
    double longitude_destination = g_points_gps_des_destinations[g_numero_de_la_destination_actuelle][1];

    // Calcul de la distance euclidienne
    double distance = calculer_distance_entre_2pts_GPS(g_latitude_bateau, g_longitude_bateau, latitude_destination, longitude_destination);

    // Retourne vrai si la distance est inférieure au seuil
    return distance < DISTANCE_SEUIL;
}

int bateau_face_au_vent() {
    return abs(g_cap_actuel_bateau - g_direction_vent) < ANGLE_FACE_VENT;
}

OrientationVent get_orientation_vent() {
    // le vent est donné par rapport au Nord, il faut donc faire conversion par rapport à la direction du bateau
    int angle_vent_bateau = (g_direction_vent - g_cap_actuel_bateau + 360) % 360;
    return (angle_vent_bateau < 180) ? ORIENTATION_GAUCHE : ORIENTATION_DROITE;
}

int position_final_accessible_en_direct() {
    // vérifier que le vent permet d'atteindre la cible en directe
    int cap = calculer_cap_final();
    if (abs(g_direction_vent - cap) < ANGLE_SECURITE_MINIMUM) {
        return 0;
    }
    return 1; // Simulation de l'accessibilité directe
}

int calculer_cap_final() {
    // Calculer le cap par rapport au Nord en degrés
    double latitude_destination = g_points_gps_des_destinations[g_numero_de_la_destination_actuelle][0];
    double longitude_destination = g_points_gps_des_destinations[g_numero_de_la_destination_actuelle][1];

    double delta_latitude = latitude_destination - g_latitude_bateau;
    double delta_longitude = longitude_destination - g_longitude_bateau;

    // Calculer l'angle en radians
    double angle_radians = atan2(delta_longitude, delta_latitude);

    // Convertir l'angle en degrés
    int angle_degrees = ((int)rad2deg(angle_radians) + 360) % 360;

    return angle_degrees;
}



// ZoneBateau get_zone_du_bateau() {
//     // Supposons que la zone autorisée est définie par un polygone de points GPS
//     // et que nous avons une fonction pour vérifier si un point est à l'intérieur d'un polygone.
//     int n = sizeof(g_points_gps_de_la_good_zone) / sizeof(g_points_gps_de_la_good_zone[0]);
//     double x = g_longitude_bateau;
//     double y = g_latitude_bateau;

//     bool inside = false;
//     for (int i = 0, j = n - 1; i < n; j = i++) {
//         double xi = g_points_gps_de_la_good_zone[i][0], yi = g_points_gps_de_la_good_zone[i][1];
//         double xj = g_points_gps_de_la_good_zone[j][0], yj = g_points_gps_de_la_good_zone[j][1];

//         bool intersect = ((yi > y) != (yj > y)) && (yj - yi != 0) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
        
//         if (intersect) {
//             inside = !inside;
//         }
//     }

//     return inside ? ZONE_GOOD : ZONE_BAD;
// }

ZoneBateau get_zone_du_bateau() {
    // Supposons que la zone autorisée est définie par un rectangle avec 4 points GPS
    // Vérifier si le bateau est à l'intérieur de ce rectangle
    bool inside = false;

    if (g_latitude_bateau <= g_points_gps_de_la_good_zone[0][0] && 
        g_latitude_bateau >= g_points_gps_de_la_good_zone[2][0] && 
        g_longitude_bateau >= g_points_gps_de_la_good_zone[0][1] && 
        g_longitude_bateau <= g_points_gps_de_la_good_zone[1][1]) 
    {
        inside = true;
    }

    return inside ? ZONE_GOOD : ZONE_BAD;
}

// calculer le centre de la zone autorisée
// définir dans quel direction est le centre par rapport au bateau
// calculer le cap pour aller vers le centre tenir compte du vent
// 
// int get_cap_pour_entrer_en_good_zone() {
//     // Calculer le centre de la zone autorisée
//     double sum_latitude = 0.0;
//     double sum_longitude = 0.0;
//     int n = sizeof(g_points_gps_de_la_good_zone) / sizeof(g_points_gps_de_la_good_zone[0]);

//     for (int i = 0; i < n; i++) {
//         sum_latitude += g_points_gps_de_la_good_zone[i][0];
//         sum_longitude += g_points_gps_de_la_good_zone[i][1];
//     }

//     double centre_latitude = sum_latitude / n;
//     double centre_longitude = sum_longitude / n;

//     // Définir dans quelle direction est le centre par rapport au bateau
//     double delta_latitude = centre_latitude - g_latitude_bateau;
//     double delta_longitude = centre_longitude - g_longitude_bateau;

//     // Calculer le cap pour aller vers le centre en tenant compte du vent
//     double angle_radians = atan2(delta_longitude, delta_latitude);
//     int cap_centre = ((int)rad2deg(angle_radians) + 360) % 360;

//     // Ajuster le cap en fonction du vent 
//     // si le vent permet le direct, on ne change pas le cap
//     if (abs(g_direction_vent - cap_centre) < ANGLE_SECURITE_MINIMUM) {
//         if (get_orientation_vent() == ORIENTATION_GAUCHE) {
//             cap_centre = ((g_direction_vent - ANGLE_REMONTE_VENT) + 360) % 360;
        
//         } else {
//             cap_centre = (g_direction_vent + ANGLE_REMONTE_VENT) % 360;

//         }
//     }

//     return cap_centre;
// }


// // on regarde si la cap voulu bateau de l'ancien round te permet de rentrer dans la zone, si ce b'est pas le cas
// // on fait * -1 sur la direction du voulu
int get_cap_pour_entrer_en_good_zone() {

    double sum_latitude = 0.0;
    double sum_longitude = 0.0;
    int n = sizeof(g_points_gps_de_la_good_zone) / sizeof(g_points_gps_de_la_good_zone[0]);

    for (int i = 0; i < n; i++) {
        sum_latitude += g_points_gps_de_la_good_zone[i][0];
        sum_longitude += g_points_gps_de_la_good_zone[i][1];
    }

    double centre_latitude = sum_latitude / n;
    double centre_longitude = sum_longitude / n;
    double actual_distance = abs(calculer_distance_entre_2pts_GPS(g_latitude_bateau, g_longitude_bateau, centre_latitude, centre_longitude));

    // calculer la distance si le cap est maintenu
    double next_latitude = g_latitude_bateau + 0.0001 * cos(deg2rad(g_cap_voulu_bateau));
    double next_longitude = g_longitude_bateau + 0.0001 * sin(deg2rad(g_cap_voulu_bateau));
    double next_distance = abs(calculer_distance_entre_2pts_GPS(next_latitude, next_longitude, centre_latitude, centre_longitude));

    // calculer la distance si le cap est diminué
    int32_t cap_bateau_dec = ((g_cap_voulu_bateau - 2 * ANGLE_SECURITE_MINIMUM) + 360) % 360;
    double next_latitude_dec = g_latitude_bateau + 0.0001 * cos(deg2rad(cap_bateau_dec));
    double next_longitude_dec = g_longitude_bateau + 0.0001 * sin(deg2rad(cap_bateau_dec));
    double next_distance_dec = abs(calculer_distance_entre_2pts_GPS(next_latitude_dec, next_longitude_dec, centre_latitude, centre_longitude));

    // calculer la distance si le cap est augmenté
    int32_t cap_bateau_inc = (g_cap_voulu_bateau + 2 * ANGLE_SECURITE_MINIMUM) % 360;
    double next_latitude_inc = g_latitude_bateau + 0.0001 * cos(deg2rad(cap_bateau_inc));
    double next_longitude_inc = g_longitude_bateau + 0.0001 * sin(deg2rad(cap_bateau_inc));
    double next_distance_inc = abs(calculer_distance_entre_2pts_GPS(next_latitude_inc, next_longitude_inc, centre_latitude, centre_longitude));

    // Si le point suivant est plus éloigné, ajuster le cap
    if (next_distance >= actual_distance) {
        if (next_distance_dec < next_distance) {
            while (abs(g_direction_vent - cap_bateau_dec) < ANGLE_SECURITE_MINIMUM) {
                cap_bateau_dec = ((cap_bateau_dec - 5) + 360) % 360;
            }
            g_cap_voulu_bateau = cap_bateau_dec;
        } else if (next_distance_inc < next_distance) {
            while (abs(g_direction_vent - cap_bateau_dec) < ANGLE_SECURITE_MINIMUM) {
                cap_bateau_inc = (cap_bateau_inc + 5) % 360;
            }
            g_cap_voulu_bateau = cap_bateau_inc;
        }
    } else if (next_distance_dec < actual_distance) {
        while (abs(g_direction_vent - cap_bateau_dec) < ANGLE_SECURITE_MINIMUM) {
            cap_bateau_dec = ((cap_bateau_dec - 5) + 360) % 360;
        }
        g_cap_voulu_bateau = cap_bateau_dec;
    } else if (next_distance_inc < actual_distance) {
        while (abs(g_direction_vent - cap_bateau_dec) < ANGLE_SECURITE_MINIMUM) {
            cap_bateau_inc = (cap_bateau_inc + 5) % 360;
        }
        g_cap_voulu_bateau = cap_bateau_inc;
    }
    
    return g_cap_voulu_bateau;
}

void set_nouvelle_destination() {
    g_numero_de_la_destination_actuelle = (g_numero_de_la_destination_actuelle + 1) % sizeof(g_points_gps_des_destinations);
}

