// -------------------------------------------------------------------
// File name: Blynk.c
// Description: This code is used to bridge the TM4C123 board and the Blynk Application
//              via the ESP8266 WiFi board
// Author: Mark McDermott and Andrew Lynch (Arduino source)
// Converted to EE445L style Jonathan Valvano
// Orig gen date: May 21, 2018
// Last update: Sept 20, 2018
//
// Download latest Blynk library here:
//   https://github.com/blynkkk/blynk-library/releases/latest
//
//  Blynk is a platform with iOS and Android apps to control
//  Arduino, Raspberry Pi and the likes over the Internet.
//  You can easily build graphic interfaces for all your
//  projects by simply dragging and dropping widgets.
//
//   Downloads, docs, tutorials: http://www.blynk.cc
//   Sketch generator:           http://examples.blynk.cc
//   Blynk community:            http://community.blynk.cc
//
//------------------------------------------------------------------------------

// TM4C123       ESP8266-ESP01 (2 by 4 header)
// PE5 (U5TX) to Pin 1 (Rx)
// PE4 (U5RX) to Pin 5 (TX)
// PE3 output debugging
// PE2 nc
// PE1 output    Pin 7 Reset
// PE0 input     Pin 3 Rdy IO2
//               Pin 2 IO0, 10k pullup to 3.3V  
//               Pin 8 Vcc, 3.3V (separate supply from LaunchPad 
// Gnd           Pin 4 Gnd  
// Place a 4.7uF tantalum and 0.1 ceramic next to ESP8266 3.3V power pin
// Use LM2937-3.3 and two 4.7 uF capacitors to convert USB +5V to 3.3V for the ESP8266
// http://www.ti.com/lit/ds/symlink/lm2937-3.3.pdf
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "Timer.h"
#include "clock.h"
#include "UART.h"
#include "PortF.h"
#include "esp8266.h"
#include "fixed.h"
#include "PWM.h"
#include "ADCSWTrigger.h"

//void EnableInterrupts(void);    // Defined in startup.s
//void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

uint32_t LED;      // VP1
// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num; 
uint32_t pin_int;
 
int editTime = 0;

int growLightDuty = 0;
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}
 
 
// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){int j; char data;
// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
           
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
		if(pin_num == 0x00)
		{
			growLightDuty = pin_int;
			PWM0B_Duty(4000 * growLightDuty);
		}
    else if(pin_num == 0x01)  
		{  
      LED = pin_int;
      PortF_Output(LED<<2); // Blue LED
    }     
		else if(pin_num == 0x02)
		{
			if(getMode() != CLOCK_MODE && pin_int == 1)
			{
				setMode(CLOCK_MODE);
			}
		}
		else if(pin_num == 0x03)
		{
			if(getMode() != SET_ALARM_MODE && pin_int == 1)
			{
				setMode(SET_ALARM_MODE);
			}
		}
		else if(pin_num == 0x04)
		{
			if(getMode() != GRAPH_SENSORS_MODE && pin_int == 1)
			{
				setMode(GRAPH_SENSORS_MODE);
			}
		}
		else if(pin_num == 0x05)
		{
			if(editTime == 1)
			{
				toggleMerridian();
			}
		}
		else if(pin_num == 0x06)
		{
			if(editTime == 1)
			{
				setHour(pin_int);
			}
		}
		else if(pin_num == 0x07)
		{
			if(editTime == 1)
			{
				setMinute(pin_int);
			}
		}
		else if(pin_num == 0x0C)
		{
			if(editTime == 1)
			{
				setSecond(pin_int);
			}
		}
	  else if(pin_num == 0x08)
		{
			 if(pin_int == 1) {editTime = 1;}
			 else{editTime = 0;}
		}
		else if(pin_num == 0x09)
		{
			if(pin_int) {alarmIsArmed = 1; printAlarmStatus("ALARM ON ");}
			else{alarmIsArmed = 0; printAlarmStatus("ALARM OFF"); disableAlarm();}
		}
		else if(pin_num == 0x0A)
		{
			if(pin_int == 1)
			{
				if(getMode() == CLOCK_MODE)
				{
					disableAlarm();
				}
			}
		}
		else if(pin_num == 0x0B)
		{
			alarmVolume = pin_int;
		}
		else if(pin_num == 0x0C)
		{
			setSensor(pin_int);
		}
  }  
}

void SendInformation(void)
{
	// your account will be temporarily halted if you send too much data
	int reading1 = ADC0_InSeq3();
  TM4C_to_Blynk(74, reading1);  // VP74
  	
	if(getMode() == GRAPH_SENSORS_MODE)
	{
		putData(reading1);
	}
}

  
int main(void){       
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
  PortF_Init();
	
	ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
	Output_Init(); 
	
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\rBlynk example");
#endif
  
	ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C, 800000); 
  Timer3_Init(&SendInformation, 40000000); 
	PWM0B_Init(40000, 4000 * growLightDuty);
	EnableInterrupts();
  while(1) 
	{   
		switch(getMode())
		{
			case CLOCK_MODE:
			clockMode();
			break;
			
			case SET_ALARM_MODE:
		  setAlarmMode();	
			break;
		}
		
  }
}


