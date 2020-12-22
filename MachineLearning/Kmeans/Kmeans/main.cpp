#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int createMaskByKmeans(cv::Mat src, cv::Mat &mask)
{
	if ((mask.type() != CV_8UC1)|| (src.size() != mask.size())) {
		return 0;
	}

	int width = src.cols;
	int height = src.rows;
	int pixNum = width * height;
	int clusterCount = 2;

	Mat labels;
	Mat centers;

	//制作Kmeans用的数据
	Mat sampleData = src.reshape(3, pixNum);
	Mat km_data;
	sampleData.convertTo(km_data, CV_32F);

	//执行Kmeans
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1);
	kmeans(km_data, clusterCount, labels, criteria, clusterCount, KMEANS_PP_CENTERS, centers);

	//制作mask
	uchar fg[2] = { 0,255 };
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			mask.at<uchar>(row, col) = fg[labels.at<int>(row*width + col)];
		}
	}

	return 0;
}

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\movie.jpg");
	cv::Mat maskMat = Mat::zeros(srcMat.size(), CV_8UC1);

	createMaskByKmeans(srcMat,maskMat);
	cv::imshow("srcMat", srcMat);
	cv::imshow("maskMat", maskMat);

	waitKey(0);

	return 0;

}