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

// D13 Pin Assignment pico
const uint LED_PIN = PICO_DEFAULT_LED_PIN;



// GPIO pin setup
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


// Globals
SemaphoreHandle_t xSemaphore;


/*FUNCTION PROTOTYPES*/
// setup 7-seg I/O
void setup_7seg();

void draw_numbers(const int );


// create a binary semaphore before starting tasks

/*TASKS*/
// Counts up to 9 for now
void task_right_disp()
{
	while (1)
	{
		setup_7seg();

		gpio_put(SevenSegCC1, 1);
		gpio_put(SevenSegCC2, 0);

		draw_numbers(0);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(1);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(2);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(3);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(4);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(5);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(6);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(7);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(8);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		draw_numbers(9);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

// tens place on the 7-seg
void task_left_disp()
{
	while (1)
	{
		setup_7seg();

	}
}

void task_count()
{
	while(1)
	{

	}
}

// blink pic0
void task_pico_blink()
{
	while (true)
	{
		gpio_init(LED_PIN);
		gpio_set_dir(LED_PIN, GPIO_OUT);
		// Flash pico LED at top of loop

		// pico led on
		gpio_put(LED_PIN, 1);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		// pico led off
		gpio_put(LED_PIN, 0);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

int main()
{
  // Use for debugging
  stdio_init_all();
 // This first task function's format is meant as a reference
  xTaskCreate(
              task_right_disp, // fucntion to be called
             "Task_right_disp", // Name of Task 
              256,  // Stack Size
              NULL, // Parameter to pass to a function
              5,  // Task Priority (0 to configMAX_PRIORITIES - 1)
              NULL // Task handle (check on status, watch memory usage, or end the task)
              );
  // xTaskCreate(task_tens_place, "Task_2", 256, NULL, 4, NULL);
  // tell the scheduler to start running
  vTaskStartScheduler();

  while (1){}
}


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
