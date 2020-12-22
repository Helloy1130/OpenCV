#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\etest.jpg", 1);
	cv::Mat dstMat;

	std::vector<cv::Mat> rgbChannels(3);
	split(srcMat, rgbChannels);

	while (1)
	{
		equalizeHist(rgbChannels[0], rgbChannels[0]);//单通道直方图均衡
		equalizeHist(rgbChannels[1], rgbChannels[1]);
		equalizeHist(rgbChannels[2], rgbChannels[2]);
		cv::merge(rgbChannels, dstMat);//通道合成

		cv::imshow("srcMat", srcMat);
		cv::imshow("dstMat", dstMat);

		waitKey(0);
	}

	return 0;

}