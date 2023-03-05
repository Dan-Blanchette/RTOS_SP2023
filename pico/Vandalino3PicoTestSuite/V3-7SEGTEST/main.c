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


/*PICO PIN SETUP*/
// D13 Pin Assignment pico
const uint LED_PIN = PICO_DEFAULT_LED_PIN;


/*SEMAPHORES*/
// Semaphore intilizaition
SemaphoreHandle_t xSem;

/***********************
 * 						  *
 * 	   QUEUES 		  *
 * 						  *
 ***********************/
// digit queues for the 7-seg display
static QueueHandle_t xQueueDisp = NULL;
// pico D13 Blink
static QueueHandle_t xQueuePico = NULL;


/***********************
 * 						  *
 *       TASKS         *
 * 						  *
 ***********************/
// Starts at draws the 0-9 count, 9-0 count on the right display
// PRIORITY: 5
void task_right_disp()
{
	
	while (1)
	{
		// Take the semaphore
		xSemaphoreTake(xSem, portMAX_DELAY);
		//printf("In right display\n");
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
		//printf("In left display\n");
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

		//printf("decrementing %d\n", i);

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


void task_buttons()
{
	while(1)
	{
		int button1, button2, button3;
		button1 = gpio_get(BUTTON1);
		button2 = gpio_get(BUTTON2);
		button3 = gpio_get(BUTTON3);

		printf("button1/2/3 %d%d%d\n", button1, button2, button3);
		vTaskDelay(100 / portTICK_PERIOD_MS);
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
  stdio_init_all();
 
 // creates Queue NOTE: like malloc specify data type
 xQueueDisp = xQueueCreate(1, sizeof(int));
 // create Pico Queue (has two values 0 or 1)
 xQueuePico = xQueueCreate(1, sizeof(int));
 // NOTE:create binary semaphore before tasks but after init!
 xSem = xSemaphoreCreateBinary();
 // take flag from semaphore
 xSemaphoreGive(xSem);

  
	// This first task function's format is meant as a reference guide for the parameters

  xTaskCreate(task_buttons, "Task_Buttons", 256, NULL, 5, NULL);
  xTaskCreate(
              task_right_disp, // fucntion to be called
             "Task_Right_Disp", // Name of Task 
              256,  // Stack Size
              NULL, // Parameter to pass to a function
              4,  // Task Priority (0 to configMAX_PRIORITIES - 1)
              NULL // Task handle (check on status, watch memory usage, or end the task)
              );
  xTaskCreate(task_left_disp, "Task_Left_Disp", 256, NULL, 4, NULL);
  xTaskCreate(task_count, "Task_Count", 256, NULL, 3, NULL);
  xTaskCreate(task_pico_blink, "Task_Pico_Blink", 256, NULL, 2, NULL);
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

void draw_hex_val(const int rem)
{
	switch (rem)
	{
		// draw  'a'
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

// BUTTONS
/*
		int button1, button2, button3;
		button1 = gpio_get(BUTTON1);
		button2 = gpio_get(BUTTON2);
		button3 = gpio_get(BUTTON3);

		printf("button1/2/3 %d%d%d\n", button1, button2, button3);
*/