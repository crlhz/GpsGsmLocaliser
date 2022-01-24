#ifndef LCD_H_
#define LCD_H_

#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "main.h"

#define CLEAR_DISPLAY 0x01
#define RETURN_HOME 0x02
#define ENTRY_MODE_SET 0x04
#define CURSOR_INC 0x02
#define DISPLAY_ON_OFF_CONTROL 0x08
#define DISPLAY_ON	0x04
#define FUNCTION_SET 0x20
#define NUMBER_LINES 0x08
#define SET_DDRAM_ADDR 0x80

#define DELAY(X) HAL_Delay(X)

#define LCD_NIB 4
#define LCD_DATA_REG 1
#define LCD_COMMAND_REG 0

#define LCD_PortType GPIO_TypeDef*
#define LCD_PinType uint16_t

typedef struct {
	LCD_PortType * DataPort;
	LCD_PinType * DataPin;

	LCD_PortType RsPort;
	LCD_PinType RsPin;

	LCD_PortType EnPort;
	LCD_PinType EnPin;

} LCD_HandleTypeDef;


void LCD_Init(LCD_HandleTypeDef * LCD);
void LCD_Int(LCD_HandleTypeDef * LCD, int Number);
void LCD_String(LCD_HandleTypeDef * LCD, char * String);
void LCD_Cursor(LCD_HandleTypeDef * LCD, uint8_t Row, uint8_t Col);
LCD_HandleTypeDef LCD_Create(
		LCD_PortType Port[], LCD_PinType Pin[],
		LCD_PortType RsPort, LCD_PinType RsPin,
		LCD_PortType EnPort, LCD_PinType EnPin);
void LCD_Clear(LCD_HandleTypeDef * LCD);

#endif
