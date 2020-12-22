#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\rim.png", 1);
	cv::Mat grayMat;
	cv::Mat binaryMat;
	cv::Mat labelMat;//标签
	cv::Mat statsMat;//状态矩阵
	cv::Mat centrMat;//连通域中心
	cv::Mat dstMat;
	srcMat.copyTo(dstMat);

	while (1)
	{
		cvtColor(srcMat, grayMat, CV_RGB2GRAY);
		cv::Mat inversedMat = 255 - grayMat;//反色
		threshold(inversedMat, binaryMat, 150, 255, THRESH_OTSU);//大津法二值化
		//寻找连通域
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(binaryMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//绘制轮廓
		for (int i = 0; i < contours.size(); i++) 
		{
			//获得最小外界四边形
			RotatedRect rbox = minAreaRect(contours[i]);
			//计算宽长比
			cv::Size2f side = rbox.size;
			float ratio = side.width / side.height;
			if (fabs(1 - ratio)<0.05 && side.width>10 && side.height>10)//输出圆
			{
				drawContours(dstMat, contours, i, Scalar(0, 255, 255), CV_FILLED, 8);
			}
		}
		cv::imshow("srcMat", srcMat);
		cv::imshow("grayMat", grayMat);
		cv::imshow("binaryMat", binaryMat);
		cv::imshow("dstMat", dstMat);

		waitKey(30);
	}

	return 0;

}