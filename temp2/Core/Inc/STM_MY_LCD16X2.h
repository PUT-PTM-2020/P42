#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

//Commands
#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME 			0x02
#define LCD_ENTRYMODESET 		0x04
#define LCD_DISPLAYCONTROL 	0x08
#define LCD_CURSORSHIFT 		0x10
#define LCD_FUNCTIONSET 		0x20
#define LCD_SETCGRAMADDR 		0x40
#define LCD_SETDDRAMADDR 		0x80


#define LCD_ENTRY_SH		 				0x01
#define LCD_ENTRY_ID 						0x02
#define LCD_ENTRY_SH		 				0x01
#define LCD_ENTRY_ID 						0x02
#define LCD_DISPLAY_B						0x01
#define LCD_DISPLAY_C						0x02
#define LCD_DISPLAY_D						0x04
#define LCD_SHIFT_RL						0x04
#define LCD_SHIFT_SC						0x08
#define LCD_FUNCTION_F					0x04
#define LCD_FUNCTION_N					0x08
#define LCD_FUNCTION_DL					0x10


static void LCD1602_EnablePulse(void);
static void LCD1602_RS(bool state);
static void LCD1602_write(uint8_t byte);
static void LCD1602_writeCommand(uint8_t command);
static void LCD1602_writeData(uint8_t data);
static void LCD1602_write4bitCommand(uint8_t nibble);


void LCD1602_Begin8BIT(GPIO_TypeDef* PORT_RS_E, uint16_t RS, uint16_t E, GPIO_TypeDef* PORT_LSBs0to3, uint16_t D0, uint16_t D1, uint16_t D2, uint16_t D3, GPIO_TypeDef* PORT_MSBs4to7, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7);
void LCD1602_Begin4BIT(GPIO_TypeDef* PORT_RS_E, uint16_t RS, uint16_t E, GPIO_TypeDef* PORT_MSBs4to7, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7);
void LCD1602_print(char string[]);
void LCD1602_setCursor(uint8_t row, uint8_t col);
void LCD1602_1stLine(void);
void LCD1602_2ndLine(void);
void LCD1602_TwoLines(void);
void LCD1602_OneLine(void);
void LCD1602_noCursor(void);
void LCD1602_cursor(void);
void LCD1602_clear(void);
void LCD1602_noBlink(void);
void LCD1602_blink(void);
void LCD1602_noDisplay(void);
void LCD1602_display(void);
void LCD1602_shiftToRight(uint8_t num);
void LCD1602_shiftToLeft(uint8_t num);


void LCD1602_PrintInt(int number);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
