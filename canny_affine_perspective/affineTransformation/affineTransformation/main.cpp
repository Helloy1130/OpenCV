#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	while (1)
	{
		cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\lena.jpg", 1);//原图
		cv::Mat dstMat;
		if (srcMat.empty()) return -1;

		//变换前的三点坐标
		const cv::Point2f src_pt[] = {
										cv::Point2f(200, 200),
										cv::Point2f(250, 200),
										cv::Point2f(200, 100) };
		//变换后的三点坐标
		const cv::Point2f dst_pt[] = {
										cv::Point2f(300, 100),
										cv::Point2f(300, 50),
										cv::Point2f(200, 100) };
		//获得仿射矩阵
		const cv::Mat affine_matrix = cv::getAffineTransform(src_pt, dst_pt);

		cv::warpAffine(srcMat, dstMat, affine_matrix, srcMat.size());//仿射变换函数

		cv::imshow("src", srcMat);
		cv::imshow("dst", dstMat);

		waitKey(30);
	}

	return 0;

}