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
		Mat dstMat;

		cap >> frame;

		imshow("frame", frame);
		blur(frame, dstMat, Size(3,3));//普通均值滤波
		imshow("meanFilter", dstMat);

		waitKey(30);
	}

	return 0;

}


