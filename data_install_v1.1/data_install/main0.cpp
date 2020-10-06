#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <opencv2\opencv.hpp>
#include <atlbase.h>

#include "FFT.h"

FFT fft;

FILE *fp;

//表示部分
std::vector<BYTE> colorBuffer;

//表示用
#define ImageWidth 1300
#define ImageHeight 600

#define GraphWidth 1200
#define GraphHeight 200

double *data[2];
int datastep = 0;

cv::Mat MainImageWindow(ImageHeight, ImageWidth, CV_8UC3, cv::Scalar(255, 255, 255));
std::vector<UINT16> depthBuffer;

char readfilename[] = "data.csv";


//描画関数　OpenCVベース
void line(cv::Mat & Image, int x1, int y1, int x2, int y2, int R, int G, int B, int width) {
	cv::line(Image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(B, G, R), width, CV_AA);
}

void rect(cv::Mat & Image, int x1, int y1, int x2, int y2, int R, int G, int B, int width) {
	cv::rectangle(Image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(B, G, R), width, CV_AA);
}

void circle(cv::Mat & Image, int cx, int cy, int radius, int R, int G, int B, int width) {
	cv::circle(Image, cv::Point(cx, cy), radius, cv::Scalar(B, G, R), width, CV_AA);
}

void initialize()
{
	const int samples = 1024, bitSize = 10;
	int i, j, k;
	const int buf_size = 100;
	char buf[buf_size];
	double *fftbuf[2];
	double *spbuf[2];
	double interval = 100.0 / (double)samples, tsum = 0;

	for (i = 0; i < 2; i++) {
		fftbuf[i] = (double*)malloc(sizeof(double) * samples);
		spbuf[i] = (double*)malloc(sizeof(double) * samples);
	}

	double plot[2];
	double alfa = 200.0 / 1.5, beta = 30.0;

	line(MainImageWindow, 50, 50, 50, 50 + GraphHeight, 0, 0, 0, 1);
	line(MainImageWindow, 50, 50 + GraphHeight, 50 + GraphWidth, 50 + GraphHeight, 0, 0, 0, 1);

	line(MainImageWindow, 50, 50 + GraphHeight, 50, 50 + 2*GraphHeight, 0, 0, 0, 1);
	line(MainImageWindow, 50, 50 + 2*GraphHeight, 50 + GraphWidth, 50 + 2*GraphHeight, 0, 0, 0, 1);

	//全データ数（行数）の確認
	fp = fopen(readfilename, "r");
	if (fp) {
		while (fgets(buf, buf_size, fp) != NULL) {
			datastep++;
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);

		for (i = 0; i < 2; i++) data[i] = (double*)malloc(sizeof(double) * datastep); //動的メモリの確保
	
	}
	fp = fopen(readfilename, "r");
	if (fp) {
		j = 0; k = 0;
		for (i = 0; i < datastep; i++) {
			fscanf(fp, "%lf,%lf\n", &data[0][i], &data[1][i]);
			//if (i < samples) { fftbuf[0][i] = (data[0][i] * 0.1)* fft.window_func(WF_Rectangular, i, samples); fftbuf[1][i] = 0; }
			if (i > 0) {
				while (tsum < data[0][i] && j < samples) {
					fftbuf[0][j] = (((data[1][i] - data[1][i - 1]) / (data[0][i] - data[0][i - 1])) * (tsum - data[0][i - 1]) + data[1][i - 1]) * fft.window_func(WF_Blackman, j, samples);
					fftbuf[1][j] = 0.0;
					j++;
					tsum += interval;
				}

				plot[1] = plot[0];
				plot[0] = alfa * data[1][i];
				line(MainImageWindow, 50 + (k + 1), (50 + GraphHeight) - plot[0], 50 + k, (50 + GraphHeight) - plot[1], 0, 0, 255, 1);
				k++;
			}
			else {
				plot[0] = data[1][i];
				plot[0] = alfa * (double)plot[0];
				tsum = data[0][i];
			}
		}
	}

	fclose(fp);
	//double tm = 0;
	//fp = fopen("data3.csv", "a");
	//if (fp) {
	//	tm = data[0];
	//	fprintf(fp, "%lf,%lf\n", tm, data[0]);
	//	for (i = 1; i < datastep; i++) {
	//		if (data[i - 1] != data[i]) {
	//			tm += data[i];
	//			fprintf(fp, "%lf,%lf\n", tm, data[i]);
	//		}
	//	}
	//}
	//fclose(fp);

	fft.fft_cal(fftbuf[0], fftbuf[1], spbuf[0], spbuf[1], bitSize);

	double ps[2], f, LF = 0, HF = 0;
	ps[1] = sqrt(spbuf[0][0] * spbuf[0][0] + spbuf[1][0] * spbuf[1][0])*beta;
	for (i = 1; i < samples/2; i++) {
		ps[0] = sqrt(spbuf[0][i] * spbuf[0][i] + spbuf[1][i] * spbuf[1][i])*beta;
		if (i > 2) {
			f = (double)i / (interval * samples);
			line(MainImageWindow, 50 + (i - 1), (50 + 2 * GraphHeight) - ps[1], 50 + i, (50 + 2 * GraphHeight) - ps[0], 255, 0, 0, 1);
			printf("%lf,%lf\n", f, ps[1]/100.0);

		}
		ps[1] = ps[0];
	}

	for (i = 0; i < 2; i++) {
		free(data[i]);
		free(fftbuf[i]);
		free(spbuf[i]);
	}

}

void main()
{
	initialize();
	cv::imshow("ImageWindow", MainImageWindow);

	cv::waitKey(0);
}




