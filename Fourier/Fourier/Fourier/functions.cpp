#include "stdafx.h"
#include "functions.h"


std::vector<Point>  mousePoints;
Point points;

//�����Ӧ����
void on_mouse(int EVENT, int x, int y, int flags, void* userdata)
{

	Mat hh;
	hh = *(Mat*)userdata;
	Point p(x, y);
	switch (EVENT)
	{
	case EVENT_LBUTTONDOWN:
	{
		points.x = x;
		points.y = y;
		mousePoints.push_back(points);
		circle(hh, points, 4, cvScalar(255, 255, 255), -1);
		imshow("mouseCallback", hh);
	}
	break;
	}

}

int selectPolygon(cv::Mat srcMat, cv::Mat &dstMat)
{

	vector<vector<Point>> contours;
	cv::Mat selectMat;

	cv::Mat m = cv::Mat::zeros(srcMat.size(), CV_32F);

	m = 1;

	if (!srcMat.empty()) {
		srcMat.copyTo(selectMat);
		srcMat.copyTo(dstMat);
	}
	else {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	namedWindow("mouseCallback");
	imshow("mouseCallback", selectMat);
	setMouseCallback("mouseCallback", on_mouse, &selectMat);
	waitKey(0);
	destroyAllWindows();
	//����roi
	contours.push_back(mousePoints);
	if (contours[0].size() < 3) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	drawContours(m, contours, 0, Scalar(0), -1);

	m.copyTo(dstMat);

	return 0;
}


int mouseROI()
{
	cv::Mat srcMat = imread("../testImages\\rose.jpg");
	cv::Mat dstMat;

	selectPolygon(srcMat, dstMat);

	imshow("srcMat", srcMat);
	imshow("select Area", dstMat);
	waitKey(0);

	return 0;
}

int dftDemo(cv::Mat &mag) {

	cv::Mat srcMat = imread("../testImages\\img2.jpg", 0);

	if (srcMat.empty()) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	Mat padMat;
	//��ͼ��ĳߴ���2��3��5��������ʱ����ɢ����Ҷ�任�ļ����ٶ���졣	
	//�������ͼ�����ѱ任�ߴ�
	int m = getOptimalDFTSize(srcMat.rows);
	int n = getOptimalDFTSize(srcMat.cols);
	//���³ߴ��ͼƬ���б�Ե��Ե���
	copyMakeBorder(srcMat, padMat, 0, m - srcMat.rows, 0, n - srcMat.cols, BORDER_CONSTANT, Scalar::all(0));

	//����һ������,�洢Ƶ��ת����float���͵Ķ����ٴ洢һ������һ����С�ռ�Ķ������洢��������
	Mat planes[] = { Mat_<float>(padMat), Mat::zeros(padMat.size(), CV_32F) };
	Mat planes_true = Mat_<float>(padMat);
	Mat phMat = Mat_<float>(padMat);
	Mat complexMat;

	//��2����ͨ����ͼ��ϳ�һ����ͨ��ͼ��
	merge(planes, 2, complexMat);
	//���и���Ҷ�任,���������ԭMat��,����Ҷ�任���Ϊ����.ͨ��1�����ʵ��,ͨ����������鲿
	dft(complexMat, complexMat);
	//��˫ͨ����ͼ�����������ͨ����ͼ 
	//ʵ����planes[0] = Re(DFT(I),
	//�鲿��planes[1]=  Im(DFT(I))) 
	split(complexMat, planes);
	//����Ⱥ���λ
	magnitude(planes[0], planes[1], planes_true);
	phase(planes[0], planes[1], phMat);

	//���²����Ϊ����ʾ����
	Mat magMat = planes_true;
	// log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	magMat += Scalar::all(1);
	log(magMat, magMat);

	//ȷ���Գ�
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;
	//��ͼ������
	/*
	0 | 1         3 | 2
	-------  ===> -------
	2 | 3         1 | 0
	*/
	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(cx, 0, cx, cy));
	Mat q2(magMat, Rect(0, cy, cx, cy));
	Mat q3(magMat, Rect(cx, cy, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	//Ϊ��imshow������ʾ����һ����0��1֮��
	normalize(magMat, magMat, 0, 1, NORM_MINMAX);
	magMat = magMat * 255;

	//imshow("Input Image", srcMat);    // Show the result
	//imshow("spectrum magnitude", magMat);
	//waitKey(0);
	magMat.copyTo(mag);

	return 0;
	
}

int ifftDemo()
{
	cv::Mat dst;
	cv::Mat src = imread("../testImages\\img2.jpg", 0);

	int m = getOptimalDFTSize(src.rows); //2,3,5�ı����и���Ч�ʵĸ���Ҷ�任
	int n = getOptimalDFTSize(src.cols);
	Mat padded;
	//�ѻҶ�ͼ��������Ͻ�,���ұߺ��±���չͼ��,��չ�������Ϊ0;
	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
	//planes[0]Ϊdft�任��ʵ����planes[1]Ϊ�鲿��phΪ��λ�� plane_true=magΪ��ֵ
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat planes_true = Mat_<float>(padded);
	Mat ph = Mat_<float>(padded);
	Mat complexImg;
	//��ͨ��complexImg����ʵ�������鲿
	merge(planes, 2, complexImg);
	//���ϱߺϳɵ�mat���и���Ҷ�任,***֧��ԭ�ز���***,����Ҷ�任���Ϊ����.ͨ��1�����ʵ��,ͨ����������鲿
	dft(complexImg, complexImg);
	//�ѱ任��Ľ���ָ����mat,һ��ʵ��,һ���鲿,�����������
	split(complexImg, planes);

	//---------------�˲���Ŀ��Ϊ���õ���ʾ��ֵ---�����ָ�ԭͼʱ�����ٴ���һ��-------------------------
	magnitude(planes[0], planes[1], planes_true);//������mag
	phase(planes[0], planes[1], ph);//��λ��ph
	Mat A = planes[0];
	Mat B = planes[1];
	Mat mag = planes_true;

	mag += Scalar::all(1);//�Է�ֵ��1
	log(mag, mag);//������ķ�ֵһ��ܴ󣬴ﵽ10^4,ͨ��û�а취��ͼ������ʾ��������Ҫ�������log��⡣

				  //ȡ�����е����ֵ�����ں�����ԭʱȥ��һ��
	double maxVal;
	minMaxLoc(mag, 0, &maxVal, 0, 0);

	//�޼�Ƶ��,���ͼ����л������������Ļ�,����Ƶ���ǲ��ԳƵ�,���Ҫ�޼�
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	ph = ph(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	Mat _magI = mag.clone();
	//�����ȹ�һ��������ʾ��Χ��
	normalize(_magI, _magI, 0, 1, CV_MINMAX);
	//imshow("before rearrange", _magI);

	//��ʾ����Ƶ��ͼ
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	//������������Ϊ��׼����magͼ��ֳ��Ĳ���
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(mag, mag, 0, 1, CV_MINMAX);
	//imshow("ԭͼ�Ҷ�ͼ", src);
	//imshow("Ƶ�׷���", mag);
	mag = mag * 255;
	imwrite("ԭƵ��.jpg", mag);
	/*--------------------------------------------------*/

	mag = mag / 255;
	//cv::Mat mask;
	cv::Mat mag2;
	cv::Mat ph2;
	Mat proceMag;

	dftDemo(mag2);
	mag2 = mag2 / 255;

	cv::Mat mask1 = Mat::zeros(mag.size(), CV_32FC1);//��ͨ
	circle(mask1, Point(mag.cols / 2, mag.rows / 2), 30, Scalar(1), -1);//mask����

	cv::Mat mask2 = Mat::ones(mag.size(), CV_32FC1);//��ͨ
	circle(mask2, Point(mag.cols / 2, mag.rows / 2), 30, Scalar(0), -1);//mask����
	//selectPolygon(mag, mask);

	mag = mag.mul(mask1);
	mag2 = mag2.mul(mask2);
	mag = mag + mag2;
	proceMag = mag * 255;

	imwrite("������Ƶ��.jpg", proceMag);

	//ǰ�����跴����һ�飬Ŀ����Ϊ����任��ԭͼ
	Mat q00(mag, Rect(0, 0, cx, cy));
	Mat q10(mag, Rect(cx, 0, cx, cy));
	Mat q20(mag, Rect(0, cy, cx, cy));
	Mat q30(mag, Rect(cx, cy, cx, cy));

	//��������
	q00.copyTo(tmp);
	q30.copyTo(q00);
	tmp.copyTo(q30);
	q10.copyTo(tmp);
	q20.copyTo(q10);
	tmp.copyTo(q20);

	mag = mag * maxVal;//����һ���ľ���ԭ 
	exp(mag, mag);//��Ӧ��ǰ��ȥ����
	mag = mag - Scalar::all(1);//��Ӧǰ��+1
	polarToCart(mag, ph, planes[0], planes[1]);//�ɷ�����mag����λ��ph�ָ�ʵ��planes[0]���鲿planes[1]
	merge(planes, 2, complexImg);//��ʵ���鲿�ϲ�


	//-----------------------����Ҷ����任-----------------------------------
	Mat ifft(Size(src.cols, src.rows), CV_8UC1);
	//����Ҷ��任
	idft(complexImg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);

	Rect rect(0, 0, src.cols, src.rows);
	dst = ifft(rect);
	dst = dst * 255;

	cv::Mat dspMat;
	dst.convertTo(dspMat, CV_8UC1);
	imshow("dst", dspMat);
	imshow("src", src);
	waitKey(0);

	return 0;

}
