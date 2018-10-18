#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ST7735.h"

#define decimalPlace	3
#define printWidth	6
#define	upperRange	10000
#define lowerRange	-10000
#define yStartDraw	32
#define xStartDraw	0
/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99.99 to +99.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " **.**"
  2345    " 23.45"  
 -8100    "-81.00"
  -102    " -1.02" 
    31    "  0.31" 
-12345    "-**.**"
 */ 
void ST7735_sDecOut2(int32_t n){
	bool negFlag = n < 0;
		
	if(n >= upperRange){
		printf(" **.**");
	}else if(n <= lowerRange){
		printf("-**.**");
	}else{
		char fixedNum[printWidth];
		
		n = abs(n);
		for(int i = printWidth - 1; i >= 0; i--){
			if(i == decimalPlace){
				fixedNum[i] = '.';
			}else{
				if((n != 0) || (i >= (decimalPlace - 1))){
					fixedNum[i] = (n % 10) + '0';
					n = n / 10;
				}else{
					fixedNum[i] = (negFlag) ? '-' : ' ';
					for(int j = i - 1; j >= 0; j--){
						fixedNum[j] = ' ';
					}
					i = -1;
				}
			}
		}
		ST7735_OutString(fixedNum);
	}
		
}


/**************ST7735_uBinOut6***************
 unsigned 32-bit binary fixed-point with a resolution of 1/64. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 63999, it signifies an error. 
 The ST7735_uBinOut6 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     1	  "  0.01"
    16    "  0.25"
    25	  "  0.39"
   125	  "  1.95"
   128	  "  2.00"
  1250	  " 19.53"
  7500	  "117.19"
 63999	  "999.99"
 64000	  "***.**"
*/
void ST7735_uBinOut6(uint32_t n){
	if(n >= 64000){
		printf("***.**");
	}else{
		n = (n * 100) / 64;
		char fixedNum[printWidth];
		
		for(int i = printWidth - 1; i >= 0; i--){
			if(i == decimalPlace){
				fixedNum[i] = '.';
			}else{
				if((n != 0) || (i >= (decimalPlace - 1))){
					fixedNum[i] = (n % 10) + '0';
					n = n / 10;
				}else{
					for(int j = i; j >= 0; j--){
						fixedNum[j] = ' ';
					}
					i = -1; 
				}
			}
		}
		ST7735_OutString(fixedNum);
	}
}

int32_t xMin;
int32_t xMax;
int32_t yMin;
int32_t yMax;

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	xMin = minX;
	xMax = maxX;
	yMin = minY;
	yMax = maxY;
	
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_FillRect(0, 32, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, ST7735_WHITE);
	ST7735_SetCursor(0,0);
	ST7735_OutString(title);
}

/**************resize***************
 Remaps the data point from one range to another
 Inputs:  point  		original data point
          newMax		upper bound of the new range for the resized point   
          newMin		lower bound of the new range for the resized point
					oldMax		upper bound of the original data point
					oldMin		lower bound of the original data point
 Outputs: int32_t value of the point under the new range
*/
int32_t resize(int32_t point, int32_t newMax, int32_t newMin, int32_t oldMax, int32_t oldMin){
	return ((point - oldMin) * (newMax - newMin)) / ((oldMax - oldMin) + newMin);
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int32_t resizedX;
	int32_t resizedY;

	for(int i = 0; i < num; i++){
		if(bufX[i] <= xMax && bufX[i] >= xMin && bufY[i] <= yMax && bufY[i] >= yMin){
			resizedX = resize(bufX[i], ST7735_TFTWIDTH, xStartDraw, xMax, xMin);
			resizedY = resize(bufY[i], ST7735_TFTHEIGHT, yStartDraw, yMax, yMin);
			ST7735_DrawPixel(resizedX, ST7735_TFTHEIGHT - resizedY, ST7735_BLUE);		//yaxis needs to be inverted do to origin being at the top left
		}
	}
}

int32_t abs1(int32_t x)
{
	if(x > 0){return x;}
	else{return -x;}
}

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	int Ydiff = y2 - y1;
	int Xdiff = x2 - x1;
	int currentX = x1;
	int currentY = y1;
	
	int stepNumber = 1;
	int signOfStep = 0;
	if(abs1(Xdiff) > abs1(Ydiff))
	{
		signOfStep = Xdiff / abs(Xdiff);
		while (currentX != x2)
		{
			ST7735_DrawPixel(currentX, currentY, color);
			currentX += signOfStep;
			currentY = (y1 * Xdiff + signOfStep * Ydiff * stepNumber++) / (Xdiff); 
		}
	} 
	else 
	{
		signOfStep = Ydiff / abs(Ydiff);
		while(currentY != y2)
		{
			ST7735_DrawPixel(currentX, currentY, color);
			currentX = (x1 * Ydiff + signOfStep * Xdiff * stepNumber++) / (Ydiff);
			currentY += signOfStep;
		}
	}
}
