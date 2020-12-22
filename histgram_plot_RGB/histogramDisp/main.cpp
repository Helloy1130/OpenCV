#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

	float histogram[256] = { 0 };//存储直方图

	cv::Mat srcMat = imread("C:\\Users\\Y\\Desktop\\1.jpg", 0);//参数0的意思是，将读取的图片读取后直接转换为灰度图。
	int height = srcMat.rows;
	int width = srcMat.cols;;
	for (int j = 0; j < height; j++)
	{
		uchar* data = srcMat.ptr<uchar>(j);//指向srcMat第j行的首地址
		for (int i = 0; i < width; i++)
		{
			//得到直方图
			histogram[data[i]] += 1;
		}
	}
	//得到归一化直方图
	for (int i = 0; i < 256; i++) {
		histogram[i] = histogram[i] / (height*width);
	}

	cv::Mat dispMat;
	dispMat.create(480, 512, CV_8UC3);//480行 512列
	cv::Point pt1;//线段两端点
	cv::Point pt2;

	//画直方图
	for (int i = 0; i < 255; i++) {
		pt1.x = i * 2;
		pt1.y = 480 - histogram[i] * 480;
		pt2.x = (i + 1) * 2;
		pt2.y = 480 - histogram[i+1] * 480;
		line(dispMat, pt1, pt2, CV_RGB(255, 0, 0), 1, 8, 0);//dispMat画圆的目标图像 pt1，2线段的起点和终点 线段的颜色 线段的粗细
															//领接关系和偏移，一般设置默认值，8和0
	}
	
	imshow("histogramDisp", dispMat);

	waitKey(0);

	return 0;
}