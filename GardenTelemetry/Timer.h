// Timer.h
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#ifndef __TIMER0AINTS_H__ // do not include more than once
#define __TIMER0AINTS_H__

// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f

// ***************** Timer0_Init *********************
// Activate Timer0A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0_Init(void(*task)(void), uint32_t period);

// ***************** Timer1_Init *********************
// Activate Timer1A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer1_Init(void(*task)(void), uint32_t period);

// ***************** Timer2_Init *********************
// Activate Timer2A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer2_Init(void(*task)(void), uint32_t period);


// ***************** Timer3_Init *********************
// Activate Timer3A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer3_Init(void(*task)(void), uint32_t period);


// ***************** Timer0_SetPeriod ****************
// Sets the new period for Timer0A
// Inputs: 	new period of the interrupt
// Outputs: None
void Timer0_SetPeriod(uint32_t period);

// ***************** Timer1_SetPeriod ****************
// Sets the new period for Timer1A
// Inputs: 	new period of the interrupt
// Outputs: None
void Timer1_SetPeriod(uint32_t period);

// ***************** Timer2_SetPeriod ****************
// Sets the new period for Timer2A
// Inputs: 	new period of the interrupt
// Outputs: None
void Timer2_SetPeriod(uint32_t period);

// ***************** Timer2_SetPeriod ****************
// Sets the new period for Timer2A
// Inputs: 	new period of the interrupt
// Outputs: None
void Timer3_SetPeriod(uint32_t period);

#endif // __TIMER0AINTS_H__
