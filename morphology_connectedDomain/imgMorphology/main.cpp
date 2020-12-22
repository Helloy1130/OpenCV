#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = imread("C:\\Users\\Y\\Desktop\\coin.png", 0);//读取图片直接转换为灰度图
	cv::Mat binaryMat;//二值图
	cv::Mat erodeMat;//腐蚀图
	cv::Mat dilateMat;//膨胀图
	cv::Mat openMat;//开运算
	cv::Mat closeMat;//闭运算

	threshold(srcMat, binaryMat, 100, 255, THRESH_BINARY);//大津法二值化

	Mat kernel = getStructuringElement(MORPH_RECT, Size(4, 4));//结构元素
	morphologyEx(binaryMat, erodeMat, MORPH_ERODE, kernel);//腐蚀
	morphologyEx(binaryMat, dilateMat, MORPH_DILATE, kernel);//膨胀
	morphologyEx(binaryMat, openMat, MORPH_OPEN, kernel);//开运算
	morphologyEx(binaryMat, closeMat, MORPH_CLOSE, kernel);//闭运算

	imshow("二值图", binaryMat);
	imshow("腐蚀", erodeMat);
	imshow("膨胀", dilateMat);
	imshow("开运算", openMat);
	imshow("闭运算", closeMat);

	waitKey(0);

	return 0;

}


