#ifndef GPS_H_
#define GPS_H_

#include "main.h"

UART_HandleTypeDef *GPS_huart;

volatile uint8_t GPSReceivedChar;

//Time&Date variables
uint8_t GPS_Hour;
uint8_t GPS_Minute;
uint8_t GPS_Second;
uint16_t GPS_Year;
uint8_t GPS_Month;
uint8_t GPS_Day;
char GPS_Valid;

//Position variables
double GPS_Latitude;
uint16_t GPS_LatitudeDeg;
float GPS_LatitudeSec;
char GPS_LatitudeDirection;

double GPS_Longitude;
uint16_t GPS_LongitudeDeg;
float GPS_LongitudeSec;
char GPS_LongitudeDirection;


void GPS_UartReceiveChar();
void GPS_Work();
void GPS_Init(UART_HandleTypeDef *huart);
#endif
