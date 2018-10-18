#include <stdio.h>
#include "clock.h"
#include <math.h>
#include "ST7735.h" //driver for the LCD
#include "fixed.h"  //public definitions of our fixed point functions
#include "bitmaps.h"
#include "PWM.h"
#include "Timer.h"
#include "../inc/tm4c123gh6pm.h"

#define HAND_COLOR ST7735_WHITE
#define PI 3.14159265
#define map(input, minFirst, maxFirst, minSecond, maxSecond) ((input - minFirst) * (maxSecond - minSecond) / (maxFirst - minFirst) + minSecond)

#define TIME_X_CURSOR 6
#define TIME_Y_CURSOR 9

#define TITLE_X 5
#define TITLE_Y 12

#define ALARM_X 5
#define ALARM_Y 14

#define CLOCK_TITLE "Clock Mode"
#define SET_ALARM_TITLE "Setting Alarm"

int currentMode = CLOCK_MODE;
int initMode = 1;

const int HOUR_HAND_SIZE = 17;
const int MINUTE_HAND_SIZE = 	25;
int clockOrigin[2];
int clockCenter[2];

int minutes = 59;
int seconds = 40;
int hours = 11;

int timerHours = 0;
int timerMinutes = 3;
int timerSeconds = 0;

int prevHours =  11;
int prevMinutes = 59;
int prevSeconds = 50;

int topHours = 0;
int topMinutes = 3;

int alarmSeconds = 0;
int alarmMinutes = 0;
int alarmHours = 12;
int timeChanged = 0;
int minuteChanged = 0;

char am[] = "am";
char pm[] = "pm";
char *merridian = am;
char *alarmMerridian = am;
char *prevMerridian = am;

int getMode()
{
	return currentMode;
}

void setMode(int newMode)
{
	if(newMode != currentMode) {initMode = 1;}
	currentMode = newMode;
}

void printModeTitle(char *title)
{
	ST7735_SetCursor(TITLE_X, TITLE_Y);
	ST7735_OutString(title);
}

void clockMode()
{
	if(initMode == 1)
	{
		initMode = 0;
		ST7735_FillScreen(ST7735_BLACK); 
		ST7735_SetCursor(0,0);
		initClock(32, 64);
		printModeTitle(CLOCK_TITLE);
		DisableInterrupts();
		alarmHours = hours;
		alarmMinutes = minutes;
		alarmSeconds = seconds;
		alarmMerridian = merridian;
		hours = prevHours;
		minutes = prevMinutes;
		seconds = prevSeconds;
		merridian = prevMerridian;
		Timer0_Init(&updateTime, 80000000);
		ST7735_DrawBitmap(clockOrigin[0], clockOrigin[1], clock, clockSize, clockSize);
		displayClockHands(hours, minutes);
		timeChanged = 1;
		EnableInterrupts();
	}
	//configure clock time
	if(timeChanged == 1)
	{
		DisableInterrupts();
		int currentSeconds = seconds;
		int currentMinutes = minutes;
		int currentHours = hours;
		timeChanged = 0;
		EnableInterrupts();
		displayCurrentTime(currentHours, currentMinutes, currentSeconds, merridian);
		if(currentHours == alarmHours && currentMinutes == alarmMinutes && currentSeconds == alarmSeconds)
		{
			setOffAlarm();
		}
	}
		
	if(minuteChanged)
	{
		ST7735_DrawBitmap(clockOrigin[0], clockOrigin[1], clock, clockSize, clockSize);
		DisableInterrupts();
		int currentMinutes = minutes;
		int currentHours = hours;
		minuteChanged = 0;
		EnableInterrupts();
		displayClockHands(currentHours, currentMinutes);
	}
		 	
}

void setAlarmMode()
{	if(initMode == 1)
	{
		DisableInterrupts();
		initMode = 0;
		ST7735_FillScreen(ST7735_BLACK); 
		ST7735_SetCursor(0,0);
		initClock(32, 64);
		printModeTitle(SET_ALARM_TITLE);
		prevHours = hours;
		prevMinutes = minutes;
		prevSeconds = seconds;
		prevMerridian = merridian;
		hours = alarmHours;
		minutes = alarmMinutes;
		seconds = alarmSeconds;
		merridian = alarmMerridian;
		ST7735_DrawBitmap(clockOrigin[0], clockOrigin[1], clock, clockSize, clockSize);
		displayClockHands(hours, minutes);
		timeChanged = 1;
		EnableInterrupts();
	}
	//configure clock time
	if(timeChanged == 1)
	{
		DisableInterrupts();
		int currentSeconds = seconds;
		int currentMinutes = minutes;
		int currentHours = hours;
		timeChanged = 0;
		EnableInterrupts();
		ST7735_DrawBitmap(clockOrigin[0], clockOrigin[1], clock, clockSize, clockSize);
		displayCurrentTime(currentHours, currentMinutes, currentSeconds, merridian);
		displayClockHands(currentHours, currentMinutes);
	}
		
}

void initClock(int clockX, int clockY)
{
	clockOrigin[0] = clockX;
	clockOrigin[1] = clockY;
	clockCenter[0] = clockX + (clockSize / 2);
	clockCenter[1] = clockY - (clockSize / 2);
}

void setMinute(int newMinute)
{
	DisableInterrupts();
	minutes = newMinute;
	timeChanged = 1;
	EnableInterrupts();
}

void setHour(int newHour)
{
	DisableInterrupts();
	hours = newHour;
	timeChanged = 1;
	EnableInterrupts();
}

void toggleMerridian()
{
	DisableInterrupts();
	if(merridian[0] == 'a' && merridian[1] == 'm') {merridian = pm;}
	else{merridian = am;}
	timeChanged = 1;
	EnableInterrupts();
}

void displayClockHands(int currentHours, int currentMinutes)
{
	int hourHandEndingPoint[2];
	calculateHourHandPosition(hourHandEndingPoint, currentHours);
	int minuteHandEndingPoint[2];
	calculateMinuteHandPosition(minuteHandEndingPoint, currentMinutes);
	ST7735_Line(clockCenter[0], clockCenter[1], minuteHandEndingPoint[0], minuteHandEndingPoint[1], HAND_COLOR);
	ST7735_Line(clockCenter[0], clockCenter[1], hourHandEndingPoint[0], hourHandEndingPoint[1], HAND_COLOR);
}

void calculateHourHandPosition(int endingPoint[2], int currentHours)
{
	float percentageThrough = currentHours / 12.0;
	calculateEndingPoint(endingPoint, HOUR_HAND_SIZE, percentageThrough);
}

void calculateMinuteHandPosition(int endingPoint[2], int currentMinutes)
{
	float percentageThrough = currentMinutes / 60.0;
	calculateEndingPoint(endingPoint, MINUTE_HAND_SIZE, percentageThrough);
}

void calculateEndingPoint(int endingPoint[2], int radius, float percentageThrough)
{
	float angle = (360.0 * percentageThrough + 90.0) * -1 * PI / 180.0;
	int x = clockCenter[0] - (cos(angle) * radius);
	int y = clockCenter[1] +  (sin(angle) * radius); 
	endingPoint[0] = x;
	endingPoint[1] = y;
}

void printAlarmStatus(char *title)
{
	ST7735_SetCursor(ALARM_X, ALARM_Y);
	ST7735_OutString(title);
}

void setOffAlarm()
{
	PWM0A_Init(40000, 30000);  
}

void disableAlarm()
{
	SYSCTL_RCGCPWM_R &= ~(0x01);   
}

void swapMerridian()
{
	if(merridian[0] == 'a' && merridian[1] == 'm') {merridian = pm;}
	else{merridian = am;}
}

void updateTime()
{
	DisableInterrupts();
	seconds++;
	if(seconds == 60) {seconds = 0; minutes++; minuteChanged = 1;}
	if(minutes == 60) {minutes = 0; hours++; if(hours == 12){swapMerridian();};}
	if(hours == 13) {hours = 1; }
	timeChanged = 1;
	EnableInterrupts();
}

void displayCurrentTime(int currentHours, int currentMinutes, int currentSeconds, char* merridian)
{
			ST7735_SetCursor(TIME_X_CURSOR, TIME_Y_CURSOR);
			char timeBuffer[10];
			if(currentHours < 10)
			{
				if(currentMinutes < 10) 
				{
					if(currentSeconds > 9) {sprintf(timeBuffer, "0%d:0%d:%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
					else{sprintf(timeBuffer, "0%d:0%d:0%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
				}
				else
				{
					if(currentSeconds > 9) {sprintf(timeBuffer, "0%d:%d:%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
					else{sprintf(timeBuffer, "0%d:%d:0%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
				}
			}
			else
			{
				if(currentMinutes < 10) 
				{
					if(currentSeconds > 9) {sprintf(timeBuffer, "%d:0%d:%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
					else{sprintf(timeBuffer, "%d:0%d:0%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
				}
				else
				{
					if(currentSeconds > 9) {sprintf(timeBuffer, "%d:%d:%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
					else{sprintf(timeBuffer, "%d:%d:0%d%s", currentHours, currentMinutes, currentSeconds, merridian);}
				}
			}
			ST7735_OutString(timeBuffer);
}
