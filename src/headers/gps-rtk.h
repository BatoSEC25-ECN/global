#ifndef GPS_RTK_H
#define GPS_RTK_H

#include <Wire.h> // Needed for I2C to GPS
#include "SparkFun_Ublox_Arduino_Library.h" // GPS library
#include "variables_du_bateau.h"

// Ajustez au besoin (port série pour XBee, débit en bauds)
#define XBEE_SERIAL   Serial2
#define XBEE_BAUD     115200
#define PRINT_PERIOD  1000   // En millisecondes

// Définir l'état du parser RTCM
enum ParserState {
  WAIT_FOR_D3,
  READ_LEN_HI,
  READ_LEN_LO,
  READ_PAYLOAD
};

// Définir la taille maximale d’un message RTCM
#define RTCM_MAX_MSG 1024
#define RTCM_MAX_LEN 1024

// Prototype des fonctions du GPS RTK
void initGPS(void);
void getGPS(void);
void printRTCMType(const uint8_t* buf, size_t len);
void resetRTCMSeen(void);
void printRTCMStatus(void);
void parseRTCMFrame(void);
void printRTCMType(const uint8_t* buffer, uint16_t len);
bool isUsefulRTCMType(uint16_t type);
void updateRTCMTypeReceived(uint16_t type);

#endif // GPS_RTK_H