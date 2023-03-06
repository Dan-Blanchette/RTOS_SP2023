/**
 * @file main.c
 * @author Dan Blanchette
 * @brief  This program will count down from 42 to 00 and back up from 00 to 42 using a 7-segment display.
 * 		  As an additional feature, the D13 LED on the Pico Feather will be synced with the second count.
 * @version 0.1
 * @date Started: 2023-02-3, Due: 2023-02-8
 * Total Hours: 21 (coding and research)
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
#include "hdc1080Drivers.h"

/*PICO PIN SETUP*/
// D13 Pin Assignment pico
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

/*SEMAPHORES*/
// Semaphore intilizaition
SemaphoreHandle_t xSem;

/***********************
 *                     *
 *       QUEUES        *
 *                     *
 ***********************/
// digit queues for the 7-seg display
static QueueHandle_t xQueueDisp = NULL;
// pico D13 Blink
static QueueHandle_t xQueuePico = NULL;

/***********************
 *                     *
 *       TASKS         *
 *                     *
 ***********************/

// PRIORITY 5
void task_buttons()
{
   while (1)
   {
      int button1, button2, button3;
      button1 = gpio_get(BUTTON1);
      button2 = gpio_get(BUTTON2);
      button3 = gpio_get(BUTTON3);

      printf("button1/2/3 %d%d%d\n", button1, button2, button3);
      vTaskDelay(100 / portTICK_PERIOD_MS);
   }
}

// Starts at draws the 0-9 count, 9-0 count on the right display
// PRIORITY: 4
void task_right_disp()
{

   while (1)
   {
      // Take the semaphore
      xSemaphoreTake(xSem, portMAX_DELAY);
      // printf("In right display\n");
      gpio_put(SevenSegCC1, 1);
      gpio_put(SevenSegCC2, 0);
      int rec_val = 0;
      xQueuePeek(xQueueDisp, &rec_val, portMAX_DELAY);
      int right_num = (rec_val % 10);

      if (right_num < 10)
      {
         // draw the numbers to the display
         draw_numbers(right_num);
      }

      vTaskDelay(10 / portTICK_PERIOD_MS);
      xSemaphoreGive(xSem);
      vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}

// counts 0-4 then 4-0
// PRIORITY: 5
void task_left_disp()
{
   while (1)
   {
      xSemaphoreTake(xSem, portMAX_DELAY);
      // printf("In left display\n");
      gpio_put(SevenSegCC1, 0);
      gpio_put(SevenSegCC2, 1);

      int rec_val = 0;

      // extract the digit in the buffer
      xQueuePeek(xQueueDisp, &rec_val, portMAX_DELAY);
      // divide the values from the queue by 10
      int left_num = (rec_val / 10);
      // draw the number to display

      draw_numbers(left_num);

      vTaskDelay(10 / portTICK_PERIOD_MS);
      xSemaphoreGive(xSem);
      vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}
// increment the count and pass the values to a queue
// PRIORITY: 4
void task_count()
{
   int val_to_send;
   int bin_val_to_send = 1;
   // decrement counter
   for (int i = 42; i >= 0; i--)
   {

      // printf("decrementing %d\n", i);

      val_to_send = i;
      bin_val_to_send;
      // send the incremented value to the queue buffer
      xQueueSend(xQueueDisp, &val_to_send, 0U);
      xQueueSend(xQueuePico, &bin_val_to_send, 0U);
      vTaskDelay(500 / portTICK_PERIOD_MS);

      xQueueReceive(xQueueDisp, &val_to_send, 0U);
      // the counter is 00
      if (i == 0)
      {
         // increment the count
         for (i; i <= 42; i++)
         {
            // printf("incrementing %d\n", i);
            val_to_send = i;
            bin_val_to_send;
            // send the incremented value to the queue buffer
            xQueueSend(xQueueDisp, &val_to_send, 0U);
            xQueueSend(xQueuePico, &bin_val_to_send, 0U);
            vTaskDelay(500 / portTICK_PERIOD_MS);

            // clear the queue when done
            xQueueReceive(xQueueDisp, &val_to_send, 0U);
         }
      }
   }
   vTaskDelay(10 / portTICK_PERIOD_MS);
}

/**
 * @brief HUMIDITY SENSOR TASK
 *
 */
void task_humiditySensor()
{
   while (1)
   {
      int deviceID, serialID1, serialID2, serialID3, config;
      float tempF, tempC, perH;
      deviceID = readDeviceID();
      serialID1 = readSerial1();
      serialID2 = readSerial2();
      serialID3 = readSerial3();
      config = readConfig();
      tempC = tempCels();
      tempF = tempFahr();
      perH = calc_humidity(HIGH);
      // printf("HDC1080_device: ID=0x%X\n", deviceID);
      // printf("Unique_serial:  ID_1=0x%X, ID_2=0x%X, ID_3=0x%X\n", serialID1, serialID2, serialID3);
      // printf("Config Register: 0x%X\n\n", config);
      printf("Temp Farhenheit: %f\n", tempF);
      printf("Temp Celsius: %f\n\n", tempC);
      printf("Percent Humidity: %f\n\n", perH);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      // RTOS Scheduler() Here
   }
}

// blink pico
void task_pico_blink()
{
   while (1)
   {
      int val = 0;

      xQueueReceive(xQueuePico, &val, portMAX_DELAY);
      // 	printf("what is %d\n", val);
      int que_val = val;
      // printf("the value %d\n", que_val);

      if (que_val == 1)
      {
         gpio_put(LED_PIN, 1);
         vTaskDelay(100 / portTICK_PERIOD_MS);
         gpio_put(LED_PIN, 0);
      }
      else
      {
         gpio_put(LED_PIN, 0);
      }
   }
}

/***********************
 *                     *
 *   MAIN DRIVER       *
 *                     *
 ***********************/
int main()
{
   // setup 7-seg GPIO OUT
   setup_7seg();

   // setup the buttons
   setup_buttons();

   // Init Pico
   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   // Use for debugging
   stdio_init_all(); // Initialize STD I/O for printing over serial

   // setup HDC1080
   setup_HDC1080();

   /************** QUEUES AND SEMAPHORES ******************/

   // creates Queue NOTE: like malloc specify data type
   xQueueDisp = xQueueCreate(1, sizeof(int));
   // create Pico Queue (has two values 0 or 1)
   xQueuePico = xQueueCreate(1, sizeof(int));
   // NOTE:create binary semaphore before tasks but after init!
   xSem = xSemaphoreCreateBinary();
   // take flag from semaphore
   xSemaphoreGive(xSem);

   /**************************** TASKS ************************************/

   xTaskCreate(task_buttons, "Task_Buttons", 256, NULL, 5, NULL);
   xTaskCreate(
       task_right_disp,   // fucntion to be called
       "Task_Right_Disp", // Name of Task
       256,               // Stack Size
       NULL,              // Parameter to pass to a function
       4,                 // Task Priority (0 to configMAX_PRIORITIES - 1)
       NULL               // Task handle (check on status, watch memory usage, or end the task)
   );
   xTaskCreate(task_left_disp, "Task_Left_Disp", 256, NULL, 4, NULL);
   xTaskCreate(task_count, "Task_Count", 256, NULL, 3, NULL);
   xTaskCreate(task_pico_blink, "Task_Pico_Blink", 256, NULL, 2, NULL);
   xTaskCreate(task_humiditySensor, "Task_HumiditySensor", 256, NULL, 1, NULL);
   // tell the scheduler to start running
   vTaskStartScheduler();

   while (1)
   {
   }
}

/***********************
 *                     *
 *     FUNCTIONS       *
 *                     *
 ***********************/

/**
 * @brief Set the up 7seg display
 *
 */
void setup_7seg()
{
   // initialize digital pin LED_BUILTIN as an output.
   gpio_init(SevenSegA);
   gpio_init(SevenSegB);
   gpio_init(SevenSegC);
   gpio_init(SevenSegD);
   gpio_init(SevenSegE);
   gpio_init(SevenSegF);
   gpio_init(SevenSegG);
   // This GPIO pin activates the decimal point on the 7 segment display
   gpio_init(SevenSegDP);

   gpio_init(SevenSegCC1);
   gpio_init(SevenSegCC2);

   gpio_set_dir(SevenSegA, GPIO_OUT);
   gpio_set_dir(SevenSegB, GPIO_OUT);
   gpio_set_dir(SevenSegC, GPIO_OUT);
   gpio_set_dir(SevenSegD, GPIO_OUT);
   gpio_set_dir(SevenSegE, GPIO_OUT);
   gpio_set_dir(SevenSegF, GPIO_OUT);
   gpio_set_dir(SevenSegG, GPIO_OUT);
   gpio_set_dir(SevenSegDP, GPIO_OUT);

   gpio_set_dir(SevenSegCC1, GPIO_OUT);
   gpio_set_dir(SevenSegCC2, GPIO_OUT);
}

/**
 * @brief Set the up V3 buttons
 *
 */
void setup_buttons()
{
   // initialize digital pin LED_BUILTIN as an output.
   gpio_init(BUTTON1);
   gpio_init(BUTTON2);
   gpio_init(BUTTON3);

   gpio_set_dir(BUTTON1, GPIO_IN);
   gpio_set_dir(BUTTON2, GPIO_IN);
   gpio_set_dir(BUTTON3, GPIO_IN);
}

/**
 * @brief Set the up HDC1080 Sensor
 *
 */
void setup_HDC1080()
{
   i2c_init(I2C_PORT, 100 * 1000);
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

   // Make the I2C pins available to picotool
   bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
   sleep_ms(1000);
}

/**
 * @brief send binary information to enable each segment
 * and draw the numbers
 *
 * @param segNum
 */
void draw_numbers(const int segNum)
{
   switch (segNum)
   {
   case 0:
      /*
         A
         ___        __
      F | G | B    |  |
      E |___| C    |__|
          D
      */
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 0);
      break;

   case 1:

      // display #1
      /*
         | B
         | C
      */
      gpio_put(SevenSegA, 0);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 0);
      gpio_put(SevenSegE, 0);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 0);
      break;

   case 2:
      // display #2 on the right segment
      /* __
         __|
        |__
            */
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 0);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 1);
      break;
   case 3:
      // display #3 on the right segment
      /* __
         __|
         __|
            */
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 0);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 1);
      break;
   case 4:
      // display #4 on the right segment
      /*
         |__|
            |
            */
      gpio_put(SevenSegA, 0);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 0);
      gpio_put(SevenSegE, 0);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;
   case 5:
      // display #5 on the right segment
      /*  __
         |__
          __|*/
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 0);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 0);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;
   case 6:
      // display #6 on the right segment
      /*  __
         |__
         |__|*/
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 0);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;
   case 7:
      // display #7 on the right segment
      /*
          __
            |
            |
      */
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 0);
      gpio_put(SevenSegE, 0);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 0);
      break;
   case 8:
      // display #8 on the right segment
      /*  __
         |__|
         |__|*/
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;
   case 9:
      // display #9 on the right segment
      /*  __
         |__|
            |
            */
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 0);
      gpio_put(SevenSegE, 0);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;

   default:
      // this is for debug purposes
      printf("Please enter a value between 0-9");
   }
}

/**
 * @brief if the values being calculated are remainders
 * 10 - 15 substitute the hex values instead of decimal
 * @param rem
 */
void draw_hex_val(const int rem)
{
   switch (rem)
   {
   // draw  a
   case 10:
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 1);
      break;

   // draw b
   case 11:
      gpio_put(SevenSegA, 0);
      gpio_put(SevenSegB, 0);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;

   // draw c
   case 12:
      gpio_put(SevenSegA, 0);
      gpio_put(SevenSegB, 0);
      gpio_put(SevenSegC, 0);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 1);
      break;

   // draw d
   case 13:
      gpio_put(SevenSegA, 0);
      gpio_put(SevenSegB, 1);
      gpio_put(SevenSegC, 1);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 0);
      gpio_put(SevenSegG, 1);
      break;

   // draw e
   case 14:
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 0);
      gpio_put(SevenSegC, 0);
      gpio_put(SevenSegD, 1);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;

   // draw f
   case 15:
      gpio_put(SevenSegA, 1);
      gpio_put(SevenSegB, 0);
      gpio_put(SevenSegC, 0);
      gpio_put(SevenSegD, 0);
      gpio_put(SevenSegE, 1);
      gpio_put(SevenSegF, 1);
      gpio_put(SevenSegG, 1);
      break;

   default:
      draw_numbers(rem);
      break;
   }
}

/**
 * @brief
 *
 * @return float
 */
float temperature(enum Temp_Type degrees, enum Resolution_Type resolution)
{
   // points to address 0x00
   const uint8_t TEMP_REGISTER = TEMPERATURE;
   // byte array
   uint8_t data[2];

   // read just the temperature
   if (resolution == HIGH)
   {
      // for high resolution reading 14 bits
      setConfig(T_OR_H_14R);
   }
   else
   {
      // for med resolution reading 11 bits
      setConfig(TEMP_11R);
   }

   // get the reading from the temperature sensor
   int ret = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &TEMP_REGISTER, 1, false);

   if (resolution == HIGH)
   {
      sleep_ms(9);
   }
   else if (resolution == MED)
   {
      sleep_ms(5);
   }

   ret = i2c_read_blocking(I2C_PORT, HDC1080ADDRESS, data, 2, false);

   int16_t bit_temp_val = data[0] << 8 | data[1];
   double hex_conv = ((double)bit_temp_val) / ((double)65536);
   float final_temp = (hex_conv * 165) - 40;

   if (degrees == CEL)
   {
      return final_temp; // degrees C
   }

   return (final_temp * 1.8) + 32; // degrees F conversion
}

/*HDC1080 FUNCTION DEFINITIONS*/

/**
 * @brief
 *
 * @return int
 */
int readDeviceID()
{
   // unsigned integer array that holds the device ID's
   uint8_t deviceID[2];
   int ret;

   /*Assign Register*/
   uint8_t manReg = HDC1080_MANF_DEVICE_ID_REG;

   ret = i2c_write_blocking(I2C_PORT,       // type of port
                            HDC1080ADDRESS, // device address
                            &manReg,        // device's register address to read
                            1,              // expected data size to receive in bytes
                            false           // bool value to tell the I2C controller to: True = use and hold onto the bus, False = release the bus
   );

   ret = i2c_read_blocking(I2C_PORT,       // type of port
                           HDC1080ADDRESS, // device address
                           deviceID,       // pass the uint_8 array to receive the data from the register
                           2,              // expected data size to receive in bytes
                           false           // bool value to tell the I2C controller to: True = use the bus, False = release the bus
   );

   int returnValue = deviceID[0] << 8 | deviceID[1];

   return returnValue;
}

/**
 * @brief This Block of Functions reads and returns the unique serial
 * number from the HDC1080 Device
 *
 * @return int
 */
int readSerial1()
{
   uint8_t deviceID[2];
   uint8_t serial1 = SERIAL_FIRST;

   int ret1;
   ret1 = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &serial1, 1, false);

   ret1 = i2c_read_blocking(I2C_PORT, HDC1080ADDRESS, deviceID, 2, false);

   int returnVal1 = deviceID[0] << 8 | deviceID[1];

   return returnVal1;
}

int readSerial2()
{
   uint8_t deviceID[2];
   uint8_t serial2 = SERIAL_MID;

   int ret;

   ret = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &serial2, 1, false);

   ret = i2c_read_blocking(I2C_PORT, HDC1080ADDRESS, deviceID, 2, false);

   int returnVal = deviceID[0] << 8 | deviceID[1];

   return returnVal;
}

int readSerial3()
{
   uint8_t deviceID[2];
   uint8_t serial3 = SERIAL_LAST;

   int ret;
   ret = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &serial3, 1, false);

   ret = i2c_read_blocking(I2C_PORT, HDC1080ADDRESS, deviceID, 2, false);

   int returnVal = deviceID[0] << 8 | deviceID[1];

   return returnVal;
}

/**
 * @brief Set the configuration for the HDC1080's output for temperature and humidity
 *
 * @param conf_val
 */
void setConfig(enum HDC_Config_Reg conf_val)
{
   // CONFIG = 0x02
   const uint8_t configReg = CONFIG;
   uint8_t set[] = {configReg, conf_val, 0x00};

   // write 3 bytes at a time
   int value = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &set[0], 3, false);
}

/**
 * @brief
 *
 * @return int
 */
int readConfig()
{
   int ret;
   uint8_t configOut[2];
   uint8_t config = CONFIG;

   ret = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &config, 1, false);

   ret = i2c_read_blocking(I2C_PORT, HDC1080ADDRESS, configOut, 2, false);

   int returnVal = configOut[0];

   return returnVal;
}

/**
 * @brief
 *
 * @return float
 */
float tempFahr(void)
{
   return temperature(FAHR, HIGH);
}

/**
 * @brief
 *
 * @return float
 */
float tempCels(void)
{
   return temperature(CEL, HIGH);
}

float calc_humidity(enum Resolution_Type resolution)
{
   // address 0x01
   const uint8_t HUMIDITY_REG = HUMIDITY;
   uint8_t data[2];

   // set config to read humidity
   if (resolution == HIGH)
   {
      setConfig(T_OR_H_14R);
   }
   else if (resolution == MED)
   {
      setConfig(HUMID_11R);
   }
   else
   {
      setConfig(HUMID_8R);
   }

   // write humidity reading to register
   int hum_val = i2c_write_blocking(I2C_PORT, HDC1080ADDRESS, &HUMIDITY_REG, 1, false);

   if (resolution == HIGH)
   {
      sleep_ms(9);
   }
   else if (resolution == MED)
   {
      sleep_ms(7);
   }
   else
   {
      sleep_ms(5);
   }

   hum_val = i2c_read_blocking(I2C_PORT, HDC1080ADDRESS, data, 2, false);

   int16_t bit_val = data[0] << 8 | data[1];
   double hex_conv = ((double)bit_val) / ((double)65536);
   float tot_hum = hex_conv * 100;
   return tot_hum;
}