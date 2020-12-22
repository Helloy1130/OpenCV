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

		//旋转-10°，缩放尺度为
		float angle = -10.0, scale = 1;
		//旋转中心为图像中心
		cv::Point2f center(srcMat.cols*0.5, srcMat.rows*0.5);
		//获得变换矩阵
		const cv::Mat affine_matrix = cv::getRotationMatrix2D(center, angle, scale);

		cv::warpAffine(srcMat, dstMat, affine_matrix, srcMat.size());//仿射变换函数

		cv::imshow("src", srcMat);
		cv::imshow("dst", dstMat);

		waitKey(30);
	}

	return 0;

}