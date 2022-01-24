#include "main.h"
#include "gps.h"
#include "gsm.h"
#include "string.h"
#include "stdlib.h"


#define GPS_BUFFER_SIZE 256
#define GPS_WORKING_BUFFER_SIZE 128

uint8_t GPSBufferLines;
uint8_t GPSBufferTail;
uint8_t GPSBufferHead;
uint8_t GPSWorkingBuffer[GPS_WORKING_BUFFER_SIZE];
uint8_t GPSBuffer[GPS_BUFFER_SIZE];


//Function for parsing NMEA messages
char* GPS_Strtoke(char *str, const char *delim){
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  /* assign new start in case */
  if (str) start = str;
  /* check whether text to parse left */
  if (!start) return NULL;
  /* remember current start as found token */
  token = start;
  /* find next occurrence of delim */
  start = strpbrk(start, delim);
  /* replace delim with terminator and move start to follower */
  if (start) *start++ = '\0';
  /* done */
  return token;
}

int GPS_GetCharFromBuffer(){
	if(GPSBufferHead == GPSBufferTail){
		return -1;
	}
	GPSBufferTail = (GPSBufferTail + 1) % GPS_BUFFER_SIZE;
	return GPSBuffer[GPSBufferTail];
}

int GPS_GetLineFromBuffer(){
	char LocalChar;
	char* LocalLinePointer = (char*)GPSWorkingBuffer;
	if(GPSBufferLines){
		while((LocalChar = GPS_GetCharFromBuffer())){
			if(LocalChar == 13){							//detecting CR
				break;
			}
			*LocalLinePointer = LocalChar;
			LocalLinePointer++;
		}
		*LocalLinePointer = 0;
		GPSBufferLines--;
	}
	return 0;
}

void GPS_UartReceiveChar(){
	uint8_t LocalHead;
	LocalHead=(GPSBufferHead + 1) % GPS_BUFFER_SIZE;
	if(LocalHead == GPSBufferTail){
		GPSBufferHead = GPSBufferTail;
	}
		else{
			if(GPSReceivedChar == 13){
				GPSBufferLines++;
				GPSBufferHead = LocalHead;
				GPSBuffer[LocalHead] = GPSReceivedChar;
			}
			else if((GPSReceivedChar == 0) || (GPSReceivedChar == 10)){} //Ignore that chars
			else{
				GPSBufferHead = LocalHead;
				GPSBuffer[LocalHead] = GPSReceivedChar;
			}

		}
	HAL_UART_Receive_IT(GPS_huart, (uint8_t*)&GPSReceivedChar, 1);
}

void GPS_ParseGRMC(){
	char *LocalParsePointer;
	uint32_t LocalTemp;

		//Time
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	if(strlen(LocalParsePointer) > 0){
		LocalTemp = atoi(LocalParsePointer);
		GPS_Second = LocalTemp % 100;
		GPS_Minute = (LocalTemp / 100) % 100;
		GPS_Hour = ((LocalTemp / 10000) % 100)+utc;
	}

		//Valid flag for GPS data -> A=correct data, V=incorrect data
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	if(strlen(LocalParsePointer) > 0){
		GPS_Valid = *LocalParsePointer;
		}

		// Latitude
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	if(strlen(LocalParsePointer) > 0){
		GPS_Latitude = atof(LocalParsePointer);
		GPS_LatitudeDeg = GPS_Latitude / 100;								//converting to decimal coordinates
		GPS_LatitudeSec = GPS_Latitude - (GPS_LatitudeDeg*100);
		GPS_Latitude = GPS_LatitudeDeg + (GPS_LatitudeSec)/60;
	}

		// Latitude Direction
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	if(strlen(LocalParsePointer) > 0){
		GPS_LatitudeDirection = *LocalParsePointer;
	}

		// Longnitude
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	if(strlen(LocalParsePointer) > 0){
		GPS_Longitude = atof(LocalParsePointer);
		GPS_LongitudeDeg = GPS_Longitude /100;								//converting to decimal coordinates
		GPS_LongitudeSec = GPS_Longitude - (GPS_LongitudeDeg*100);
		GPS_Longitude = GPS_LongitudeDeg + (GPS_LongitudeSec)/60;

	}

		// Longnitude Direction
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	if(strlen(LocalParsePointer) > 0){
		GPS_LongitudeDirection = *LocalParsePointer;
	}

		//Unnecessary elements
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	LocalParsePointer = GPS_Strtoke(NULL, ",");
	LocalParsePointer = GPS_Strtoke(NULL, ",");

		//Date
	if(strlen(LocalParsePointer) > 0){
		LocalTemp = atoi(LocalParsePointer);
		GPS_Year = (LocalTemp % 100);
		GPS_Month = (LocalTemp / 100) % 100;
		GPS_Day = (LocalTemp / 10000) % 100;
	}
}

void GPS_ParseLine(){
	char* LocalParsePointer = GPS_Strtoke((char*)GPSWorkingBuffer, ",");
	if(strcmp(LocalParsePointer, "$GPRMC") == 0) GPS_ParseGRMC();
}

void GPS_Work(){
	if(GPSBufferLines){
		GPS_GetLineFromBuffer();
		GPS_ParseLine();
	}
}

void GPS_Init(UART_HandleTypeDef *huart1){
	GPS_huart = huart1;
	GPS_Hour=0;
	GPS_Minute=0;
	GPS_Second=0;
	GPS_Year=0;
	GPS_Month=0;
	GPS_Day=0;
	GPS_Latitude=0;
	GPS_LatitudeDirection='0';
	GPS_Longitude=0;
	GPS_LongitudeDirection='0';
	HAL_UART_Receive_IT(GPS_huart, (uint8_t*)&GPSReceivedChar, 1);
}
