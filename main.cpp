/*
* hive_programming.cpp
*
* Created: 1/10/2018 11:44:59 PM
* Author : Jesse
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>

#include "periph.h"
#include "init.h"
#include "lookup.h"
#include "i2cmaster.h"
#include "filter.h"
#include "lcd.h"

uint8_t k = 0x00;
uint16_t display = 0x0000;
int lookup = 0;
char lcd_string_array[32];
char* status_display = &lcd_string_array[0];
char* count_display  = &lcd_string_array[16];

int16_t concat_le(uint8_t const low, uint8_t const high) {
	return static_cast<int16_t>((high << 8) | low);
}

struct Accelerometer {
	int16_t x, y, z;
	
	static uint8_t const READ_ADDR = (ACCEL_DATA & 0x3F) | READ;
	
	
	
  
	//*******************************************************************************//
  //																Accelerometer()
	//Configures the LIS3DH. It enables the intterupt in active low and sets a 
	//1.334kHz sampling rate, which is needed to ensure that the signal is sampled
	//above the Nyquist rate. It further enables high resolution, 12-bit data mode
	//for a smoother sine wave when passed to the DAC. Also, it ensures auto-increment
	//of the registers once they have been read. 
	//*******************************************************************************//
	Accelerometer() {
		configure(CTRL_REG3, CTRL3_VAL); //enable INT1
		configure(CTRL_REG1, CTRL1_VAL); //set data rate (1.344kHz)
		configure(CTRL_REG4, CTRL4_VAL); //enable high resolution
		configure(CTRL_REG6, CTRL6_VAL); //INT polarity (active low)
	}//Accelerometer

  //*******************************************************************************//
	//             void configure(uint8_t reg_addr, uint8_t write_data)
	//This function is responsible for writing the configuration commands to the 
	//accelerometer. 	This is ran only when the constructor is called in main. 
  //*******************************************************************************//
	void configure(uint8_t reg_addr, uint8_t write_data) {
		uint8_t write_byte = (reg_addr & 0x7F) | WRITE;
		// Execute 16-bit transfer
		PORTB &= ~(1 << 2);                   //set SS pin low to initiate data transfer
		SPDR = write_byte;                    //write instruction and address
		while (bit_is_clear(SPSR, SPIF)) {}   //wait until the transfer is complete (SPIF set)
		SPDR = write_data;                    //transfer data into second byte via SPDR
		while (bit_is_clear(SPSR, SPIF)) {}   //wait until the transfer is complete
		PORTB |= (1 << 2);                    //set SS high
	}
  //*******************************************************************************//
	//                                void update()
	//This function gathers data from the accelerometer and stores it in the 
	//respective member of the struct (x, y and z axes). 
  //*******************************************************************************//

	void update() {
		PORTB &= ~(1 << 2);	//set SS pin low to initiate data transfer
		uint8_t low_byte;
		
		SPDR = READ_ADDR;
		while (bit_is_clear(SPSR, SPIF)) {} //wait until the transfer is complete

		int16_t * member {&x};
		
		for(uint8_t i=0; i<3; i++) {
			SPDR = 0x00;						//send out dummy data to retrieve X_L data
			while (bit_is_clear(SPSR, SPIF)) {} //wait until the transfer is complete
			low_byte = SPDR;
			SPDR = 0x00;						//send out dummy data to retrieve X_H data
			while (bit_is_clear(SPSR, SPIF)) {} //wait until the transfer is complete
			*member = concat_le(low_byte, SPDR);
			member++;
		}//for
		PORTB |= (1 << 2);	//set SS high
	}//update()
};

/*******************************************************************************************/
//								         ISR(ADC_vect)
//This ADC handles the input voltage from the battery and implements an output should it
//fall below a specified voltage. For this application, the is 3.4V.
/*******************************************************************************************/
ISR(ADC_vect) {
	if (ADC <= 127) { //illuminate LED if voltage at the pin is less than or equal to 3.4V
		PORTB |= (1 << PB0);
	}
	else PORTB &= (0 << PB0);
	
	ADCSRA |= (1 << ADSC); //restart	
}

/**************************************************************/
//								         int main()
/**************************************************************/
int main() 
{
		bool counting_whoops = false;
		//static uint8_t j = 0;
		int16_t temp_z = 0x00;
		int16_t threshold = 30;
		int16_t neg_thresh = -30;
		int16_t z_data = 0x00;
		int8_t z_data_h = 0x00;
		int8_t z_data_l = 0x00;	
		bool status = false;	
		
		static int16_t loop_count, whoop_check, whoop_count = {0x00};
		int16_t min_whoops = 0x1E; //30		
		
		//Initialization routine
		spi_init();
		adc_init();
		i2c_init();
		lcd_init(LCD_DISP_ON);
		sei();
		

		Accelerometer accelerometer{};
		
		DDRB |= (1 << PB0) | (0 << PB1);  //PB0 to output and PB1 to input
		DDRC |= 0x02                      //PORTC used to control power to op-amp.
		DDRD  = 0xFF;					            //PORTD to output 
		
		PORTC &= ~(1 << PC1); //This is optional. Leave low to maintain power further down in code.
		lcd_clrscr(); //clear and set cursor to home
		lcd_puts("Whoop Count:0"); //Display
		
		while(1) {	
      //While the accelerometer is gathering data, write data to DAC			
			while(check_bit(PINB, 1)) {	
					i2c_start_wait(MCP4725_ADDR);
					i2c_write(0x40);	 // set A0 high for compare
					i2c_write(z_data_h); // the 8 most significant bits
					i2c_write(z_data_l); // the 4 least significant bits
					i2c_stop();
					PORTC |= (1 << PC1);
			}

			if (!check_bit(PINB, 1)) { //When the accelerometer is ready to update. Update.
				accelerometer.update();
			} 
		
			temp_z = (accelerometer.z >> 4) & 0x0FFF; //Shift to right align and ensure upper nibble is 0x0	
			
			//If the sign bit is present, make the sign bit a 0. Make the upper byte
			// all 1 to represent the two's complement correctly
			
			if(check_bit(temp_z, 11))
			{
				temp_z |= 0xF000;
			}
			
			z_data = DSP_filter(temp_z);   //Pass to filter
			
			//Count the number of whoops
			if((!counting_whoops && (z_data >= threshold)) || (z_data <= neg_thresh)) {
				counting_whoops = true; //truth
			}		
				
			if(counting_whoops) {
				loop_count++; //we've counted a sampling cycle
				if(loop_count >= 268) {
					if(whoop_check >= min_whoops) {
						//PORTC &= ~(1 << PC1);
						whoop_count++; // increment the whoop_counter which is sent to 7segment display.
						counting_whoops = false; // stop counting whoops until the threshold is exceeded again.
						whoop_check = 0;
						sprintf(count_display, "Whoop Count:%d", whoop_count);
						strncpy(lcd_string_array, count_display, 16);
						lcd_clrscr();
						lcd_puts(lcd_string_array);
						
					}
					loop_count = 0;
				}

					else if(z_data > threshold) {whoop_check++;}
			}
				
			z_data = (z_data + 2048) << 4; //lift up data from negative number
			z_data_h = z_data >> 8;	//separate into two bytes for i2c
			z_data_l = z_data;
				
		}//while
}//main
