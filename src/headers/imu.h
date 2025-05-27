#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include "variables_du_bateau.h"

// Register Function
// 0        Command register (write) / Software version (read)

// 1        Compass Bearing as a byte, i.e. 0-255 for a full circle
// 2,3      Compass Bearing as a word, i.e. 0-3599 for a full circle, representing 0-359.9 degrees. Register 2 being the high byte

// 4        Pitch angle - signed byte giving angle in degrees from the horizontal plane, Kalman filtered with Gyro
// 5        Roll angle - signed byte giving angle in degrees from the horizontal plane, Kalman filtered with Gyro

// 6,7      Magnetometer X axis raw output, 16 bit signed integer with register 6 being the upper 8 bits
// 8,9      Magnetometer Y axis raw output, 16 bit signed integer with register 8 being the upper 8 bits
// 10,11    Magnetometer Z axis raw output, 16 bit signed integer with register 10 being the upper 8 bits

// 12,13    Accelerometer  X axis raw output, 16 bit signed integer with register 12 being the upper 8 bits
// 14,15    Accelerometer  Y axis raw output, 16 bit signed integer with register 14 being the upper 8 bits
// 16,17    Accelerometer  Z axis raw output, 16 bit signed integer with register 16 being the upper 8 bits

// 18,19    Gyro X axis raw output, 16 bit signed integer with register 18 being the upper 8 bits
// 20,21    Gyro Y axis raw output, 16 bit signed integer with register 20 being the upper 8 bits
// 22,23    Gyro Z axis raw output, 16 bit signed integer with register 22 being the upper 8 bits

//---------------------------------

//Address of the CMPS12 compass on i2C
#define _i2cAddress 0x60
#define CALIBRATION_STATUS_REG 0x1E  // Registre du statut de calibration

#define CONTROL_Register 0

#define BEARING_Register 2 
#define PITCH_Register 4 
#define ROLL_Register 5

#define MAGNET_X_Register  6
#define MAGNET_Y_Register  8
#define MAGNET_Z_Register 10

#define ACCELERO_X_Register 12
#define ACCELERO_Y_Register 14
#define ACCELERO_Z_Register 16

#define _Register_GYRO_X 18
#define _Register_GYRO_Y 20
#define _Register_GYRO_Z 22

#define ONE_BYTE 1
#define TWO_BYTES 2

// Prototype des fonctions de l'IMU

void getIMU(void);
int16_t getBearing(void);
byte getPitch(void);
byte getRoll(void);
int16_t getAcceleroX(void);
int16_t getAcceleroY(void);
int16_t getAcceleroZ(void);
int16_t getMagnetX(void);
int16_t getMagnetY(void);
int16_t getMagnetZ(void);

#endif // IMU_H