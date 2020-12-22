#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
	VideoCapture cap;

	cap.open(0);//cap(0)表示打开本机的第一个摄像头， ()中如果是本地视频地址如cap(“D:\\1.avi”)则可以打开本地视频。
	if (!cap.isOpened())//isOpened()检查视频是否开启，正常开启返回1
	{
		std::cout << "不能打开视频文件" << std::endl;
		return -1;
	}

	double fps = cap.get(CAP_PROP_FPS);//通过get()及不同参数可以获得视频的不同参数，如本例获得视频的fps
	std::cout << "fps" << fps << std::endl;
	while (1)
	{
		cv::Mat frame; //声明Mat类型图片，名称为frame，并通过read()获得视频的当前帧
		bool rSucess = cap.read(frame);
		if (!rSucess)
		{
			std::cout << "不能从视频文件中读取帧" << std::endl;
			break;
		}
		else
		{
			cv::imshow("frame", frame); //通过imshow显示当前帧
		}
		waitKey(30);//waitKey延时30ms，如缺少，无法显示
	}

	return 0;
}