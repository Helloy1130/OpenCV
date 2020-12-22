#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap(0); //调用摄像头

	//0-180
	//定义肤色
	double i_minH = 0;
	double i_maxH = 20;
	//0-255饱和度
	double i_minS = 43;
	double i_maxS = 255;
	//0-255明度
	double i_minV = 55;
	double i_maxV = 255;

	while (1)
	{
		Mat frame;
		Mat hsvMat;
		Mat detectMat;
		Mat medMat;
		Mat dstMat;

		cap >> frame;

		cvtColor(frame, hsvMat, COLOR_BGR2HSV);//将原图转成hsv格式，需要设置最后一位参数为COLOR_BGR2HSV

		frame.copyTo(detectMat);//复制一个原图给detectMat，作为显示用
		cv::inRange(hsvMat, Scalar(i_minH, i_minS, i_minV), Scalar(i_maxH, i_maxS, i_maxV), detectMat);

		GaussianBlur(frame, medMat, Size(5, 5), 3, 3);//高斯滤波

		frame.copyTo(dstMat);
		medMat.copyTo(dstMat, detectMat);

		imshow("frame", frame);
		imshow("while :in the range", detectMat);
		imshow("GaussianFilter", dstMat);

		waitKey(30);
	}

	return 0;

}


