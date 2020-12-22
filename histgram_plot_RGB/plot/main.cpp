#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

	cv::Mat dispMat;
	dispMat.create(240, 240, CV_8UC3);////创建240行240列，类型是8位unsigned char 3通道
	cv::Point pt;//Opencv中定义一个点的语句，然后对x, y的坐标值赋值
	cv::Point pt1;//线段两端点
	cv::Point pt2;
	//画圆
	pt.x = 50;
	pt.y = 50;
	circle(dispMat, pt, 10, CV_RGB(255, 0, 0), 1, 8, 0);//dispMat画圆的目标图像 pt圆心的点 圆的半径 圆的颜色
														//圆的线条粗细，取-1为绘制实心圆 领接关系和偏移，一般设置默认值，8和0
	//画线段
	pt1.x = 100;
	pt1.y = 100;
	pt2.x = 150;
	pt2.y = 150;
	line(dispMat, pt1, pt2, CV_RGB(255, 0, 0), 1, 8, 0);//dispMat画圆的目标图像 pt1，2线段的起点和终点 线段的颜色 线段的粗细
														//领接关系和偏移，一般设置默认值，8和0
	//画矩形
	cv::Rect rect;
	rect.x = 200;
	rect.y = 200;
	rect.width = 10;
	rect.height = 10;
	rectangle(dispMat, rect, CV_RGB(255, 0, 0), 1, 8, 0);//dispMat画矩形的目标图像 被绘制的矩形 矩形的颜色 
															//矩形的粗细， -1则为实心矩形 领接关系和偏移，一般设置默认值8和0

	imshow("plot", dispMat);

	waitKey(0);

	return 0;
}