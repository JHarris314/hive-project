//Define macros to be used
#define clr_bit(x,y)    (x&=~(1<<y))
#define set_bit(x,y)    (x|=(1<<y))
#define check_bit(x, y) (x&(1<<y))

#define WRITE  0b00000000
#define READ   0b11000000

//Define states for interrupt on LIS3DH
#define STATE_IDLE	 1	//waiting for next trigger
#define STATE_TRIG	 2	//new trigger received
#define STATE_FINISH 3	//data processed, wait for end of trigger signal

//Use these for polling of new data (INT1) signal from sensor
#define INT_PIN 	PINB
#define INT_DDR		DDRB
#define INT_CKT		PB1  
#define INT_TEST	INT_PIN & (1<<INT_CKT)	//true when INT1 is high, false when INT1 is low - INT1 is active LOW

//Define register addresses (LIS3DH)
#define STATUS_REG_AUX 0x07
#define WHO_AM_I       0x0F
#define CTRL_REG1      0x20
#define CTRL_REG3      0x22
#define CTRL_REG4      0x23
#define CTRL_REG6      0x25
#define STATUS_REG     0x27

//Define register values (LIS3DH)
#define CTRL1_VAL	     0x97 //3-axis; HR
#define CTRL3_VAL      0x10 //INT1 enable
#define CTRL4_VAL      0x08 //HR mode
#define CTRL6_VAL      0x02 //INT1 is active low

uint8_t const ACCEL_DATA = 0x28;

//Should not need to use this b/c of ACCEL_DATA
#define OUT_X_L        0x28
#define OUT_X_H        0x29
#define OUT_Y_L        0x2A
#define OUT_Y_H        0x2B
#define OUT_Z_L        0x2C
#define OUT_Z_H        0x2D

//Define address of DAC
#define MCP4725_ADDR   0xC8


