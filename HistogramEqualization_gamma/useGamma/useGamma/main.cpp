#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\gtest.jpg", 1);
	cv::Mat dstMat;
	srcMat.copyTo(dstMat);

	double gamma = 0.35;

	//建立查找表
	uchar Lut[256] = { 0 };
	for (int i = 0; i < 256; i++)
	{
		Lut[i] = (uchar)(pow((float)i / 255, gamma) * 255);
	}

	//遍历像素,从查找表中读取
	for (int j = 0; j < srcMat.rows; j++) {
		for (int i = 0; i < srcMat.cols; i++) {
			dstMat.at<Vec3b>(j, i)[0] = Lut[srcMat.at<Vec3b>(j, i)[0]];
			dstMat.at<Vec3b>(j, i)[1] = Lut[srcMat.at<Vec3b>(j, i)[1]];
			dstMat.at<Vec3b>(j, i)[2] = Lut[srcMat.at<Vec3b>(j, i)[2]];
		}
	}
	while (1)
	{

		cv::imshow("srcMat", srcMat);
		cv::imshow("dstMat", dstMat);

		waitKey(0);
	}

	return 0;

}