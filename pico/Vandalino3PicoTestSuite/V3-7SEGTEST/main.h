/*
 * killer_iot_device for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      Dan Blanchette
 * @copyright   2023
 * @licence     MIT
 *
 */
#ifndef _KILLER_IOT_DEVICE_MAIN_HEADER_
#define _KILLER_IOT_DEVICE_MAIN_HEADER_

/*
   HDC 1080 Drivers Header
*/
// #include "hdc1080Drivers.h"


/*
 * C HEADERS
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * PICO HEADERS
 */
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/uart.h"

/*
   FreeRTOS HEADERS
*/
#include <FreeRTOS.h>
#include <task.h>
#include "semphr.h"
#include <queue.h>


/********************************
 * 						  			  *
 * SEVEN SEGMENT PIN ASSIGNMENT *
 * 						  			  *
 ********************************/
// GPIO pin setup GLOBAL VALUES
#define SevenSegCC1 11
#define SevenSegCC2 10

#define SevenSegA 26
#define SevenSegB 27
#define SevenSegC 29
#define SevenSegD 18
#define SevenSegE 25
#define SevenSegF 7
#define SevenSegG 28
#define SevenSegDP 24

// Buttons PIN assignments
#define BUTTON1 19
#define BUTTON2 9
#define BUTTON3 8




/***********************
 * 						  *
 * FUNCTION PROTOTYPES *
 * 						  *
 ***********************/
// setup 7seg I/O
void setup_7seg();

// setup buttons
void setup_buttons();

// setup I2C HDC1080
void setup_HDC1080();

// function to draw the numbers on the 7-seg display
void draw_numbers(const int );
void draw_hex_val(int rem);

#endif // _KILLER_IOT_DEVICE_MAIN_HEADER_
