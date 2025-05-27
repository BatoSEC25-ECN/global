#ifndef REGULATION_SAFRAN_H
#define REGULATION_SAFRAN_H

#include <Servo.h>

#include "variables_du_bateau.h"
#include "pins.h"

// période d'échantillonnage
#define DT_SAFRAN 50

// Prototype des fonctions du safran
void initSafran(void);
void updateSafran(void);

#endif // REGULATION_SAFRAN_H