#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcMat = cv::imread("C:\\Users\\Y\\Desktop\\topic1.jpg", 1);
	cv::Mat labelMat;//标签
	cv::Mat statsMat;//状态矩阵
	cv::Mat centrMat;//连通域中心
	cv::Mat hsvMat;
	cv::Mat detectMat;
	cv::Mat closeMat;//闭运算
	cv::Mat dstMat;

	double scale = 0.1;
	Size ResImgSize = Size(srcMat.cols*scale, srcMat.rows*scale);
	cv::Mat rsrcMat = Mat(ResImgSize, srcMat.type());
	resize(srcMat, rsrcMat, ResImgSize, INTER_LINEAR);//缩放
	//红色
	//0-180定义色调
	double i_minH = 0;
	double i_maxH = 10;
	//0-255饱和度
	double i_minS = 43;
	double i_maxS = 255;
	//0-255明度
	double i_minV = 46;
	double i_maxV = 255;
	rsrcMat.copyTo(dstMat);

	while (1)
	{
		cvtColor(rsrcMat, hsvMat, COLOR_BGR2HSV);//将原图转成hsv格式，需要设置最后一位参数为COLOR_BGR2HSV
		inRange(hsvMat, Scalar(i_minH, i_minS, i_minV), Scalar(i_maxH, i_maxS, i_maxV), detectMat);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(18, 18));//结构元素
		morphologyEx(detectMat, closeMat, MORPH_CLOSE, kernel);//闭运算
		//寻找连通域
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(closeMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//绘制轮廓
		for (int i = 0; i < contours.size(); i++)
		{
			//获得最小外界四边形
			RotatedRect rbox = minAreaRect(contours[i]);
			cv::Size2f side = rbox.size;
			if (side.width > 10 && side.height > 10)//输出
			{
				//获得最小外界四边形
				RotatedRect rbox = minAreaRect(contours[i]);
				drawContours(dstMat, contours, i, Scalar(0, 255, 255), 1, 8);
				cv::Point2f vtx[4];//四个顶点
				rbox.points(vtx);
				for (int j = 0; j < 4; ++j)
				{
					cv::line(dstMat, vtx[j], vtx[j < 3 ? j + 1 : 0], cv::Scalar(0, 0, 255), 2, CV_AA);
				}
			}
		}
		cv::imshow("rsrcMat", rsrcMat);
		cv::imshow("closeMat", closeMat);
		cv::imshow("dstMat", dstMat);

		waitKey(30);
	}

	return 0;

}