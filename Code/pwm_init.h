/*Pulse_init.h file
Function for creating a pulse train using interrupts
Uses Channel 0, and a 1Mhz Timer clock (_TAPR = 15)
Uses Timer0A to create pulse train on PF2
*/

#include "TM4C123GH6PM.h"
void update_duty_cycle (float sample);
void leds_init(void);
void pwm_init_M0PWM0(void);
float change_duty_cycle_M0PWM0(float sensor_data, float LT, float HT);

volatile int LOW 	= 4095;		
volatile int HIGH  =	2047;	

char line_display[32] 				= "----------------------\r\4";  

void leds_init(void){

	SYSCTL->RCGCGPIO |= 0x20; 					// Turn on bus clock for GPIOF
	__ASM("NOP");
	__ASM("NOP");
	__ASM("NOP");
	GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR  |= 0x01;
  GPIOF->DIR				|= 0x0E; 					// Set PF1 PF2 PF3 as output and PF0 as input
  GPIOF->AFSEL			&= (0xFFFFFFF7);  // Regular port function
	GPIOF->PCTL				&= 0xFFFF000F;  	// No alternate function
	GPIOF->AMSEL			=0; 							// Disable analog
	GPIOF->DEN				|=0x1F; 					// Enable port digital
	GPIOF->PUR  |= 0x01;
}

void pwm_init_M0PWM0(void){

    /* Clock setting for PWM and GPIO PORT */
    SYSCTL->RCGCPWM |= 1;       /* Enable clock to PWM0 module */
    SYSCTL->RCGCGPIO |= 0x04;   /* Enable system clock to PORTC */
    SYSCTL->RCC |= (1<<20);
		SYSCTL->RCC |= 0x000E0000; /* Directly feed clock to PWM1 module without pre-divider */

    /* Setting of PC4 pin for M1PWM0 channel output pin */
    GPIOC->DIR |=0x10;				//PC4 is set as output
		GPIOC->AFSEL |= 0x10;   /* PC4 sets an alternate function */
    GPIOC->PCTL &= ~0x000F0000; /* Set PC4 as output pin */
    GPIOC->PCTL |= 0x00040000;  /* Make PC4 PWM output pin */
    GPIOC->DEN |= 0x10;     /* Set PD0 as a digital pin */

    /* PWM1 channel 6 setting */
    PWM0->_3_CTL &= ~(1 << 0);   /* Disable Generator 3 counter */
    PWM0->_3_CTL &= ~(1 << 1);   /* Select down count mode of counter 3 */
    PWM0->_3_GENA = 0x0000008C;  /* Set PWM output when counter reloaded and clear when matches PWMCMPA */
    PWM0->_3_LOAD = 5000;       /* Set load value for 1kHz (16MHz/16000) */
    PWM0->_3_CMPA = 2500-1;    /* Set duty cycle to 50% by loading of 16000 to PWM1CMPA */
    PWM0->_3_CTL = 1;            /* Enable Generator 3 counter */
    PWM0->ENABLE = 0x40;         /* Enable PWM1 channel 6 output */	
}

float change_duty_cycle_M0PWM0(float sensor_data, float LT, float HT){


	
		volatile int duty_cycle=0;

		duty_cycle = PWM0->_3_CMPA;
	
		if((PWM0->_3_CMPA> 100) && (PWM0->_3_CMPA < 4800)){
	
	
			if(sensor_data>HT){
			// Decrease the PWM (blue region)
			duty_cycle -= 90;
			}
			else if(sensor_data<LT){
				// Increase the PWM (red region)
			duty_cycle += 90;
			}
			else{
				// (green region)
			duty_cycle = 2500;
			}
			
			PWM0->_3_CMPA = duty_cycle;
		}
		else if(PWM0->_3_CMPA <= 100){
		duty_cycle += 200;
			PWM0->_3_CMPA = duty_cycle;
		}
		else if(PWM0->_3_CMPA >= 4800){
		duty_cycle -= 200;
			PWM0->_3_CMPA = duty_cycle;
		}


		return (float) duty_cycle;
}