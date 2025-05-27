#include "headers/gps-rtk.h"

SFE_UBLOX_GPS myGPS;

/////////////nouveux params////////////////

static ParserState rtcmState = WAIT_FOR_D3;
static uint8_t rtcmBuffer[RTCM_MAX_MSG];
static uint16_t bytesToRead = 0;
static uint16_t idx = 0;

// Suivi des trames RTCM reçues
bool rtcmSeen[1300] = {false};
unsigned long lastRTCMReset = 0;

// Types critiques requis
const uint16_t requiredRTCM[] = {1006, 1074, 1087};
bool requiredReceived[sizeof(requiredRTCM)/sizeof(uint16_t)] = {false};
bool allRequiredReceived = false;

bool seen1005 = false;
bool seen1074 = false;
bool seen1084 = false;
bool seen1094 = false;
bool seen1230 = false;

/////////////nouveux params////////////////

void initGPS(void)
{
  XBEE_SERIAL.begin(XBEE_BAUD, SERIAL_8N1);

  // myGPS.enableDebugging(Serial);

  while (myGPS.begin(Wire) == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.setNavigationFrequency(20); //Set output to 20 times a second

  byte rate = myGPS.getNavigationFrequency(); //Get the update rate of this module
  Serial.print("Current update rate: ");
  Serial.println(rate);

  //myGPS.saveConfiguration(); //Save the current settings to flash and BBR
}




void getGPS(void)
{
  // 1) Lire UN seul octet du XBee, s'il est dispo :
  if (XBEE_SERIAL.available() > 0)
  {
    uint8_t b = XBEE_SERIAL.read();
    // Serial.print("gestion xbee");
    // Serial.print(b);
    // // Envoyer l'octet au module GNSS :
    // // (selon votre version de la lib : pushRawData(buffer, taille))
    myGPS.pushRawData(&b, 1);
    // parseRTCMFrame();
  }

  // 2) Faire un "update" du GNSS 
  // (Dans les nouvelles versions : myGPS.checkUblox(Wire), checkCallbacks())
  // Mais dans votre version, probablement :
  myGPS.checkUblox();
 
  // First, let's collect the position data
  int32_t latitude = myGPS.getHighResLatitude();
  int8_t latitudeHp = myGPS.getHighResLatitudeHp();
  int32_t longitude = myGPS.getHighResLongitude();
  int8_t longitudeHp = myGPS.getHighResLongitudeHp();
  int32_t ellipsoid = myGPS.getElipsoid();
  int8_t ellipsoidHp = myGPS.getElipsoidHp();
  int32_t msl = myGPS.getMeanSeaLevel();
  int8_t mslHp = myGPS.getMeanSeaLevelHp();
  uint32_t accuracy = myGPS.getHorizontalAccuracy();

  // Defines storage for the lat and lon units integer and fractional parts
  int32_t integer_latitude_bateau; // Integer part of the latitude in degrees
  int32_t lat_frac; // Fractional part of the latitude
  int32_t integer_longitude_bateau; // Integer part of the longitude in degrees
  int32_t lon_frac; // Fractional part of the longitude

  // Calculate the latitude and longitude integer and fractional parts
  integer_latitude_bateau = latitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
  int32_t un_lat_frac = latitude - (integer_latitude_bateau  * 10000000); // Calculate the fractional part of the latitude
  lat_frac = (un_lat_frac * 100) + latitudeHp; // Now add the high resolution component
  if (lat_frac < 0) // If the fractional part is negative, remove the minus sign
  {
    lat_frac = 0 - lat_frac;
  }
  if (integer_latitude_bateau < 0){
    lat_frac = -lat_frac;
  }
  g_latitude_bateau = (double)integer_latitude_bateau + ((double)lat_frac / 1e9);

  integer_longitude_bateau = longitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
  lon_frac = longitude - (integer_longitude_bateau * 10000000); // Calculate the fractional part of the longitude
  lon_frac = (lon_frac * 100) + longitudeHp; // Now add the high resolution component
  if (lon_frac < 0) // If the fractional part is negative, remove the minus sign
  {
    lon_frac = 0 - lon_frac;
  }
  if (integer_longitude_bateau < 0){
    lon_frac = -lon_frac;
  }
  g_longitude_bateau = (double)integer_longitude_bateau + ((double)lon_frac / 1e9);
  
  // Print the lat and lon
  Serial.print("Lat (deg): ");
  Serial.print(integer_latitude_bateau); // Print the integer part of the latitude
  Serial.print(".");
  Serial.print(lat_frac); // Print the fractional part of the latitude
  Serial.print(", test lat (deg): ");
  Serial.print(g_latitude_bateau, 9); // Print the integer part of the latitude
  Serial.print(", Lon (deg): ");
  Serial.print(integer_longitude_bateau); // Print the integer part of the latitude
  Serial.print(".");
  Serial.println(abs(lon_frac)); // Print the fractional part of the latitude
  Serial.print(", test long (deg): ");
  Serial.print(g_longitude_bateau, 9); // Print the integer part of the latitude

  // Now define float storage for the heights and accuracy
  float f_ellipsoid;
  float f_msl;
  float f_accuracy;

  // Calculate the height above ellipsoid in mm * 10^-1
  f_ellipsoid = (ellipsoid * 10) + ellipsoidHp;
  // Now convert to m
  f_ellipsoid = f_ellipsoid / 10000.0; // Convert from mm * 10^-1 to m

  // Calculate the height above mean sea level in mm * 10^-1
  f_msl = (msl * 10) + mslHp;
  // Now convert to m
  g_altitude_bateau = f_msl / 10000.0; // Convert from mm * 10^-1 to m

  // Convert the horizontal accuracy (mm * 10^-1) to a float
  f_accuracy = accuracy;
  // Now convert to m
  f_accuracy = f_accuracy / 10000.0; // Convert from mm * 10^-1 to m

  // Finally, do the printing
  /*Serial.print("Ellipsoid (m): ");
  Serial.print(f_ellipsoid, 4); // Print the ellipsoid with 4 decimal places

  Serial.print(", Mean Sea Level(m): ");
  Serial.print(g_altitude_bateau, 4); // Print the mean sea level with 4 decimal places

  Serial.print(", Accuracy (m): ");
  Serial.println(f_accuracy, 4); // Print the accuracy with 4 decimal places
*/
      uint8_t fixType = myGPS.getFixType();
  uint8_t carrierSolution = myGPS.getCarrierSolutionType();

  /*Serial.print("Fix Type: ");
  switch (fixType)
  {
    case 0: Serial.print("No Fix"); break;
    case 1: Serial.print("Dead Reckoning"); break;
    case 2: Serial.print("2D Fix"); break;
    case 3: Serial.print("3D Fix"); break;
    case 4: Serial.print("GNSS + Dead Reckoning"); break;
    case 5: Serial.print("Time Only"); break;
    default: Serial.print("Unknown"); break;
  }

  Serial.print(", RTK Status: ");
  switch (carrierSolution)
  {
    case 0: Serial.println("No RTK"); break;
    case 1: Serial.println("RTK Float"); break;
    case 2: Serial.println("RTK Fixed"); break;
    default: Serial.println("Unknown"); break;
  }*/

  // => Fin de loop(), on y retourne immédiatement.  
  // Si d’autres octets XBee sont arrivés, on les lira au prochain tour,
  // tout en relançant checkUblox() à chaque fois.
}


void printRTCMType(const uint8_t* buf, size_t len) {
  if (len < 6) return;
  uint16_t type = ((buf[3] & 0xFC) << 4) | ((buf[4] & 0xF0) >> 4);
  Serial.print("RTCM Type: ");
  Serial.println(type);

  if (type < 1300) rtcmSeen[type] = true;

  for (size_t i = 0; i < sizeof(requiredRTCM)/sizeof(uint16_t); i++) {
    if (type == requiredRTCM[i]) {
      requiredReceived[i] = true;
    }
  }

  allRequiredReceived = true;
  for (size_t i = 0; i < sizeof(requiredRTCM)/sizeof(uint16_t); i++) {
    if (!requiredReceived[i]) {
      allRequiredReceived = false;
      break;
    }
  }

  if (allRequiredReceived) {
    Serial.println("✅ Tous les types RTCM requis ont été reçus.");
  }
}

void resetRTCMSeen(void) {
  for (int i = 0; i < 1300; i++) rtcmSeen[i] = false;
  for (size_t i = 0; i < sizeof(requiredRTCM)/sizeof(uint16_t); i++) requiredReceived[i] = false;
  allRequiredReceived = false;
  lastRTCMReset = millis();
}

void printRTCMStatus(void) {
  Serial.println("[RTCM Types vus récemment]");
  for (int i = 0; i < 1300; i++) {
    if (rtcmSeen[i]) {
      Serial.print(i);
      Serial.print(" ");
    }
  }
  Serial.println();

  Serial.print("Reçus requis: ");
  for (size_t i = 0; i < sizeof(requiredRTCM)/sizeof(uint16_t); i++) {
    Serial.print(requiredRTCM[i]);
    Serial.print(requiredReceived[i] ? ":✅ " : ":❌ ");
  }
  Serial.println();
}

void parseRTCMFrame(void) {
  while (XBEE_SERIAL.available()) {
    uint8_t b = XBEE_SERIAL.read();

    switch(rtcmState) {
      case WAIT_FOR_D3:
        if (b == 0xD3) {
          idx = 0;
          rtcmBuffer[idx++] = b;
          rtcmState = READ_LEN_HI;
        }
        break;

      case READ_LEN_HI:
        rtcmBuffer[idx++] = b;
        bytesToRead = (b & 0x3F) << 8;
        rtcmState = READ_LEN_LO;
        break;

      case READ_LEN_LO:
        rtcmBuffer[idx++] = b;
        bytesToRead |= b;
        if (bytesToRead > RTCM_MAX_MSG - 6) {
          rtcmState = WAIT_FOR_D3;
        } else {
          bytesToRead += 3; // CRC
          rtcmState = READ_PAYLOAD;
        }
        break;

      // case READ_PAYLOAD:
      //   rtcmBuffer[idx++] = b;
      //   bytesToRead--;
      //   if (bytesToRead == 0) {
      //     printRTCMType(rtcmBuffer, idx);
      //     if (allRequiredReceived) {
      //       myGPS.pushRawData(rtcmBuffer, idx);
      //     }
      //     rtcmState = WAIT_FOR_D3;
      //     idx = 0;
      //   }
      //   break;
      case READ_PAYLOAD:
        rtcmBuffer[idx++] = b;
        bytesToRead--;
        if (bytesToRead == 0) {
          // Trame complète reçue

          // 🔍 Extraire le type RTCM (les bits 6 à 17, soit octet 3 & 4)
          uint16_t rtcmType = ((uint16_t)(rtcmBuffer[3] & 0xFC) << 4) | ((rtcmBuffer[4] & 0xF0) >> 4);

          // 🖨️ Affichage optionnel pour debug
          Serial.print("RTCM Type: ");
          Serial.println(rtcmType);

          // ✅ Mettre à jour la checklist des types nécessaires
          updateRTCMTypeReceived(rtcmType); // ↪️ À définir plus haut

          // 🚦Pousser au GPS si trame pertinente
          if (isUsefulRTCMType(rtcmType)) {
            myGPS.pushRawData(rtcmBuffer, idx);
          }

          // 🔄 Réinitialisation de l'état du parseur
          rtcmState = WAIT_FOR_D3;
          idx = 0;
          bytesToRead = 0;
        }
        break;

    }
  }

  if (millis() - lastRTCMReset > 5000) {
    printRTCMStatus();
    resetRTCMSeen();
  }
}



// Affiche le type de message RTCM reçu (ex: 1005, 1077, 1230, ...)
void printRTCMType(const uint8_t* buffer, uint16_t len) {
  if (len < 6 || buffer[0] != 0xD3) return;

  uint16_t header = ((buffer[3] & 0x03) << 8) | buffer[4];
  Serial.print("RTCM Type: ");
  Serial.println(header);
}

bool isUsefulRTCMType(uint16_t type) {
  // Liste des types utiles à garder
  return (type == 1005 || type == 1074 || type == 1084 || type == 1094 || type == 1230);
}

void updateRTCMTypeReceived(uint16_t type) {
  if (type == 1005) seen1005 = true;
  if (type == 1074) seen1074 = true;
  if (type == 1084) seen1084 = true;
  if (type == 1094) seen1094 = true;
  if (type == 1230) seen1230 = true;

  allRequiredReceived = seen1005 && seen1074 && seen1084 && seen1094 && seen1230;
}