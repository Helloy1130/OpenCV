#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//计算得到最小二乘法的系数矩阵
//in_point:数据点矩阵
//n:多项式系数
//return:系数矩阵Mat
Mat polyfit(vector<Point>& in_point, int n)
{
	int size = in_point.size();
	//所求未知数个数
	int x_num = n + 1;
	//构造矩阵U和Y
	Mat mat_u(size, x_num, CV_64F);
	Mat mat_y(size, 1, CV_64F);

	for (int i = 0; i < mat_u.rows; ++i) {
		for (int j = 0; j < mat_u.cols; ++j)
		{
			mat_u.at<double>(i, j) = pow(in_point[i].x, j);
		}
	}

	for (int i = 0; i < mat_y.rows; ++i)
	{
		mat_y.at<double>(i, 0) = in_point[i].y;
	}

	//矩阵运算，获得系数矩阵K
	Mat mat_k(x_num, 1, CV_64F);
	mat_k = (mat_u.t()*mat_u).inv()*mat_u.t()*mat_y;
	cout << "mat_k:" << mat_k << endl;
	return mat_k;
}

//绘制火焰的外框
//inputImg:在输入图像中画出火焰的外框
//fireImg:识别出火焰的二值图
int drawFire(Mat &inputImg, Mat fireImg)
{
	vector<vector<Point>> contours_set;//保存轮廓提取后的点集及拓扑关系  

	findContours(fireImg, contours_set, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//检测外轮廓
	Point point1;
	Point point2;
	float xmin = inputImg.cols, ymin = inputImg.rows, xmax = 0, ymax = 0;
	Rect finalRect;
	vector<vector<Point> >::iterator iter = contours_set.begin();
	for (; iter != contours_set.end();)
	{
		Rect rect = boundingRect(*iter);

		if (rect.area() > 0)
		{
			point1.x = rect.x;
			point1.y = rect.y;
			point2.x = point1.x + rect.width;
			point2.y = point1.y + rect.height;

			if (point1.x < xmin)
				xmin = point1.x;
			if (point1.y < ymin)
				ymin = point1.y;
			if (point2.x > xmax)
				xmax = point2.x;
			if (point2.y > ymax)
				ymax = point2.y;

			++iter;
		}
		else
		{
			iter = contours_set.erase(iter);
		}
	}

	if (xmin == inputImg.cols && ymin == inputImg.rows && xmax == 0 && ymax == 0)//没有检测到火焰轮廓的情况下不画
	{
		xmin = ymin = xmax = ymax = 0;
	}

	finalRect = Rect(xmin, ymin, xmax - xmin, ymax - ymin);

	rectangle(inputImg, finalRect, Scalar(0, 255, 0));

	return 0;
}

//根据颜色和亮度判断火焰
Mat checkColor(Mat &inImg)
{
	Mat fireImg;
	fireImg.create(inImg.size(), CV_8UC1);

	int redThre = 135; // 115~135
	int saturationTh = 55; //55~65
	Mat multiRGB[3];
	split(inImg, multiRGB); //将图片的R,G,B三通道拆分

	for (int i = 0; i < inImg.rows; i++)
	{
		for (int j = 0; j < inImg.cols; j++)
		{
			float B, G, R;
			B = multiRGB[0].at<uchar>(i, j); //每个像素的R,G,B值
			G = multiRGB[1].at<uchar>(i, j);
			R = multiRGB[2].at<uchar>(i, j);

			int maxValue = max(max(B, G), R);
			int minValue = min(min(B, G), R);

			double S = (1 - 3.0*minValue / (R + G + B));

			//R > RT  R>=G>=B  S>=((255-R)*ST/RT)
			if (R > redThre && R >= G && G >= B && S > 0.20 && S > ((255 - R) * saturationTh / redThre))
			{
				fireImg.at<uchar>(i, j) = 255;
			}
			else
			{
				fireImg.at<uchar>(i, j) = 0;
			}
		}
	}

	dilate(fireImg, fireImg, Mat(10, 10, CV_8UC1));//进行膨胀处理

	drawFire(inImg, fireImg);

	imshow("fire", fireImg);

	return fireImg;
}


int movieProcess()
{
	VideoCapture capVideo("..\\movie\\test.mp4");
	//如果视频打开失败
	if (!capVideo.isOpened()) {
		std::cout << "Unable to open video!" << std::endl;
		return -1;
	}

	int cnt = 0;
	Mat frame;
	Mat frame_gray;
	Mat bgMat;
	Mat subMat;
	Mat bny_subMat;
	int sub_threshold = 100;

	//输入拟合点  
	int x0 = 181; int y0 = 36;
	int xi = x0; int yi = y0;
	std::vector<cv::Point> points;
	points.push_back(cv::Point(x0, y0));//起点手动输入
	std::vector<cv::Point> points_fitted;

	while (1)
	{
		capVideo >> frame;
		if (frame.empty()) break;//若视频结束，则退出
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		
		//先进行背景差分，得到连通域
		if (cnt == 0) {
			//第一帧，获得背景图像
			frame_gray.copyTo(bgMat);
		}
		else {
			//第二帧开始背景差分
			//背景图像和当前图像相减
			absdiff(frame_gray, bgMat, subMat);
			//差分结果二值化
			threshold(subMat, bny_subMat, sub_threshold, 255, CV_THRESH_BINARY);
			imshow("背景差分", bny_subMat);

			Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));//结构元素
			morphologyEx(bny_subMat, bny_subMat, MORPH_DILATE, kernel);//膨胀运算

			//寻找拟合点
			for (int i = yi + 1; i < bny_subMat.rows; i = i + 5) {
				for (int j = xi + 1; j < bny_subMat.cols; j = j + 5) {
					if (bny_subMat.at<uchar>(i, j) == 255) {//如果该点为亮点 第i行，第j列
						double distance = sqrt(pow(j - xi, 2) + pow(i - yi, 2));//计算此亮点与前一帧确定的点的距离
						if (distance < 50) {//当距离小于一定的值,则将此点认为是拟合点  
							points.push_back(Point(j, i));
							xi = j; yi = i;
							i = bny_subMat.rows; j = bny_subMat.cols;
						}
					}
				}
			}
			//取一定数量的拟合点，使用最小二乘法拟合曲线
			if (points.size() == 30)
			{
				Mat A = polyfit(points, 2);
				points_fitted.clear();//清空上次拟合的曲线
				for (int x = x0; x < bny_subMat.cols; x++)
				{
					double y = A.at<double>(0, 0) + A.at<double>(1, 0) * x + A.at<double>(2, 0)*pow(x, 2);
					points_fitted.push_back(Point(x, y));
				}
				xi = x0; yi = y0;//重复拟合，实时校正曲线
				points.clear();
			}

			polylines(frame, points_fitted, false, Scalar(255, 255, 0), 2, 8, 0);//画拟合线
			//火焰识别
			checkColor(frame);
			
			imshow("frame", frame);
			waitKey(30);
		}
		
		cnt++;
	}

	return 0;
}

int main()
{
	//开始计时
	double start = static_cast<double>(cvGetTickCount());

	movieProcess();

	//结束计时
	double time = ((double)cvGetTickCount() - start) / cvGetTickFrequency();
	//显示时间
	cout << "processing time:" << time / 1000 << "ms" << endl;

	//等待键盘响应，按任意键结束程序
	system("pause");
	return 0;
}