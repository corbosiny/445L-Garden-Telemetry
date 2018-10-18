#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

#define CLOCK_MODE								0
#define SET_ALARM_MODE					  1

static int alarmIsArmed = 0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void setMode(int mode);

int getMode(void);

void printModeTitle(char *title);

void printAlarmStatus(char *status);

void clockMode(void);

void setAlarmMode(void);

void setOffAlarm(void);

void disableAlarm(void);

void updateTime(void);

void displayCurrentTime(int currentHours, int currentMinutes, int currentSeconds, char* meridiem);

void setMinute(int newMinute);
void setHour(int newHour);
void toggleMerridian(void);

void initClock(int clockX, int clockY);

void displayClockHands(int currentHours, int currentMinutes);

void calculateHourHandPosition(int endingPoint[2], int currentHours);

void calculateMinuteHandPosition(int endingPoint[2], int currentMinutes);

void calculateSecondHandPosition(int endingPoint[2], int currentSeconds);

void calculateEndingPoint(int endingPoint[2], int radius, float percentageThrough);

#endif

