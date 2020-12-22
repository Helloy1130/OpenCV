#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap(0); //调用摄像头

	while (1)
	{
		Mat frame;
		Mat grayMat;
		Mat dstMat;
		Mat dstMatX;
		Mat dstMatY;

		cap >> frame;

		imshow("frame", frame);
		cvtColor(frame, grayMat, COLOR_BGR2GRAY);//转为灰度图
		imshow("Gray", grayMat);
		Sobel(grayMat, dstMat, CV_16SC1, 1, 0, 3);//Sobel边缘提取 x方向
		convertScaleAbs(dstMat, dstMatX);//计算绝对值，并将结果转换成8位
		imshow("SobelX", dstMatX);

		Sobel(grayMat, dstMat, CV_16SC1, 0, 1, 3);//Sobel边缘提取 y方向
		convertScaleAbs(dstMat, dstMatY);//计算绝对值，并将结果转换成8位
		imshow("SobelY", dstMatY);

		waitKey(30);
	}

	return 0;

}


