#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap(0); //调用摄像头，并对摄像头取得的帧进行缩放

	double scale = 0.5;

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

		cap >> frame;
		Size ResImgSize = Size(frame.cols*scale, frame.rows*scale);
		Mat rFrame = Mat(ResImgSize, frame.type());
		resize(frame, rFrame, ResImgSize, INTER_LINEAR);//利用resize

		cvtColor(rFrame, hsvMat, COLOR_BGR2HSV);//将原图转成hsv格式，需要设置最后一位参数为COLOR_BGR2HSV

		rFrame.copyTo(detectMat);//复制一个原图给detectMat，作为显示用

		//Scalar是一个由长度为4的数组
		//	作为元素构成的结构体， Scalar
		//	最多可以存储四个值，没有提
		//	供的值默认是0。
		cv::inRange(hsvMat, Scalar(i_minH, i_minS, i_minV), Scalar(i_maxH, i_maxS, i_maxV), detectMat);

		imshow("while :in the range", detectMat);
		imshow("frame", rFrame);

		waitKey(30);
	}

	return 0;

}


