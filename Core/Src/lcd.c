#include "lcd.h"

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};

//LCD static functions
static void LCD_WriteData(LCD_HandleTypeDef * LCD, uint8_t Data);
static void LCD_WriteCommand(LCD_HandleTypeDef * LCD, uint8_t Command);
static void LCD_Write(LCD_HandleTypeDef * LCD, uint8_t Data, uint8_t Length);

//LCD configuration
LCD_HandleTypeDef LCD_Create(
		LCD_PortType Port[], LCD_PinType Pin[],
		LCD_PortType RsPort, LCD_PinType RsPin,
		LCD_PortType EnPort, LCD_PinType EnPin)
{
	LCD_HandleTypeDef LCD;


	LCD.EnPin = EnPin;
	LCD.EnPort = EnPort;

	LCD.RsPin = RsPin;
	LCD.RsPort = RsPort;

	LCD.DataPin = Pin;
	LCD.DataPort = Port;

	LCD_Init(&LCD);

	return LCD;
}

void LCD_Init(LCD_HandleTypeDef * LCD){
	LCD_WriteCommand(LCD, 0x33);
	LCD_WriteCommand(LCD, 0x32);
	LCD_WriteCommand(LCD, FUNCTION_SET | NUMBER_LINES);

	LCD_WriteCommand(LCD, CLEAR_DISPLAY);
	LCD_WriteCommand(LCD, DISPLAY_ON_OFF_CONTROL | DISPLAY_ON);
	LCD_WriteCommand(LCD, ENTRY_MODE_SET | CURSOR_INC);
}

void LCD_Int(LCD_HandleTypeDef * LCD, int Number){
	char Buffer[11];
	sprintf(Buffer, "%02d", Number);
	LCD_String(LCD, Buffer);
}

void LCD_String(LCD_HandleTypeDef * LCD, char * String){
	for(uint8_t i = 0; i < strlen(String); i++){
		LCD_WriteData(LCD, String[i]);
	}
}

void LCD_Cursor(LCD_HandleTypeDef * LCD, uint8_t Row, uint8_t Col){
	LCD_WriteCommand(LCD, SET_DDRAM_ADDR + ROW_16[Row] + Col);
}

void LCD_Clear(LCD_HandleTypeDef * LCD){
	LCD_WriteCommand(LCD, CLEAR_DISPLAY);
}

void LCD_WriteCommand(LCD_HandleTypeDef * LCD, uint8_t Command){
		HAL_GPIO_WritePin(LCD->RsPort, LCD->RsPin, LCD_COMMAND_REG);
		LCD_Write(LCD, (Command >> 4), LCD_NIB);
		LCD_Write(LCD, Command & 0x0F, LCD_NIB);
}

void LCD_WriteData(LCD_HandleTypeDef * LCD, uint8_t Data){
		HAL_GPIO_WritePin(LCD->RsPort, LCD->RsPin, LCD_DATA_REG);
		LCD_Write(LCD, Data >> 4, LCD_NIB);
		LCD_Write(LCD, Data & 0x0F, LCD_NIB);
}

void LCD_Write(LCD_HandleTypeDef * LCD, uint8_t Data, uint8_t Length){
	for(uint8_t i = 0; i < Length; i++){
		HAL_GPIO_WritePin(LCD->DataPort[i], LCD->DataPin[i], (Data >> i) & 0x01);
	}

	HAL_GPIO_WritePin(LCD->EnPort, LCD->EnPin, 1);
	DELAY(1);
	HAL_GPIO_WritePin(LCD->EnPort, LCD->EnPin, 0);
}
