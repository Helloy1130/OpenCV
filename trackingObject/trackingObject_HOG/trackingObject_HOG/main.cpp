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
	cv::Mat templateMat = cv::imread("C:\\Users\\Y\\Desktop\\template.png", 0);
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\img.png", 0);
	cv::Mat compareMat;
	//需要比较的图片大小一致
	int cellSize = 16;
	int nX = templateMat.cols / cellSize;
	int nY = templateMat.rows / cellSize;
	int bins = 8 * nX*nY;

	while (1)
	{
		float *template_hist = new float[bins];
		memset(template_hist, 0, sizeof(float)*bins);


		calculateHOG(templateMat, nX, nY, template_hist);//计算模板的直方图

		float distanceMin;
		Rect dstRect;

		for (int j = 0; j < srcMat.rows - templateMat.rows; j++)
		{
			for (int i = 0; i < srcMat.cols - templateMat.cols; i++)
			{
				Rect rect(i, j, templateMat.cols, templateMat.rows);
				compareMat = Mat(srcMat, rect);	//按照rect切割

				float *compare_hist = new float[bins];
				memset(compare_hist, 0, sizeof(float)*bins);

				calculateHOG(compareMat, nX, nY, compare_hist);//需要比较的图的直方图

				//判断相似程度
				float distance = 0;
				for (int k = 0; k < bins; k++)
				{
					distance += pow(template_hist[k] - compare_hist[k], 2);
				}
				distance = sqrt(distance);
				if (j == 0) {
					distanceMin = distance;
				}
				else if (distance < distanceMin) {
					distanceMin = distance;
					dstRect = rect;
				}

				delete[] compare_hist;
				//cout << "比较图像与参考图像的相似度：" << distance << endl;
			}
		}

		rectangle(srcMat, dstRect, Scalar(0, 0, 255));

		cv::imshow("templateMat", templateMat);
		cv::imshow("srcMat", srcMat);

		delete[] template_hist;
		
		waitKey(0);
	}

	return 0;

}