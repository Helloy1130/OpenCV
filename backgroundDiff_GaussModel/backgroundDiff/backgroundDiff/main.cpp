#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int sub_threshold = 0;
Mat bgMat;
Mat subMat;
Mat bny_subMat;

void threshold_track(int, void *)//这里就是定义的一个回调函数，里面是canny相关的操作
{

	threshold(subMat, bny_subMat, sub_threshold, 255, CV_THRESH_BINARY);
	imshow("Result", bny_subMat);
}

int bgSub_demo()
{
	VideoCapture capVideo(0); //调用摄像头

	//如果视频打开失败
	if (!capVideo.isOpened()) {
		std::cout << "Unable to open video!" << std::endl;
		return -1;
	}

	int cnt = 0;

	Mat frame;

	while (1)
	{
		capVideo >> frame;
		cvtColor(frame, frame, COLOR_BGR2GRAY);

		if (cnt == 0) {
			//第一帧，获得背景图像
			frame.copyTo(bgMat);
		}
		else {
			//第二帧开始背景差分
			//背景图像和当前图像相减
			absdiff(frame, bgMat, subMat);
			//差分结果二值化
			threshold(subMat, bny_subMat, 20, 255, CV_THRESH_BINARY);

			namedWindow("Result", WINDOW_AUTOSIZE);
			//滑动条创建
			cv::createTrackbar("threshold", "Result", &sub_threshold, 255, threshold_track);
			threshold_track(0, 0);

			imshow("frame", frame);
			waitKey(30);
		}
		cnt++;
	}

	return 0;
}

int main()
{
	bgSub_demo();
	return 0;

}