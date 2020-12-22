#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//Trackbar的回调函数
void threshod_Mat(int th, void* data)
{
	Mat src = *(Mat*)(data);

	Mat dst;
	//二值化
	threshold(src, dst, th, 255, 0);

	imshow("binaryMat", dst);
}

int main()
{
	cv::Mat srcMat = imread("C:\\Users\\Y\\Desktop\\1.jpg", 0);//参数0的意思是，将读取的图片读取后直接转换为灰度图

	int lowTh = 30;
	int maxTh = 255;

	imshow("binaryMat", srcMat);
	createTrackbar("threshold",
					"binaryMat",
					&lowTh,
					maxTh,
					threshod_Mat,
					&srcMat);

	waitKey(0);

	return 0;

}


