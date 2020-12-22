#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

	cv::Mat src_color = imread("C:\\Users\\Y\\Desktop\\图片1.png");

	std::vector<cv::Mat> channels;	//声明vector，作为分离后3个通道图像的保存容器
	cv::split(src_color, channels);	//调用split函数，分离src_color的rgb三通道
	cv::Mat B = channels.at(0);		//定义3个Mat容器获得3个通道的分离结果，
	cv::Mat G = channels.at(1);		//注意opencv中三个通道的排列顺序是B, G, R
	cv::Mat R = channels.at(2);

	imshow("red", R);
	imshow("green", G);
	imshow("blue", B); //显示3个通道的图像，以及原图
	imshow("original Mat", src_color);

	waitKey(0);

	return 0;
}

