#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = imread("C:\\Users\\Y\\Desktop\\coin.png", 0);//读取图片直接转换为灰度图
	cv::Mat binaryMat;//二值图
	cv::Mat labelMat;//标签
	cv::Mat statsMat;//状态矩阵
	cv::Mat centrMat;//连通域中心
	cv::Mat resultMat;//结果

	threshold(srcMat, binaryMat, 100, 255, THRESH_BINARY);//大津法二值化

	//获得连通域 返回值为连通域个数
	int numComp = connectedComponentsWithStats(binaryMat, labelMat, statsMat, centrMat, 8, CV_32S);
	binaryMat.copyTo(resultMat);

	//绘制bounding box
	for (int i = 1; i < numComp; i++)
	{
		Rect bndbox;
		//bounding box左上角坐标
		bndbox.x = statsMat.at<int>(i, 0);
		bndbox.y = statsMat.at<int>(i, 1);
		//bouding box的宽和长 
		bndbox.width = statsMat.at<int>(i, 2);
		bndbox.height = statsMat.at<int>(i, 3);
		//绘制
		rectangle(resultMat, bndbox, CV_RGB(255, 255, 255), 1, 8, 0);
		//连通域编号
	}

	imshow("二值图", binaryMat);
	imshow("结果图", resultMat);

	cout << "硬币个数：" << numComp - 1 << endl;

	waitKey(0);

	return 0;

}


