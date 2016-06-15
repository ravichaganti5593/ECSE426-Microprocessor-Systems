#include <string.h>
#include <stdio.h>
#include "stm32f429i_discovery_lcd.h"
#include "lcd_writer_sm.h"

static const uint8_t MAX_NUM_PER_LINE = 5;
static char messages[LCD_WRITER_LINE_COUNT][LCD_WRITER_STR_LEN];
static uint8_t index;

#define LCD_ADDR_MSG(i) (&(messages[i][1]))

/**
 * Display all messages to LCD screen
 *
 * @param void
 *
 * @return void
 */
void lcd_writer_display(void) {
	for (uint8_t i = 0;i < LCD_WRITER_LINE_COUNT; i++) {
		LCD_DisplayStringLine(LINE(i), (uint8_t*)messages[i]);
	}
}

/**
 * Increment the line index
 *
 * @param void
 *
 * @return void
 */
static void increment_index(void) {
	if (messages[index][0] != '\0') {
		messages[index][0] = ' ';	
	}
	
	index++;
	if (index >= LCD_WRITER_LINE_COUNT) {
		index = 0;
	}
}

/**
 * Write a message on the next line of LCD
 *
 * @param message 	pointer to the null terminated message string
 *
 * @return void
 */
void lcd_write_message(char* message) {
	increment_index();
	messages[index][0] = '*';
	strcpy(LCD_ADDR_MSG(index), message);
}

/**
 * Display all numbers in a buffer to the next line on LCD
 *
 * @param numbers 	pointer to the buffer containing the number
 * @param len 		length of the buffer
 *
 * @return void
 */
static void display_all_numbers(uint8_t* numbers, uint8_t len) {
	if (len == 0) {
		return;
	}
	
	static char long_addr[LCD_WRITER_STR_LEN];
	char* writing_addr = long_addr + 1;
	long_addr[0] = '>';

	if (len == 1) {
		sprintf(writing_addr, "%d-", numbers[0]);
	} else if (len == 2) {
		sprintf(writing_addr, "%d-%d", numbers[0], numbers[1]);
	} else if (len == 3) {
		sprintf(writing_addr, "%d-%d-%d", numbers[0], numbers[1], numbers[2]);
	} else if (len == 4) {
		sprintf(writing_addr, "%d-%d-%d-%d", numbers[0], numbers[1], numbers[2], numbers[3]);
	} else if (len == 5) {
		sprintf(writing_addr, "%d-%d-%d-%d-%d", numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);
	} else if (len == 6) {
		sprintf(writing_addr, "%d-%d-%d-%d-%d-%d", numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], numbers[5]);
	}
	lcd_write_message(long_addr);
}

/**
 * Display a byte buffer on LCD. Can span multiple lines. Max 4 numbers per line
 *
 * @param buffers 	pointer to the buffer containing the numbers
 * @param len 		length of the buffer
 *
 * @return void
 */
void lcd_writer_print_buffer(uint8_t* buffer, uint8_t len) {
	if (len == 0) {
		return;
	}

	static uint8_t to_display[MAX_NUM_PER_LINE];
	uint8_t i = 0;
	while (1) {
		uint8_t display_len = 0;
		for (uint8_t j = 0; j < MAX_NUM_PER_LINE; j++) {
			to_display[j] = buffer[i];
			display_len++;
			i++;

			if (i == len) {
				break;
			}
		}

		display_all_numbers(to_display, display_len);
		if (i == len) {
			break;
		}
	}
}

/**
 * Clear all messages on LCD
 *
 * @param void
 *
 * @return void
 */
void lcd_writer_clear(void) {
	index = LCD_WRITER_LINE_COUNT - 1;
	for (uint8_t i = 0; i < LCD_WRITER_LINE_COUNT; i++) {
		messages[i][0] = '\0';
	}
}
