//Bryan Gardner
//2/18/2018

#include "double.h"

//int foo[2] = {0};
int DSP_filter(int input_to_filter)
{
	//PORTB |= (1 << PB0);
	//	Filter1

	w1[0] = (a1[1] * w1[1]) + (a1[2] * w1[2]) + input_to_filter;
	y1 = (b1[0] * w1[0]) + (b1[1] * w1[1]) + (b1[2] * w1[2]);

	w2[0] = (a2[1] * w2[1]) + (a2[2] * w2[2]) + y1;
	y2 = (b2[0] * w2[0]) + (b2[1] * w2[1]) + (b2[2] * w2[2]);

	w3[0] = (a3[1] * w3[1]) + (a3[2] * w3[2]) + y2;
	y3 = (b3[0] * w3[0]) + (b3[1] * w3[1]) + (b3[2] * w3[2]);


	w4[0] = (a4[1] * w4[1]) + (a4[2] * w4[2]) + y3;
	y4 = (b4[0] * w4[0]) + (b4[1] * w4[1]) + (b4[2] * w4[2]);

	w5[0] = (a5[1] * w5[1]) + (a5[2] * w5[2]) + y4;
	y5 = (b5[0] * w5[0]) + (b5[1] * w5[1]) + (b5[2] * w5[2]);

	w6[0] = (a6[1] * w6[1]) + (a6[2] * w6[2]) + y5;
	y6 = (b6[0] * w6[0]) + (b6[1] * w6[1]) + (b6[2] * w6[2]);


	for (int j = 2; j >= 1; j--)
	{
		temp1 = w1[j - 1];
		w1[j] = temp1;

		temp2 = w2[j - 1];
		w2[j] = temp2;

		temp3 = w3[j - 1];
		w3[j] = temp3;

		temp4 = w4[j - 1];
		w4[j] = temp4;

		temp5 = w5[j - 1];
		w5[j] = temp5;

		temp6 = w6[j - 1];
		w6[j] = temp6;
		//PORTB &= ~(1 << PB0);
	}

	return y6;
	
}
