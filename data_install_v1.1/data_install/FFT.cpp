#include "FFT.h"


#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

FFT::FFT()
{
}


FFT::~FFT()
{
}

// ビットを左右反転した配列を返す
int* BitScrollArray(int arraySize)
{
	int i, j;
	int* reBitArray = (int*)malloc(arraySize * sizeof(int));
	int arraySizeHarf = arraySize >> 1;

	reBitArray[0] = 0;
	for (i = 1; i < arraySize; i <<= 1)
	{
		for (j = 0; j < i; j++)
			reBitArray[j + i] = reBitArray[j] + arraySizeHarf;
		arraySizeHarf >>= 1;
	}
	return reBitArray;
}

//---------------------------------------------------------------------
double FFT::window_func(int n, int id, int size) 
{
	double winValue;

	if (n == WF_Hamming)
	{
		winValue = 0.54 - 0.46 * cos(2 * M_PI * id / (size - 1));
	}
	else if (n == WF_Hanning)
	{
		winValue = 0.5 - 0.5 * cos(2 * M_PI * id / (size - 1));
	}
	else if (n == WF_Blackman)
	{
		winValue = 0.42 - 0.5 * cos(2 * M_PI * id / (size - 1))
			+ 0.08 * cos(4 * M_PI * id / (size - 1));
	}
	else if (n == WF_Rectangular)
	{
		winValue = 1.0;
	}
	else
	{
		winValue = 1.0;
	}

	return winValue;
}

//---------------------------------------------------------------------
void FFT::fft_cal(double inputRe[], double inputIm[], double outputRe[], double outputIm[], int bitSize)
{
	int i, j, stage, type;
	int dataSize = 1 << bitSize;
	int butterflyDistance;
	int numType;
	int butterflySize;
	int jp;
	int* reverseBitArray = BitScrollArray(dataSize);
	float wRe, wIm, uRe, uIm, tempRe, tempIm, tempWRe, tempWIm;

	// バタフライ演算のための置き換え
	for (i = 0; i < dataSize; i++)
	{
		outputRe[i] = inputRe[reverseBitArray[i]];
		outputIm[i] = inputIm[reverseBitArray[i]];
	}

	// バタフライ演算
	for (stage = 1; stage <= bitSize; stage++)
	{
		butterflyDistance = 1 << stage;
		numType = butterflyDistance >> 1;
		butterflySize = butterflyDistance >> 1;

		wRe = 1.0;
		wIm = 0.0;
		uRe = cos(M_PI / butterflySize);
		uIm = -sin(M_PI / butterflySize);

		for (type = 0; type < numType; type++)
		{
			for (j = type; j < dataSize; j += butterflyDistance)
			{
				jp = j + butterflySize;
				tempRe = outputRe[jp] * wRe - outputIm[jp] * wIm;
				tempIm = outputRe[jp] * wIm + outputIm[jp] * wRe;
				outputRe[jp] = outputRe[j] - tempRe;
				outputIm[jp] = outputIm[j] - tempIm;
				outputRe[j] += tempRe;
				outputIm[j] += tempIm;
			}
			tempWRe = wRe * uRe - wIm * uIm;
			tempWIm = wRe * uIm + wIm * uRe;
			wRe = tempWRe;
			wIm = tempWIm;
		}
	}
}

//---------------------------------------------------------------------