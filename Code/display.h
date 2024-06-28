#include "TM4C123GH6PM.h"
#include <stdint.h>

#define SSI_SR_BSY	0x00000010
#define SSI_SR_TNF	0x00000002 
#define SSI0_SR (*((volatile uint32_t*)0x4000800C))
#define SSI0_DR (*((volatile uint32_t*)0x40008008))
#define SSI_SR_BSY (*((volatile uint32_t*)0x00000010))
	
enum operation {COMMAND, DATA}; 

void display_init(void);
void SSI0_init(void);
void delay_100ms(void);
void write_DR(uint8_t data);
void set_mode(enum operation MODE);
void clear_screen(void);
void set_cursor(uint8_t x_cursor, uint8_t y_cursor);
void print_number(int num, uint8_t x_cursor, uint8_t y_cursor);
void print_digit(int num);
void string_low();
void string_high();
void string_lux();
void configure_screen();
void print_low_data(int data);
void print_high_data(int data);
void print_lux_data(int data);
void print_heart();
void print_blank();
void print_state();
								 

void display_init(void){

    // RST pin will be connected to PA7
    // To initialize the display, we need to set RST to low, wait 100ms, 
    // set RST to high
    
    GPIOA->DATA &= 0x7F; // Set PA7 LOW

    delay_100ms();

    GPIOA->DATA |= 0x80; // Set PA7 HIGH

    // (Command Mode) D/C pin will be connected to PA6
    GPIOA->DATA &= 0xBF; // Clear PA6 D/C pin to low to enter command mode

    // SSI->DR: SSI Data Register
    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x21; // Function set: extended instruction set (H=1) (V=0)

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0xB0; // Set LCD Vop (Contrast)

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x04; // Temperature Coefficient: 2

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x13; // LCD bias 

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x20; // Function set: standard instruction set (H=0) (V=0)

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x0C; // Display control: normal mode (D=1) (E=0)

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x80; // Set X address of RAM to 0

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
    SSI0->DR = 0x40; // Set Y address of RAM to 0

    while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy

    return;
}

void SSI0_init(void){

    SYSCTL->RCGCSSI |= 0x1; // Enable SSI0

    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");

    SYSCTL->RCGCGPIO |= 0x3; // Enable GPIOA and GPIOB

    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");

    // Configure GPIOA
    GPIOA->DIR |= 0xEC; // Set PA2, PA3, PA5, PA6, PA7 as outputs and PA4 as input
    GPIOA->DEN |= 0xFC; // Enable PA2, PA3, PA4, PA5, PA6, PA7
    GPIOA->AFSEL |= 0x3C; // Enable alternate function for PA2, PA3, PA4, PA5
    GPIOA->PCTL |= 0x222200; // Set PA2, PA3, PA4, PA5 as SSI0
    GPIOA->AMSEL &= ~0xFC; // Disable analog function for PA2, PA3, PA4, PA5

    GPIOA->DIR |= 0x01; // Set PA0 as output
    GPIOA->DEN |= 0x01; // Enable PA0

    // Configure SSI
    // CR1: SSI Control 1
    SSI0->CR1 &= 0xFFFFFFFF; // For master mode, the SSICR1 register must be written with 0x0000.0000
    SSI0->CPSR |= 0x04; // Prescale value of 4
    // CR0: SSI Control 0
    // CR0[15:12] SSI Serial Clock Rate
    // This bit field is used to generate the transmit and receive bit rate of the SSI. 
    // The bit rate is: BR = SysClk / (CPSDVSR * (1 + SCR))
    SSI0->CR0 |= 0x0100; 
    // 8-bit data send/receive
    SSI0->CR0 |= 0x07; // 8bit data
    SSI0->CR0 &= ~0x30; // Clear SPH and SPO
    SSI0->CR1 |= 0x02; // Enable SSI

   // while((SSI0->SR & SSI_SR_BSY) == SSI_SR_BSY){} // Wait until SSI is not busy
			
			while ((SSI0->SR & 0x16) == 0){}

    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");

    return;
}

void set_mode(enum operation MODE){

	if(MODE == COMMAND){
		// (COMMAND Mode) D/C pin will be connected to PA6
    GPIOA->DATA &= 0xBF; // Clear PA6 D/C pin to low to enter command mode
	}
	else if(MODE == DATA){
		
		// (DATA Mode) D/C pin will be connected to PA6
		GPIOA->DATA |= 0x40; // Data mode on
	
	}
	while ((SSI0->SR & 0x10) != 0x00){};
}
void clear_screen(void){

		set_cursor(0,0);

		for (int i = 0; i < 504; i++){ 
        
			 while ((SSI0->SR & 0x10) != 0x00){};
			write_DR(0x00);
    }
		set_mode(DATA);
}

void set_cursor(uint8_t x_cursor, uint8_t y_cursor) {
	
	set_mode(COMMAND);
	write_DR(0x20);
    write_DR(0x80 | (x_cursor)); // X address
    write_DR(0x40 | y_cursor); // Y address
	
	while ((SSI0->SR & 0x10) != 0x00){}
	set_mode(DATA);
}

void write_DR(uint8_t data) {
    while ((SSI0->SR & 0x10) != 0x00){} // Wait until SSI is not busy
    SSI0_DR = data;
	while ((SSI0->SR & 0x10) != 0x00){}
}

void print_digit(int num) {

		if(num == 0)
		{
			//0
			write_DR(0x7C);
			write_DR(0x82);
			write_DR(0x82);
			write_DR(0x82);
			write_DR(0x7C);
			write_DR(0x00);
		}
		else if(num == 1)
		{
			//1
			write_DR(0x88);
			write_DR(0x84);
			write_DR(0xFE);
			write_DR(0x80);
			write_DR(0x80);
			write_DR(0x00);
		}
		else if(num == 2)
		{
			//2
			write_DR(0x84);
			write_DR(0xC2);
			write_DR(0xA2);
			write_DR(0x92);
			write_DR(0x8C);
			write_DR(0x00);
		}
		else if(num == 3)
		{
			//3
			write_DR(0x44);
			write_DR(0x82);
			write_DR(0x92);
			write_DR(0x92);
			write_DR(0x6C);
			write_DR(0x00);
		}
		else if(num == 4)
		{
			//4
			write_DR(0x20);
			write_DR(0x30);
			write_DR(0x28);
			write_DR(0x24);
			write_DR(0xFE);
			write_DR(0x00);
		}
		else if(num == 5)
		{
			//5
			write_DR(0x8E);
			write_DR(0x8A);
			write_DR(0x8A);
			write_DR(0x8A);
			write_DR(0x72);
			write_DR(0x00);
		}
		else if(num == 6)
		{
			//6
			write_DR(0x7C);
			write_DR(0xA4);
			write_DR(0x92);
			write_DR(0x92);
			write_DR(0x64);
			write_DR(0x00);
		}
		else if(num == 7)
		{
			//7
			write_DR(0x82);
			write_DR(0x42);
			write_DR(0x22);
			write_DR(0x12);
			write_DR(0x0E);
			write_DR(0x00);
		}
		else if(num == 8)
		{
			//8
			write_DR(0x44);
			write_DR(0xAA);
			write_DR(0x92);
			write_DR(0xAA);
			write_DR(0x44);
			write_DR(0x00);
		}
		else if(num == 9)
		{
			//9
			write_DR(0x0C);
			write_DR(0x12);
			write_DR(0x12);
			write_DR(0x12);
			write_DR(0x0A);
			write_DR(0xFC);
			write_DR(0x00);
		}
}

void print_number(int num, uint8_t x_cursor, uint8_t y_cursor){

	set_cursor(x_cursor,y_cursor);

	// Seperate the digits
	int hex_num = num;
	int thousands = hex_num / 1000; 
	int hundreds = hex_num / 100; 
	hundreds -= 10*thousands; 
	int ones = hex_num % 10;
	int tens = (hex_num - thousands * 1000 - hundreds * 100 - ones) / 10; 

	// Print the digits side by side
	print_digit(thousands);
	print_digit(hundreds);
	print_digit(tens);
	print_digit(ones);
}

void string_low(){
		set_mode(DATA);
		// Letter "L"
		write_DR(0xFE);
		write_DR(0x80);
		write_DR(0x80);
		write_DR(0x80);
		write_DR(0x80);
	
		write_DR(0x00);
		// Letter "O"
		write_DR(0x7C);
		write_DR(0x82);
		write_DR(0x82);
		write_DR(0x82);
		write_DR(0x7C);
		write_DR(0x00);
		// Letter "W"
		write_DR(0x7E);
		write_DR(0x80);
		write_DR(0x40);
		write_DR(0x80);
		write_DR(0x7E);
		write_DR(0x00);
		// ":"
		write_DR(0x28);
		write_DR(0x00);
}

void string_high(){
		set_mode(DATA);
		// Letter "H"
		write_DR(0xFE);
		write_DR(0x10);
		write_DR(0x10);
		write_DR(0x10);
		write_DR(0xFE);
	
		write_DR(0x00);
		// Letter "I"
		write_DR(0x82);
		write_DR(0x82);
		write_DR(0xFE);
		write_DR(0x82);
		write_DR(0x82);
		write_DR(0x00);
		// Letter "G"
		write_DR(0x7C);
		write_DR(0x82);
		write_DR(0x92);
		write_DR(0x92);
		write_DR(0x66);
		write_DR(0x00);
		// Letter "H"
		write_DR(0xFE);
		write_DR(0x10);
		write_DR(0x10);
		write_DR(0x10);
		write_DR(0xFE);
		write_DR(0x00);
		
		// ":"
		write_DR(0x28);
		write_DR(0x00);
}

void string_lux(){
		set_mode(DATA);
		// Letter "L"
		write_DR(0xFE);
		write_DR(0x80);
		write_DR(0x80);
		write_DR(0x80);
		write_DR(0x80);
	
		write_DR(0x00);
		// Letter "U"
		write_DR(0x7E);
		write_DR(0x80);
		write_DR(0x80);
		write_DR(0x80);
		write_DR(0x7E);
		write_DR(0x00);
		// Letter "X"
		write_DR(0xC6);
		write_DR(0x28);
		write_DR(0x10);
		write_DR(0x28);
		write_DR(0xC6);
		write_DR(0x00);
		// ":"
		write_DR(0x28);
		write_DR(0x00);
}

void configure_screen(){
	// Set cursor to (0,0)
	set_cursor(0,0);
	// Write "LOW"
	string_low();
	
	// Set cursor to (0,1)
	set_cursor(0,1);
	// Write "HIGH"
	string_high();
	
	// Set cursor to (0,2)
	set_cursor(0,2);
	// Write "LUX"
	string_lux();
}

void print_low_data(int data){
	print_number(data, 30, 0);
}

void print_high_data(int data){
	print_number(data, 30, 1);
}

void print_lux_data(int data){
	print_number(data, 30, 2);
}

void print_heart(){
        set_mode(DATA);
        // Heart
        write_DR(0x0C);
        write_DR(0x1E);
        write_DR(0x3F);
        write_DR(0x7E);
        write_DR(0xFC);
        write_DR(0x7E);
        write_DR(0x3F);
        write_DR(0x1E);
        write_DR(0x0C);
}

void print_blank(){
        set_mode(DATA);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
        write_DR(0x00);
}

void print_state(int state){
    if(state == 1){
        set_cursor(60,0);
        print_heart();
        set_cursor(60,1);
        print_blank();
    }
    else if(state == -1){
        set_cursor(60,1);
        print_heart();
        set_cursor(60,0);
        print_blank();
    }
}