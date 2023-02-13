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

#endif // _KILLER_IOT_DEVICE_MAIN_HEADER_
