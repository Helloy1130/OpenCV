#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//函数名：calculateHOG
//计算图像的梯度强度和方向,将图像分割成cell，按照cell分别计算直方图
//src:输入图像
//nX：X方向cell的个数
//nY：Y方向cell的个数
//hist:输出得到直方图数组
int calculateHOG(Mat src, int nX, int nY, float *hist)
{
	//计算所有像素的梯度及角度
	Mat gx, gy;
	Mat mag, angle;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	//x方向梯度，y方向梯度，梯度，角度，决定输出弧度or角度
	//默认是弧度radians，true选择角度degrees
	cartToPolar(gx, gy, mag, angle, true);
	//遍历每一个cell,按照cell分别计算直方图
	for (int i = 0; i < nY; i++) {
		for (int j = 0; j < nX; j++) {
			for (int m = 0; m < 16; m++) {
				for (int n = 0; n < 16; n++) {
					//将角度方向量化
					int k = (int)(angle.at<float>(i * 16 + m, j * 16 + n) / 45);
					hist[(i*nX + j) * 8 + k] += mag.at<float>(i * 16 + m, j * 16 + n);//累加梯度值
				}
			}
		}
	}

	return 0;
}

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\hogTemplate.jpg", 0);
	cv::Mat compareMat1 = cv::imread("C:\\Users\\Y\\Desktop\\img1.jpg", 0);
	cv::Mat compareMat2 = cv::imread("C:\\Users\\Y\\Desktop\\img2.jpg", 0);
	//需要比较的图片大小一致
	int cellSize = 16;
	int nX = srcMat.cols / cellSize;
	int nY = srcMat.rows / cellSize;
	int bins = 8 * nX*nY;
	
	while (1)
	{
		float *src_hist = new float[bins];
		memset(src_hist, 0, sizeof(float)*bins);
		float *compare_hist1 = new float[bins];
		memset(compare_hist1, 0, sizeof(float)*bins);
		float *compare_hist2 = new float[bins];
		memset(compare_hist2, 0, sizeof(float)*bins);

		calculateHOG(srcMat, nX, nY, src_hist);//计算原图的直方图
		calculateHOG(compareMat1, nX, nY, compare_hist1);//比较图1的直方图
		calculateHOG(compareMat2, nX, nY, compare_hist2);//比较图2的直方图

		//判断相似程度
		float distance1 = 0;
		float distance2 = 0;
		for (int i = 0; i < bins; i++)
		{
			distance1 += pow(src_hist[i] - compare_hist1[i], 2);
			distance2 += pow(src_hist[i] - compare_hist2[i], 2);
		}
		distance1 = sqrt(distance1);
		distance2 = sqrt(distance2);
		cout << "图像1与参考图像的相似度：" << distance1 << endl;
		cout << "图像2与参考图像的相似度：" << distance2 << endl;
		if (distance1 < distance2) {
			cout << "图像1与参考图像更相似" << endl;
		}
		else {
			cout << "图像2与参考图像更相似" << endl;
		}

		cv::imshow("srcMat", srcMat);
		cv::imshow("compareMat1", compareMat1);
		cv::imshow("compareMat2", compareMat2);
		delete[] src_hist;
		delete[] compare_hist1;
		delete[] compare_hist2;
		waitKey(0);
	}

	return 0;

}