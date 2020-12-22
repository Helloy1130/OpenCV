#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\1.png", 1);
	cv::Mat grayMat;
	cv::Mat cannyMat;
	cv::Mat lineMat;
	cv::Mat dstMat;
	srcMat.copyTo(dstMat);

	while (1)
	{
		cvtColor(srcMat, grayMat, CV_RGB2GRAY);
		cv::Canny(grayMat, cannyMat, 60, 180, 3);//Canny算子，输入为源图像

		cv::HoughLinesP(cannyMat, lineMat, 1, CV_PI / 180, 40, 50, 10);//输出线段

		//循环遍历绘制每一条线段
		for (int j = 0; j < lineMat.rows; j++) {
			for (int i = 0; i < lineMat.cols; i++) {
				Vec4i m = lineMat.at<Vec4i>(j, i);
				line(dstMat, Point(m[0], m[1]), Point(m[2], m[3]), Scalar(0, 0, 255), 1, CV_AA);
			}
		}

		cv::imshow("srcMat", srcMat);
		cv::imshow("grayMat", grayMat);
		cv::imshow("Canny", cannyMat);
		cv::imshow("dstMat", dstMat);

		waitKey(30);
	}

	return 0;

}