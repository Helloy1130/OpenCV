#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

	float histogram[256] = { 0 };//存储直方图

	cv::Mat srcMat = imread("C:\\Users\\Y\\Desktop\\1.jpg", 0);//参数0的意思是，将读取的图片读取后直接转换为灰度图。
	int height = srcMat.rows;
	int width = srcMat.cols;
	for (int j = 0; j < height; j++)
	{
		uchar* data = srcMat.ptr<uchar>(j);//指向srcMat第j行的首地址
		for (int i = 0; i < width; i++)
		{
			//开始处理像素
			histogram[data[i]] += 1;
		}
	}
	//输出直方图
	for (int i = 0; i < 256; i++) {
		std::cout << i << ":" << histogram[i] / (height*width) << std::endl;
	}

	waitKey(0);

	return 0;
}