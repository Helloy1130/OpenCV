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

		cv::HoughLines(cannyMat, lineMat, 1, CV_PI / 180, 100);//投票阈值 至少要100个像素贡献
		
		//循环遍历绘制每一条线段
		for (int j = 0; j < lineMat.rows; j++) {
			for (int i = 0; i < lineMat.cols; i++) {
				float rho = lineMat.at<Vec2f>(j, i)[0], theta = lineMat.at<Vec2f>(j, i)[1];
				cv::Point pt1, pt2;
				double a = cos(theta), b = sin(theta);
				double x0 = a * rho, y0 = b * rho;
				pt1.x = cv::saturate_cast<int>(x0 + 1000 * (-b));
				pt1.y = cv::saturate_cast<int>(y0 + 1000 * (a));
				pt2.x = cv::saturate_cast<int>(x0 - 1000 * (-b));
				pt2.y = cv::saturate_cast<int>(y0 - 1000 * (a));
				cv::line(dstMat, pt1, pt2, cv::Scalar(0, 0, 255), 1, CV_AA);
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