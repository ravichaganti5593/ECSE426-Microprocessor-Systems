/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header

#include <stdio.h>
#include <string.h>

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
//#include "background16bpp.h"
#include "static/trottier16.h"

#include "interfaces/cc2500.h"
#include "modules/coordinate_db.h"
#include "modules/lcd_writer_sm.h"
#include "modules/commands.h"
#include "modules/protocol_go_back_1.h"

#include "utils/utils.h"
#include "my_types.h"

/**
 *	Micro P lab has 34 floor tiles
 *	Each floor tile is 12 inches = 1 foot
 *		calcualted by almost 2 US dollar bills
 *		around 1 + 1/12 sheet of US letter paper
 *		verfied with measuring tape
 *
 *	bottom of lab is pixel 316
 *	top of lab is pixel 253
 *
 *	lab is 63 pixels long and 34 feet
 *	each pixel is then 34/63 ~= 0.540 feet
 *
 *	Each foot is 1.85194 = approx. 1.853x
 */
#define MSTEP_TO_FEET 2.5f
#define FEET_TO_PIXEL 1.853f
#define MSTEP_TO_PIXEL MSTEP_TO_FEET * FEET_TO_PIXEL
#define LAB_DOOR_PX_X 163
#define LAB_DOOR_PX_Y 266

static char long_message[40];
static uint8_t temp_buffer[100];

static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--)
  {
  }
}

/**
 *	Convert an array of coordinates in units of msteps (Michael-steps) to
 *	pixel locations to display on the LCD map. Conversion happens in place to
 *	save memory.
 *
 *	@author 			Jacob Barnett
 *
 *	@param mstep_coord 	array of coordinates in michael seps to convert
 *	@param length 		length of the array to be converted
 *
 *	@return void
 */
void mstep_to_pixloc(int16_t* mstep_coord, uint8_t length) {

	if (length % 2 != 0) return;

	for (uint8_t i = 0; i < length; i+=2) {
		mstep_coord[i] = mstep_coord[i] * MSTEP_TO_PIXEL + LAB_DOOR_PX_X;
		mstep_coord[i+1] = LAB_DOOR_PX_Y - mstep_coord[i+1] * MSTEP_TO_PIXEL;
	}
}

static uint8_t is_drawing;
static COORDINATE_TYPE xs[COORDINATE_DB_SIZE];
static COORDINATE_TYPE ys[COORDINATE_DB_SIZE];

/**
 *	Draw an array of points on the screen, taking into account a scaling factor.
 *
 *	@author				Jacob Barnett
 *
 *	@param scale_x		scaling factor for the x direction
 *	@param scale_y		scaling factor for the y direction
 *	@param xs			list of x coordinates
 *	@param ys			list of y coordinates
 *	@param length		length of each of the lists of coordinates
 *
 *	@return void
 */
void draw_points(uint16_t scale_x, uint16_t scale_y, COORDINATE_TYPE* xs, COORDINATE_TYPE* ys, uint16_t length) {

	float prev_pixels_x = (float)xs[0] / scale_x;
	float prev_pixels_y = (float)ys[0] / scale_y;
	float pixels_x;
	float pixels_y;

	for (int i = 1; i < length; ++i) {
		pixels_x = (float)xs[i] / scale_x;
		pixels_y = (float)ys[i] / scale_y;

		LCD_DrawUniLine(prev_pixels_x, prev_pixels_y, pixels_x, pixels_y);

		prev_pixels_x = pixels_x;
		prev_pixels_y = pixels_y;
	}
}

/**
 *	Draw the list of coordinates currently in the database
 *
 *	@author HP Truong, Jacob Barnett
 *
 *	@param void
 *	@return void 
 */
void draw_from_db(void) {
	LCD_Clear(LCD_COLOR_WHITE);
	LCD_SetTextColor(LCD_COLOR_RED);
	memcpy ( (void *)(LCD_FRAME_BUFFER + BUFFER_OFFSET), (void *) ((uint8_t*) (&background)), sizeof(background));
	
	uint8_t len = coordinate_db_get_len();
	if (len == 0) {
		return;
	}

	static coordinate next_coord;

	for (uint8_t i = 0; i < len; i++) {
		coordinate_db_get_entry(i, &next_coord);
		xs[i] = next_coord.x;
		ys[i] = next_coord.y;
	}

	draw_points(1, 1, xs, ys, len);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Receiving and plotting thread:
 *	Can receive two types of commands from the measuring board along with
 *	coordinate data. A message will either be a command or data. Commands are
 *	as follows:
 *
 *	I. Clear	
 *	clears the database of all stored values 	
 *
 *	II. Plot and terminate
 *	plots database values on the LCD and terminates all other threads	
 *
 *	@author HP Truong, Jacob Barnett
 *
 *	@param argument			any arguments to be passed to the thread
 *	@return void
 */
void receive_and_plot(void const *argument) {
	is_drawing = FALSE;
	lcd_writer_clear();
	protocol_go_back_1_init(GO_BACK_ONE_MODE_RECEIVER);
	
	static uint8_t result = 0;
	
	while (1) {
		memset(temp_buffer, 0, 12);
		protocol_go_back_1_periodic();

		temp_buffer[0] = protocol_go_back_1_get_state();
		temp_buffer[1] = CC2500_get_state();
		
		//Debug prints
		uint8_t len = protocol_go_back_1_get_received_data(temp_buffer + 2);
		lcd_writer_clear();
		lcd_writer_print_buffer(temp_buffer, 14);
		sprintf(long_message, "%d", 4);
		lcd_write_message(long_message);
		sprintf(long_message, "drawing = %d", is_drawing);
		lcd_write_message(long_message);
		sprintf(long_message, "len is %d", coordinate_db_get_len());
		lcd_write_message(long_message);
		sprintf(long_message, "result is %d", result);
		lcd_write_message(long_message);
		//End debug prints
		
		
		if (temp_buffer[0] == GO_BACK_ONE_RECEIVER_STATE_IDLE) {
			if (len == COMMAND_CLEAR_LEN) {
				if (memcmp(temp_buffer + 2, CLEAR_COMMAND, len) == 0) {
					is_drawing = FALSE;
					coordinate_db_clear();
					protocol_go_back_1_init(GO_BACK_ONE_MODE_RECEIVER);
					continue;
				}
			}

			if (len == COMMAND_PLOT_LEN) {
				if (memcmp(temp_buffer + 2, PLOT_COMMAND, len) == 0) {
					is_drawing = TRUE;
					draw_from_db();
					break;
				}
			}

			if (is_drawing == FALSE) {
				
				if (len > 0 && len % sizeof(COORDINATE_TYPE) == 0) {			
					mstep_to_pixloc((COORDINATE_TYPE*) (temp_buffer + 2), len / sizeof(COORDINATE_TYPE));
					result = coordinate_db_insert_entry((COORDINATE_TYPE*) (temp_buffer + 2), len / sizeof(COORDINATE_TYPE));
				}
			}

			protocol_go_back_1_receive();
		}
		
		osDelay(20);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Printing debug to the LCD thread:
 *	Can print text to the LCD for debugging and informational purposes
 *
 *	@author HP Truong, Jacob Barnett
 *
 *	@param argument 		any arguments to be passed to the thread
 *	@return void
 */
void print_lcd_debug(void const *argument){
	while(1){
		if (is_drawing == FALSE) {
			LCD_Clear(LCD_COLOR_WHITE);
			lcd_writer_display();
		} else {
			break;
		}

		osDelay(250);
	}
}

osThreadDef(receive_and_plot, osPriorityNormal, 1, 0);
osThreadDef(print_lcd_debug, osPriorityNormal, 1, 0);

// ID for theads
osThreadId receive_and_plot_thread;
osThreadId print_lcd_debug_thread;

/*
 *	Main function: initializes all system values and components, then starts
 *	operation of the two threads.
 *
 *	@author HP Truong, Jacob Barnett
 *
 *	@param void
 *	@return void
 */
int main (void) {
	CC2500_LowLevel_Init();
	CC2500_Reset();

	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// initialize peripherals here
	/* LCD initiatization */
	LCD_Init();
  
	/* LCD Layer initiatization */
	LCD_LayerInit();

	/* Enable the LTDC controler */
	LTDC_Cmd(ENABLE);

	/* Set LCD foreground layer as the current layer */
	LCD_SetLayer(LCD_FOREGROUND_LAYER);

	LCD_SetFont(&Font16x24);
	LCD_Clear(LCD_COLOR_WHITE);

	receive_and_plot_thread = osThreadCreate(osThread(receive_and_plot), NULL);
	print_lcd_debug_thread = osThreadCreate(osThread(print_lcd_debug), NULL);

	osKernelStart ();                         // start thread execution 
}
