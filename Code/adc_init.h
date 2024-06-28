#include "TM4C123GH6PM.h"

void adc_init(void);

void adc_init(void)
{
	
	// Power up one one of the ATD modules
	// In order to use ADC0 set bit 0 to 1
	SYSCTL->RCGCADC |= 0x1;
	
	// Wait for stabilization
	__ASM("NOP");
	__ASM("NOP");
	__ASM("NOP");
	
	// Enable Clock for Port B  
	SYSCTL->RCGCGPIO |= 0x2;
	
	// Wait for stabilization
	__ASM("NOP");
	__ASM("NOP");
	__ASM("NOP");
	
	// The Alternate Function register tells the TM4C 
	// that we won't be using PB4 as a simple on/off switch
	// Set the 4th bit (PB4)
	GPIOB->AFSEL 	|= 	(1<<4);
	GPIOB->DIR 		&= 	~(1<<4);
	GPIOB->DEN 		|= 	(1<<4);
	GPIOB->AMSEL 	|= 	(1<<4);
	
	
	// To diable sequencer 3, cclear bit 3 of the ADCACTSS
	ADC0->ACTSS 	&= 	~(1<<3);
	
	// ADCEMUX configures what triggers the sequencer
	// We will be triggering each sample in software
	// Clear bits 15:12
  ADC0->EMUX  	&= ~0xF000;
	
	// Stop samploing after one sample END0 bit 
	ADC0->SSCTL3 |=(1<<1);
	// Set flags in the RIS register
	ADC0->SSCTL3 |=(1<<2);
	
	// Indicate which channel to use for the ATD module (AIN10)
	ADC0->SSMUX3	|= 0x0A;
	
	// ATD sampling rate
	ADC0->PC			|= 0x01;
	
	// ATD system is ready to start sampling on our trigger (S3)
	ADC0->ACTSS |= (1<<3);
}
