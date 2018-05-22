/*******************************************************************************************/
//											spi_init()
//Initializes the SPI protocol. Enables SS MOSI and SCK, which will be used to configure
//ports in the accelerometer and retrieve data from them.
/*******************************************************************************************/
void spi_init() {
	DDRB |= 0b00101100; //turn on SS, MOSI, SCK
	SPCR |= (1 << SPE) | (1 << MSTR); //16MHz/4 speed
	SPSR |= (0 << SPI2X); //SPI 2X (1 << SPI2X)
}

/*******************************************************************************************/
//											adc_init()
/*******************************************************************************************/
void adc_init() {
	ADMUX  |=  (0 << REFS1) | (1 << REFS0) | (1 << MUX1) | (1 << MUX0); //AVcc and using ADC3
	ADCSRA |= (1 << ADEN) | (1 << ADIE); //enable ADC and its interrupt
	ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //set a 128 pre-scale
	ADCSRA |= (1 << ADSC); //start conversion
}

