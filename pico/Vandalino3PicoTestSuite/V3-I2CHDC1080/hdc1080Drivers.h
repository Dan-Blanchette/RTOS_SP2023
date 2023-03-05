/**
 * @file hdc1080Drivers.h
 * @author Dan Blanchette
 * @brief HDC1080 Header File
 * @version 0.1
 * @date 2023-02-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HDC1080_DRIVERS
#define HDC1080_DRIVERS

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx

#define HDC1080ADDRESS 0x40 // In the Data Sheet this address identifies the device
#define HDC1080_MANF_DEVICE_ID_REG 0xFE
#define TEMPERATURE 0x00
#define HUMIDITY 0x00
#define CONFIG 0x02
#define SERIAL_FIRST 0xFB
#define SERIAL_MID 0xFC
#define SERIAL_LAST 0xFD
#define DEV_ID 0xFF

#define I2C_PORT i2c1




/* ENUMERATED DATA TYPES*/
enum Temp_Type
{
   CEL = 0,
   FAHR = 1
};
enum HDC_Read_Measurements
{
   TEMP_C,
   TEMP_F,
   PERCENT_HUM
};
enum Resolution_Type
{
   HIGH = 14,
   MED = 11,
   LOW = 8
};
enum HDC_Config_Reg
{
   T_OR_H_14R = 0x00, // read the temperature and humidity at 14 bit resolution
   TEMP_11R = 0x004,  // read the temperature at 11 bit resolution
   HUMID_11R = 0x01,  // read the humidity at 11 bit resolution
   HUMID_8R = 0x02,   // read the humidity at 8 bit resolution
   BOTH_14 = 0x10,    // read both the temperature and humdity at 14 bit resolution
   BOTH_11 = 0x15,    // read both the temperature and humdity at 11 bit resolution
   RESET_VAL = 0x10,  // reset the configuration register - this cannot be read
   HEATER_1 = 0x20,   // turn heater on
   HEATER_0 = 0x10    // turn off the heater which is achieved by resetting the config register
};

/*FUNCTIONS*/

// calculate the temperature from the sensor
float temperature(enum Temp_Type, enum Resolution_Type);
// read the unique serial ID
int readSerial1(void);
int readSerial2(void);
int readSerial3(void);
// end serial ID functions


int readDeviceID(void);
void setConfig(enum HDC_Config_Reg);

int readConfig(void); // reads the bits of the config register

//read temps at 14 bit Res
float tempFahr(void);
float tempCels(void);
float calc_humidity(enum Resolution_Type);

#endif