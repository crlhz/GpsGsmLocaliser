#include "gsm.h"
#include "gps.h"
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#define GSM_BUFFER_SIZE 256
#define GSM_WORKING_BUFFER_SIZE 128

volatile uint8_t GSMReceivedChar;
uint8_t GSMBufferLines;
uint8_t GSMBufferTail;
uint8_t GSMBufferHead;
uint8_t GSMWorkingBuffer[GSM_WORKING_BUFFER_SIZE];
uint8_t GSMBuffer[GSM_BUFFER_SIZE];
uint16_t size=0;


int GSM_GetCharFromBuffer(){
	if(GSMBufferHead == GSMBufferTail){
		return -1;
	}
	GSMBufferTail = (GSMBufferTail + 1) % GSM_BUFFER_SIZE;
	return GSMBuffer[GSMBufferTail];
}

int GSM_GetLineFromBuffer(){
	char LocalChar;
	char* LocalLinePointer = (char*)GSMWorkingBuffer;
	if(GSMBufferLines){
		while((LocalChar = GSM_GetCharFromBuffer())){
			if(LocalChar == 13){							//detecting CR
				break;
			}
			*LocalLinePointer = LocalChar;
			LocalLinePointer++;
		}
		*LocalLinePointer = 0;
		GSMBufferLines--;
	}
	return 0;
}

void GSM_UartReceiveChar(){
	uint8_t LocalHead;
	LocalHead=(GSMBufferHead + 1) % GSM_BUFFER_SIZE;
	if(LocalHead == GSMBufferTail){
		GSMBufferHead = GSMBufferTail;
	}
		else{
			if(GSMReceivedChar == 13){
				GSMBufferLines++;
				GSMBufferHead = LocalHead;
				GSMBuffer[LocalHead] = GSMReceivedChar;
			}
			else if((GSMReceivedChar == 0) || (GSMReceivedChar == 10)){} //Ignore that chars
			else{
				GSMBufferHead = LocalHead;
				GSMBuffer[LocalHead] = GSMReceivedChar;
			}

		}
	HAL_UART_Receive_IT(GSM_huart, (uint8_t*)&GSMReceivedChar, 1);
}

void GSM_ParseNumber(){
	char *LocalParsePointer;

		//**Get phone number only in init**//
		LocalParsePointer = GPS_Strtoke(NULL, " ");
			if((strlen(LocalParsePointer) > 0)&&(controlLCD==0)){
				sprintf(number,"%s",LocalParsePointer);
			}

		//**Get UTC correction**//
		LocalParsePointer = GPS_Strtoke(NULL, " ");
			if((strlen(LocalParsePointer) > 0)&&(controlLCD==0)){
				utc=atoi(LocalParsePointer);
			}
}

void GSM_ParseMode(){
	char *LocalParsePointer;

		//**Get mode**//
		LocalParsePointer = GPS_Strtoke(NULL, " ");
			if(strlen(LocalParsePointer) > 0){
				mode = atoi(LocalParsePointer);
				if(mode==1){
					compareMinutes=GPS_Minute;
				}
			}

		//**Get interval between messages in mode 1**//
		LocalParsePointer = GPS_Strtoke(NULL, " ");
			if(strlen(LocalParsePointer) > 0){
				interval = atoi(LocalParsePointer);
			}
}

void GSM_Read(){
	size=sprintf(Message, "at+cmgl=\"REC UNREAD\"\r");
	HAL_UART_Transmit_IT(GSM_huart, Message, size);
}

void GSM_SelectNumber(){
		size=sprintf(Message, "AT+CMGS=\"+48%s\"\r",number);
		HAL_UART_Transmit_IT(GSM_huart, Message, size);
}

void GSM_ParseLine(){
	char* LocalParsePointer = GPS_Strtoke((char*)GSMWorkingBuffer, " ");
	if(strcmp(LocalParsePointer, "+CMTI:") == 0){
		GSM_Read();
	}

	if(strcmp(LocalParsePointer, "NUMBER") == 0){
			GSM_ParseNumber();
		}

	if(strcmp(LocalParsePointer, "MODE") == 0){
		GSM_Response=2;
		GSM_ParseMode();
		GSM_SelectNumber();
	}

	if(strcmp(LocalParsePointer, "FIND") == 0){
		GSM_Response=3;
		GSM_SelectNumber();
	}
}

void GSM_LocateMessage(){
	size=sprintf(Message, "%02d.%02d.%4d %02d:%02d:%02d\nTu jestem:\n%6f%c\n%6f%c\r\x1A", GPS_Day, GPS_Month, GPS_Year+2000, GPS_Hour, GPS_Minute, GPS_Second,
					GPS_Latitude, GPS_LatitudeDirection, GPS_Longitude, GPS_LongitudeDirection);
	HAL_UART_Transmit_IT(GSM_huart, Message, size);
}

void GSM_Work(){
	if(GSMBufferLines){
		GSM_GetLineFromBuffer();
		GSM_ParseLine();
	}
}

void GSM_Init(UART_HandleTypeDef *huart2){
	GSM_huart=huart2;
	HAL_UART_Receive_IT(GSM_huart, (uint8_t*)&GSMReceivedChar, 1);
}

void GSM_SendMessage(char *text){
	size=sprintf(Message, "%s\r\x1A",text);				//x1A means end of message for GSM module
	HAL_UART_Transmit_IT(GSM_huart, Message, size);
}


