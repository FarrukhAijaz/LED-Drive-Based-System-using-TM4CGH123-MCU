#include "TM4C123GH6PM.h"
#include "adc_init.h"
#include "pwm_init.h"
#include "utils.h"
#include "display.h"
#include "LUX_Calculation.h"

extern int CH0(void);
extern int CH1(void);
extern void I2C3_Init(void);
extern void I2C3_Write(void);
extern void I2C3_Error_Check(void);
extern void I2C3_Read(void);
unsigned int luminosity;
float detected_value;
unsigned int CH0_mean=0;
unsigned int CH1_mean=0;
float sample = 0;
float duty = 0;
float low_threshold = 200;
float high_threshold = 700;
int state = -1;
char msg1[32];	
char msg2[32];
char msg3[32];
char msg4[32];
char line[32] = "---------\r\4";

int get_lum_data(void);
void check_the_state(void);
float get_pot_data(void);

int get_lum_data(void){
		 int det_value = 0;
			for (int counter=0; counter<128; ++counter)
			{
					CH0_mean += CH0();
					CH1_mean += CH1();
			}
			CH0_mean=CH0_mean/128;
			CH1_mean=CH1_mean/128;
			luminosity = CalculateLux(1u, 2u, CH0_mean, CH1_mean, 1);
			//CH0_mean=0;
			//CH1_mean=0;
			det_value = (float) luminosity;
			return det_value;
}

float get_pot_data(void){
	
		// Start sampling (sequencer 3)
		ADC0->PSSI |= (1<<3);

		// Check if the sequence is complete
		if(ADC0->RIS & 0x8){	
			
			// Once sampling is complete on sequencer 3
			// the value pf the oldest sample will be in
			// ADC Sample Sequence Result FIFO 3 register
			sample = ADC0->SSFIFO3;
			
			sample *= 1500;
			sample /= 4095;
			
			// Continue sampling
			ADC0->ISC |= 0x8;
		
		}
		
		return sample;

}

void check_the_state(void){

	if((~(GPIOF->DATA) & 0x1) == 0x1){
				state *= -1;
				delay_100ms();
				delay_100ms();
					
				while((~(GPIOF->DATA) & 0x1) == 0x1){}

				delay_100ms();
				delay_100ms();
				delay_100ms();
			
			}

}



int main()
{
	// Initialization functions
	adc_init();
	switch_init();
	SSI0_init();
	display_init();
	leds_init();
	I2C3_Init();
	I2C3_Write();
	pwm_init_M0PWM0();
	
	// Clear the screen 
	clear_screen();
	clear_screen();
	
	// Print Labels on the screen
	configure_screen();
		
	while(1){
		
			// Read potentiometer input (ADC)
			sample = get_pot_data();
		
			// Poll the SW1 (PF0) to change the state
			// state = 1 -> changing low threshold is allowed
			// state = -1 -> changing high threshold is allowed
			check_the_state();
			
			// Update the thrsholds
			if(state == 1){
				if(sample<high_threshold){
				low_threshold = sample;
					delay_100ms();
				}
			
			}
			else if(state == -1){
				if(sample>low_threshold){
				high_threshold = sample;
					delay_100ms();
				}
			
			}
			
			//Get the sensor data from TSL2561 (I2C)
			detected_value = get_lum_data();
			
			// Update the Duty Cycle 
			duty = change_duty_cycle_M0PWM0(detected_value, low_threshold, high_threshold);

			//Setting Indicator LEDs
			if(detected_value>high_threshold){
			GPIOF->DATA|=0x04;
			GPIOF->DATA&=~0x08;
			GPIOF->DATA&=~0x02;
			}
			else if(detected_value<low_threshold){
			GPIOF->DATA|=0x02;
			GPIOF->DATA&=~0x04;
			GPIOF->DATA&=~0x08;
			}
			else{
			GPIOF->DATA|=0x08;
			GPIOF->DATA&=~0x02;
			GPIOF->DATA&=~0x04;
			}
			//Display the data to Nokia LCD
			print_low_data(low_threshold);
			print_high_data(high_threshold);
			print_lux_data(detected_value);
			print_state(state);
			//Monitor the data through Termite
			sprintf(msg1, "LT: %s \r\4", convert_to_char_array_XYZ(low_threshold));
			OutStr(msg1);
			sprintf(msg2, "HT: %s \r\4", convert_to_char_array_XYZ(high_threshold));
			OutStr(msg2);
			sprintf(msg3, "LUX: %s \r\4", convert_to_char_array_XYZ(detected_value));
			OutStr(msg3);
			OutStr(line);

			
	}
}

