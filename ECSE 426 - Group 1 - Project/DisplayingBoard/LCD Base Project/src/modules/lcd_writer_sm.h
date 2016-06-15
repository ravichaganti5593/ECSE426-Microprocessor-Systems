#ifndef LCD_WRITER_H
#define LCD_WRITER_H

#include "stm32f4xx.h"

#define LCD_WRITER_LINE_COUNT 13
#define LCD_WRITER_STR_LEN    20

/**
 * Display all messages to LCD screen
 *
 * @param void
 *
 * @return void
 */
void lcd_writer_display(void);

/**
 * Write a message on the next line of LCD
 *
 * @param message 	pointer to the null terminated message string
 *
 * @return void
 */
void lcd_write_message(char* message);

/**
 * Display a byte buffer on LCD. Can span multiple lines. Max 4 numbers per line
 *
 * @param buffers 	pointer to the buffer containing the numbers
 * @param len 		length of the buffer
 *
 * @return void
 */
void lcd_writer_print_buffer(uint8_t* buffer, uint8_t len);

/**
 * Clear all messages on LCD
 *
 * @param void
 *
 * @return void
 */
void lcd_writer_clear(void);

#endif
