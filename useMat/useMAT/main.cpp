#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

	std::cout << "hello opencv!" << std::endl;
	//Mat创建
	cv::Mat M1(2, 2, CV_8UC3, Scalar(0, 0, 255));//2行2列，类型是8位unsigned char 3通道(数据)
	std::cout << "M1 =" << std::endl;
	std::cout << M1 << std::endl;

	cv::Mat M2(2, 2, CV_8UC3, 1);//1 只赋给第一个通道
	std::cout << "M2 =" << std::endl;
	std::cout << M2 << std::endl;
	
	int height = M2.rows;
	int width = M2.cols;
	std::cout << "size :" << height << "," << width << std::endl;
	//Mat复制
	cv::Mat M3;
	M3.create(3, 4, CV_8UC3);

	cv::Mat M4;
	cv::Mat M5;
	M4=M3;//浅复制，指向原数据
	M3.copyTo(M5);//深复制，开辟了新数据

	randu(M3,Scalar::all(0), Scalar::all(255));//产生随机值填充矩阵 范围是0-255

	std::cout << "M3 =" << std::endl;
	std::cout << M3 << std::endl;
	std::cout << "M4 =" << std::endl;
	std::cout << M4 << std::endl;
	std::cout << "M5 =" << std::endl;
	std::cout << M5 << std::endl;

	//遍历Mat
	cv::Mat M6;
	cv::Mat M7;
	M6.create(3, 4, CV_8UC1);
	M7.create(3, 4, CV_8UC3);

	for (int j = 0; j < M6.rows; j++)
	{
		uchar* data = M6.ptr<uchar>(j);//指向M6第j行的首地址
		for (int i = 0; i < M6.cols; i++)
		{
			data[i] = j;
		}
	}
	for (int j = 0; j < M7.rows; j++)
	{
		uchar* data = M7.ptr<uchar>(j);//指向M7第j行的首地址
		for (int i = 0; i < M7.cols; i++)
		{
			M7.at<Vec3b>(j, i)[0] = j;//对三个通道进行幅值
			M7.at<Vec3b>(j, i)[1] = j + 1;
			M7.at<Vec3b>(j, i)[2] = j + 2;
		}
	}
	std::cout << "M6 =" << std::endl;
	std::cout << M6 << std::endl;
	std::cout << "M7 =" << std::endl;
	std::cout << M7 << std::endl;

	system("pause");
	return 0;
}