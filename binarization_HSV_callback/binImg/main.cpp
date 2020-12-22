#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = imread("C:\\Users\\Y\\Desktop\\1.jpg", 0);//参数0的意思是，将读取的图片读取后直接转换为灰度图
	cv::Mat result1;//结果图 大津法
	cv::Mat result2;//结果图 区域自适应二值化

	threshold(srcMat, result1, 100, 255, THRESH_BINARY);//大津法为THRESH_OTSU

	adaptiveThreshold(srcMat, result2, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 15, 10);

	imshow("大津法", result1);
	imshow("区域自适应二值化", result2);

	waitKey(0);

	return 0;

}


