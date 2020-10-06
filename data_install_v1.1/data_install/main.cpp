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

FILE *fp, *fp2, *fp3, *fp4;

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

char readfilename[] = "RRI.csv";
char writefilename[] = "LFHF.csv";
char writefilename1[] = "LF.csv";
char writefilename2[] = "HF.csv";

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
	int i, j, k, l, m, n;
	const int buf_size = 100;
	char buf[buf_size];
	double *fftbuf[2];
	double *fftbuf2[2];
	double *spbuf[2];
	double interval = 179.0 / (double)samples, tsum = 0;

	for (i = 0; i < 2; i++) {
		fftbuf[i] = (double*)malloc(sizeof(double) * samples);
		fftbuf2[i] = (double*)malloc(sizeof(double) * samples);
		spbuf[i] = (double*)malloc(sizeof(double) * samples);
	}

	line(MainImageWindow, 50, 50, 50, 50 + GraphHeight, 0, 0, 0, 1);
	line(MainImageWindow, 50, 50 + GraphHeight, 50 + GraphWidth, 50 + GraphHeight, 0, 0, 0, 1);

	line(MainImageWindow, 50, 50 + GraphHeight, 50, 50 + 2 * GraphHeight, 0, 0, 0, 1);
	line(MainImageWindow, 50, 50 + 2 * GraphHeight, 50 + GraphWidth, 50 + 2 * GraphHeight, 0, 0, 0, 1);

	//全データ数（行数）の確認
	fp = fopen(readfilename, "r");
	if (fp) {
		while (fgets(buf, buf_size, fp) != NULL) {
			datastep++;
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);

		for (i = 0; i < 2; i++) data[i] = (double*)malloc(sizeof(double) * datastep * 2); //動的メモリの確保

	}
	fp = fopen(readfilename, "r");
	fp2 = fopen(writefilename, "a");
	fp3 = fopen(writefilename1, "a");
	fp4 = fopen(writefilename2, "a");


	double plot[2], ps[2];
	double alfa = 200.0 / 1.5, beta = 30.0;
	double gamma[2] = { 1.0, 1.0 };
	double rate, rate2, ave;
	int b = 1;

	if (fp) {
		j = 0; k = 0; n = 0;
		for (i = 0; i < datastep; i++) {
			fscanf(fp, "%lf,%lf\n", &data[0][n], &data[1][n]);
			//if (i < samples) { fftbuf[0][i] = (data[0][i] * 0.1)* fft.window_func(WF_Rectangular, i, samples); fftbuf[1][i] = 0; }
			if (i > 0) {
				b = 1;
				//gamma[1] = gamma[0];
				//rate = (data[1][n] / ave); 
				//if (rate >= 1.715) {
				//	gamma[0] = 1.0 / rate; // 0.5;//0.01 / fabs(data[1][i] - data[1][i - 1]);
				//	b = (int)((data[1][n] / ave) + 0.285);
				//}
				//else
					gamma[0] = 1.0;

				for (m = 0; m < b; m++) {
					data[1][n] = gamma[0] * data[1][n];//(gamma[0] * (data[1][i] - data[1][i - 1]) + data[1][i - 1]);

					rate2 = (data[1][n] / data[1][n - 1]);
					//if (rate2 >= 1.01)
					//	gamma[0] = 1.01 / rate2; // 0.5;//0.01 / fabs(data[1][i] - data[1][i - 1]);
					//else if (rate2 <= 0.99)
					//	gamma[0] = 0.99 / rate2; // 0.5;//0.01 / fabs(data[1][i] - data[1][i - 1]);
					//else
					//	gamma[0] = 1.0;

					data[1][n] = gamma[0] * data[1][n];//(gamma[0] * (data[1][i] - data[1][i - 1]) + data[1][i - 1]);
					data[0][n] = data[0][n - 1] + data[1][n];
					ave = 0.9 * ave + 0.1 * data[1][n];

					while (tsum < data[0][n]) {
						if (j < samples) {
							fftbuf[0][j] = (((data[1][n] - data[1][n - 1]) / (data[0][n] - data[0][n - 1])) * (tsum - data[0][n - 1]) + data[1][n - 1]);
							fftbuf[1][j] = 0.0;
							fftbuf2[0][j] = fftbuf[0][j] * fft.window_func(WF_Rectangular, j, samples);
							fftbuf2[1][j] = 0.0;

							j++;
						}
						else {
							for (l = 0; l < samples - 1; l++) {
								fftbuf[0][l] = fftbuf[0][l + 1];
								fftbuf[1][l] = 0.0;
								fftbuf2[0][l] = fftbuf[0][l] * fft.window_func(WF_Rectangular, l, samples);
								fftbuf2[1][l] = 0.0;
							}
							fftbuf[0][j - 1] = (((data[1][n] - data[1][n - 1]) / (data[0][n] - data[0][n - 1])) * (tsum - data[0][n - 1]) + data[1][n - 1]);
							fftbuf[1][j - 1] = 0.0;
							fftbuf2[0][j - 1] = fftbuf[0][j - 1] * fft.window_func(WF_Rectangular, j - 1, samples);
							fftbuf2[1][j - 1] = 0.0;
						}

						tsum += interval;
					}

					plot[1] = plot[0];
					plot[0] = alfa * data[1][n];
					line(MainImageWindow, 50 + (k + 1), (50 + GraphHeight) - plot[0], 50 + k, (50 + GraphHeight) - plot[1], 0, 0, 255, 1);

					if (j == samples) {
						fft.fft_cal(fftbuf2[0], fftbuf2[1], spbuf[0], spbuf[1], bitSize);

						double  f, LF = 0, HF = 0;

						for (l = 0; l < samples / 2; l++) {
							if (l > 80) // 
								break;
							else if (l > 0) {
								if (l < 31) {
									LF += sqrt(spbuf[0][l] * spbuf[0][l] + spbuf[1][l] * spbuf[1][l]);
								}
								else {
									HF += sqrt(spbuf[0][l] * spbuf[0][l] + spbuf[1][l] * spbuf[1][l]);
								}
							}

						}
						ps[1] = ps[0];
						ps[0] = (LF / HF) * 100.0;

						fprintf(fp2, "%lf,%lf,%lf\n", data[0][n], data[1][n], (LF / HF));
						fprintf(fp3, "%lf,%lf,%lf\n", data[0][n], data[1][n], LF);
						fprintf(fp4, "%lf,%lf,%lf\n", data[0][n], data[1][n], HF);
						line(MainImageWindow, 50 + (k + 1), (50 + 2 * GraphHeight) - ps[0], 50 + k, (50 + 2 * GraphHeight) - ps[1], 255, 0, 0, 1);
					}
					gamma[0] = 1.0;
					n++;
					data[1][n] = gamma[0] * data[1][n-1];
					k++;
				}
			}
			else {
				ave = data[1][i];
				plot[0] = data[1][i];
				plot[0] = alfa * (double)plot[0];
				tsum = data[0][i];
				n++;
			}


		}
	}

	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
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



	for (i = 0; i < 2; i++) {
		free(data[i]);
		free(fftbuf[i]);
		free(fftbuf2[i]);
		free(spbuf[i]);
	}

}

void main()
{
	initialize();
	cv::imshow("ImageWindow", MainImageWindow);

	cv::waitKey(0);
}




