#pragma once
#define WF_Rectangular 0
#define WF_Hamming 1
#define WF_Hanning 2
#define WF_Blackman 3

class FFT
{
public:
	FFT();
	~FFT();

	void fft_cal(double inputRe[], double inputIm[], double outputRe[], double outputIm[], int bitSize);
	double window_func(int n, int dataID, int dataSize);
};