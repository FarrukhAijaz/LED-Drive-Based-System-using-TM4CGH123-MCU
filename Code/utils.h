#include "TM4C123GH6PM.h"

void delay(void);
char* convert_to_char_array_XYZ(float float_value);
void delay_100ms(void);
void switch_init(void);

char* convert_to_char_array_XYZ(float float_value)
{
	// Initialize the array to be returned
	char* char_array[12];
	
	// Initialize the variables
	float fraction, decimal;
	int frac_part, dec_part; 
	
	float_value *=100;
	//float_value *= 806;
	//float_value /= 1000000;
	
	frac_part = (int) float_value;
	frac_part %= 100;
	
	frac_part = abs(frac_part);
	
	// Seperate the decimal part and the fraction part
	decimal = floor(float_value);


	// Convert the numbers to integer
	dec_part = (int) decimal;

	dec_part /= 100;
	
	// Append the integers to the char array
	sprintf(char_array, "%d,%d", dec_part, frac_part);
	
	// return the char array
	return char_array;
}

// 0.1 second delay with "for" loop
void delay(void){
	
	int i;
	// The value 850000 provides the 1-second delay 
	for(i=0; i<85000; i++){
		__NOP();
		__NOP();
		__NOP();
	}
}

void switch_init(void){
	
	SYSCTL->RCGCGPIO |= 0x20; 					// Turn on bus clock for GPIOF
	__ASM("NOP");
	__ASM("NOP");
	__ASM("NOP");
	
	GPIOF->DIR				&= 0x0E; 					// Set PF4 and PF0 as input
  GPIOF->AFSEL			&= (0xFFFFFFF7);  // Regular port function
	GPIOF->PCTL				&= 0xFFF00000;  	// No alternate function
	GPIOF->AMSEL			=0; 							// Disable analog
	GPIOF->DEN				|=0x10; 					// Enable port digital PF4 and PF0
	
}

// 0.1 second delay with Timer1
void delay_100ms(void){
	
	// Enable clock for Timer1
	 SYSCTL->RCGCTIMER |=2;
	 
	// Set CTL to 0 to prevent it running unintentionally
	 TIMER1->CTL 		|= 0;
	// Set CFG to 0x4 to enable 16-bit Mode (Split Timer)
	 TIMER1->CFG 		|= 0x4;
	// TAMR [1:0] Periodic, Count Down
	 TIMER1->TAMR 	|= 0x2;
	// TAILR specifies the value from which the count-down begins
	 TIMER1->TAILR 	|= 64000-1;
	// Prescaler
	 TIMER1->TAPR		|= 25-1;
	// Clear the TATORIS bit in RIS
	 TIMER1->ICR 		|= 0x1;
	// Set TAEN [0] to enable the Timer
	 TIMER1->CTL 		|= 0x1;
	 
	 while((TIMER1->RIS & 0x1)==0);
	
	// Clear the interrupt flag
	 TIMER1->ICR = 0x01;
}
