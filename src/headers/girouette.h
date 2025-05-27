#ifndef GIROUETTE_H
#define GIROUETTE_H

#include <Wire.h>
#include "variables_du_bateau.h"
#include "pins.h"

#define I2C_ADDRESS_GIROUETTE 0x08

// Prototype des fonctions de la girouette
void initGirouette(void);
void receiveEvent(int howMany);
void printGirouette(void);

#endif // GIROUETTE_H