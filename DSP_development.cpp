//Bryan Gardner
//2/18/2018

#include <iostream>
#include <cstdlib>
#include <fstream>
using namespace std;


double sample_input_array[6000] = { 0.0 };
double input[134401] = { 0.0 };

double a1[3] = { 1, 0.8549 , -0.9462 }; // This array will contain the a coefficients for the first SOS (second order system)
double a2[3] = { 1 ,-1.0549 ,-0.9495 }; // This array will contain the a coefficients for the second SOS (second order system)
double a3[3] = { 1 ,-0.8131 , -0.8516 }; // This array will contain the a coefficients for the third SOS (second order system)
double a4[3] = { 1 ,0.5987 ,-0.8444 }; // This array will contain the a coefficients for the fourth SOS (second order system)
double a5[3] = { 1 ,-0.3828 ,-0.7741 }; // This array will contain the a coefficients for the fifth SOS (second order system)
double a6[3] = { 1 ,0.1498 ,-0.7700 }; // This array will contain the a coefficients for the sixth SOS (second order system)


double b1[3] = { 0.4698, 0,-0.4698 }; // This array will contain the b coefficients for the first SOS (second order system)
double b2[3] = { 0.4698,0 ,-0.4698 }; // This array will contain the b coefficients for the second SOS (second order system)
double b3[3] = { 0.3547,0 ,-0.3547 }; // This array will contain the b coefficients for the third SOS (second order system)
double b4[3] = { 0.3547,0 ,-0.3547 }; // This array will contain the b coefficients for the fourth SOS (second order system)
double b5[3] = { 0.1721,0 ,-0.1721 }; // This array will contain the b coefficients for the fifth SOS (second order system)
double b6[3] = { 0.1721,0 ,-0.1721 }; // This array will contain the b coefficients for the sixth SOS (second order system)

double temp = { 0 };

double w1[3] = { 0 };
double w2[3] = { 0 };
double w3[3] = { 0 };
double w4[3] = { 0 };
double w5[3] = { 0 };
double w6[3] = { 0 };

double y1, y2, y3, y4, y5, y6, temp1, temp2, temp3, temp4, temp5, temp6 = { 0 };

int DSP_filter(int input_to_filter)
{


	//	Filter1

	w1[0] = (a1[1] * w1[1]) + (a1[2] * w1[2]) + input_to_filter; //This is the first SOS. Output from this section is passed to the next SOS section.
	y1 = (b1[0] * w1[0]) + (b1[1] * w1[1]) + (b1[2] * w1[2]);

	w2[0] = (a2[1] * w2[1]) + (a2[2] * w2[2]) + y1;				//This is the second SOS. Output from this section is passed to the next SOS section.
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
	}

	return y6;

}









int main()
{

	double sum_Xn = 0;
	double output_of_DSP = 0;
	int i = 0;

	double x = 0;

	double send_to_speaker = 0;

	ifstream inputfile;
	ofstream log;

	inputfile.open("./scaled1000.txt"); //reading in as input
	if (!inputfile) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}

	while (inputfile >> x) {
		i++;
		input[i] = x; //input array gets loaded with values in this loop.
		cout << x;
		cout << "\n";
	}

	inputfile.close();

	log.open("log.txt"); //storing output

	for (int u = 0; u < (4096); u++)
	{
		send_to_speaker = DSP_filter(input[u]); //input values are fed through the filter in this loop.
		log << send_to_speaker;
		log << "\n";
	}

	log.close();
	return 0;
}
