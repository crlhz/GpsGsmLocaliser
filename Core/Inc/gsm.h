#ifndef INC_GSM_H_
#define INC_GSM_H_

#include "main.h"
#include "string.h"

UART_HandleTypeDef *GSM_huart;
char Message[180];
uint8_t mode;
uint8_t GSM_Response;
uint8_t controlLCD;
uint8_t interval;
uint8_t utc;
uint8_t timeSetFlag;
uint8_t compareMinutes;
char number[10];

void GSM_UartReceiveChar();
void GSM_Work();
void GSM_LocateMessage();
void GSM_SelectNumber();
void GSM_SendMessage(char *text);
void GSM_Init(UART_HandleTypeDef *huart);
#endif
