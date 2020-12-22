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
		//获得仿射变换矩阵
		cv::Mat affine_matrix = cv::getRotationMatrix2D(center, angle, scale);

		//调整图像
		cv::Rect bbox = cv::RotatedRect(center, srcMat.size(), angle).boundingRect();//构造一个带倾斜角的矩形，返回能够包含旋转矩形框的最小矩形
		affine_matrix.at<double>(0, 2) += bbox.width / 2.0 - center.x;//x方向平移
		affine_matrix.at<double>(1, 2) += bbox.height / 2.0 - center.y;//y方向平移

		cv::warpAffine(srcMat, dstMat, affine_matrix, bbox.size());//仿射变换函数

		cv::imshow("src", srcMat);
		cv::imshow("dst", dstMat);

		waitKey(30);
	}

	return 0;

}