#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat bin_image;

/****************************************寻找赛道中线****************************************/
#define ROW 108
#define COL 192
#define WHITE 255
#define BLACK 0
#define MidLine 107
#define effective_left 15
#define effective_right COL-16

int   MiddleLine[ROW];
int   RightEdge[ROW];
int   LeftEdge[ROW];
int   Width[ROW];//动态路宽

int   MidPri = MidLine;
int   LeftLose = 0;
int   RightLose = 0;
int   AllLose = 0;
int   signalFlag_row = 0;
int   signalFlag_frame = 0;

//全行扫描和边缘结合提取赛道的中线
//前十行全行扫描
//剩下的行，根据上一行的寻线情况来决定当前行的寻线方式和起点
int findTrack()
{
	int16_t i = 0;
	int16_t j = 0;
	uint8_t jj = 0;

	LeftLose = 0;//变量清零
	RightLose = 0;
	AllLose = 0;
	for (i = 0; i < ROW; i++)//赋初值
	{
		RightEdge[i] = COL - 1;
		LeftEdge[i] = 0;
		MiddleLine[i] = MidLine;
		Width[i] = 77+0.2447*i;     //动态路宽
	}

	for (i = ROW - 1; i >= ROW - 10; i--) //首先找前十行，全行扫描
	{
		signalFlag_row = 0;
		if (i == ROW - 1)//首行就以图像中心作为扫描起点
		{
			j = MidPri;
		}
		else
		{
			j = MiddleLine[i + 1];//否则就以上一行中点的位置作为本行扫描起点
		}
		if (j <= effective_left)
		{
			j = effective_left;
		}
		while (j >= effective_left)//j>=effective_left有效范围内进行搜寻
		{
			if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j - 1) == BLACK && bin_image.at<uchar>(i, j - 2) == BLACK)//从右向左找到白黑黑跳变
			{
				LeftEdge[i] = j;//找到则赋值 找不到保持原值0
				break;//跳出本行寻线
			}
			j--;//列数往左移动
		}
		if (i == ROW - 1) //再找右边界
		{
			j = MidPri;//如果首行，从图像中心开始搜寻
		}
		else
		{
			j = MiddleLine[i + 1];//否则从上一行中心位置开始搜寻
		}
		if (j >= effective_right)//有效范围内搜寻右线
		{
			j = effective_right;
		}
		while (j <= effective_right)
		{
			if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j + 1) == BLACK && bin_image.at<uchar>(i, j + 2) == BLACK)//从左向右找到白白黑跳变点
			{
				RightEdge[i] = j;//找到则赋值   找不到保持原值
				break;//跳出本行寻线
			}
			j++;//列数往右移动
		}
		if (LeftEdge[i] > 60) {
			LeftEdge[i] = 0;//标志物影响边界，直接判定为丢线
			signalFlag_row = 1;
			signalFlag_frame = 1;
		}
		if (RightEdge[i] < 145) {
			RightEdge[i] = COL - 1;
			signalFlag_row = 1;
			signalFlag_frame = 1;
		}
		if (LeftEdge[i] >= effective_left && RightEdge[i] <= effective_right)//中线判断，没有丢线
		{
			MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;
		}
		else if (LeftEdge[i] >= effective_left && RightEdge[i] > effective_right)//丢了右线,没有丢左线
		{
			if (!signalFlag_row)
			{
				RightLose++;
			}
			//如果当行的
			if (i != ROW - 1 && ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 5)))//突变
			{
				MiddleLine[i] = MiddleLine[i + 1];
			}
			else
			{
				MiddleLine[i] = LeftEdge[i] + Width[i] / 2;//正常的话就用半宽补
			}
		}
		else if (LeftEdge[i] < effective_left && RightEdge[i] <= effective_right)//丢了左线
		{
			if (!signalFlag_row)
			{
				LeftLose++;
			}
			if (i != ROW - 1 && (RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 5))//突变
			{
				MiddleLine[i] = MiddleLine[i + 1];
			}
			else
			{
				MiddleLine[i] = RightEdge[i] - Width[i] / 2;//线宽
			}
		}
		else if (LeftEdge[i]<effective_left && RightEdge[i]>effective_right)//两边都丢了的话
		{
			if (!signalFlag_row)
			{
				AllLose++;
			}
			if (i == ROW - 1)//如果是首行就以图像中心作为中点
			{
				MiddleLine[i] = MidPri;
			}
			else
			{
				MiddleLine[i] = MiddleLine[i + 1];//如果不是首行就用上一行的中线作为本行中点
			}
		}

		if (MiddleLine[ROW - 1] >= effective_right)
		{
			MidPri = effective_right;
		}
		else if (MiddleLine[ROW - 1] <= effective_left)
		{
			MidPri = effective_left;
		}
		else
		{
			MidPri = MiddleLine[ROW - 1];//记录本帧图像第63行的中线值，作为下一幅图像的63行扫描起始点
		}
	}
	for (i = ROW - 11; i >= 25; i--)//查找剩余行
	{
		signalFlag_row = 0;
		if (LeftEdge[i + 1] >= effective_left && RightEdge[i + 1] <= effective_right) //上一行两边都找到 启用边沿扫描
		{
			j = ((LeftEdge[i + 1] + 10) > effective_right) ? effective_right : (LeftEdge[i + 1] + 10);//先找左边界
			jj = ((LeftEdge[i + 1] - 5) < effective_left) ? effective_left : (LeftEdge[i + 1] - 5);
			while (j >= jj)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j - 1) == BLACK && bin_image.at<uchar>(i, j - 2) == BLACK)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
			j = ((RightEdge[i + 1] - 10) < effective_left) ? effective_left : (RightEdge[i + 1] - 10); //在找右边界
			jj = ((RightEdge[i + 1] + 5) > effective_right) ? effective_right : (RightEdge[i + 1] + 5);
			while (j <= jj)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j + 1) == BLACK && bin_image.at<uchar>(i, j + 2) == BLACK)
				{
					RightEdge[i] = j;
					break;
				}
				j++;
			}
		}
		else if (LeftEdge[i + 1] >= effective_left && RightEdge[i + 1] > effective_right)//上一行只找到左边界
		{
			j = ((LeftEdge[i + 1] + 10) > effective_right) ? effective_right : (LeftEdge[i + 1] + 10);//左边界用边沿扫描
			jj = ((LeftEdge[i + 1] - 5) < effective_left) ? effective_left : (LeftEdge[i + 1] - 5);
			while (j >= jj)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j - 1) == BLACK && bin_image.at<uchar>(i, j - 2) == BLACK)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
			j = MiddleLine[i + 1];//上一行丢了右边界用全行扫描
			if (j >= effective_right)
			{
				j = effective_right;
			}
			while (j <= effective_right)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j + 1) == BLACK && bin_image.at<uchar>(i, j + 2) == BLACK)
				{
					RightEdge[i] = j;
					break;
				}
				j++;
			}
		}
		else if (LeftEdge[i + 1] < effective_left && RightEdge[i + 1] <= effective_right) //上一行只找到右边界
		{
			j = ((RightEdge[i + 1] - 10) < effective_left) ? effective_left : (RightEdge[i + 1] - 10);//边缘追踪找右边界
			jj = ((RightEdge[i + 1] + 5) > effective_right) ? effective_right : (RightEdge[i + 1] + 5);
			while (j <= jj)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j + 1) == BLACK && bin_image.at<uchar>(i, j + 2) == BLACK)
				{
					RightEdge[i] = j;
					break;
				}
				j++;
			}
			j = MiddleLine[i + 1]; //全行扫描找左边界
			if (j < effective_left)
			{
				j = effective_left;
			}
			while (j >= effective_left)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j - 1) == BLACK && bin_image.at<uchar>(i, j - 2) == BLACK)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
		}
		else if (LeftEdge[i + 1] < effective_left && RightEdge[i + 1] > effective_right)  //上一行没找到边界，可能是十字或者环形或者是标志物影响
		{
			j = MiddleLine[i + 1];   //全行找左边界
			while (j >= effective_left)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j - 1) == BLACK && bin_image.at<uchar>(i, j - 2) == BLACK)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
			j = MiddleLine[i + 1];   //全行找右边界
			while (j <= effective_right)
			{
				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i, j + 1) == BLACK && bin_image.at<uchar>(i, j + 2) == BLACK)
				{
					RightEdge[i] = j;
					break;
				}
				j++;
			}
		}
		if (LeftEdge[i] > 60) {
			LeftEdge[i] = 0;//标志物影响边界，直接判定为丢线
			signalFlag_row = 1;
		}
		if (RightEdge[i] < 145) {
			RightEdge[i] = COL - 1;
			signalFlag_row = 1;
		}
		if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 5))//突变
		{
			MiddleLine[i] = MiddleLine[i + 1];//用上一行
		}
		else
		{
			if (LeftEdge[i] >= effective_left && RightEdge[i] <= effective_right)//不丢线
			{
				MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;
			}
			else if (LeftEdge[i] >= effective_left && RightEdge[i] > effective_right)//lose right
			{
				if (!signalFlag_row)
				{
					RightLose++;
				}

				if (LeftEdge[i + 1] >= effective_left)
				{
					MiddleLine[i] = MiddleLine[i + 1] + LeftEdge[i] - LeftEdge[i + 1];
				}
				else
				{
					MiddleLine[i] = LeftEdge[i] + Width[i] / 2;
				}
			}
			else if (LeftEdge[i] < effective_left && RightEdge[i] <= effective_right)//lose left
			{
				if (!signalFlag_row)
				{
					LeftLose++;
				}
				if (RightEdge[i + 1] <= effective_right)
				{
					MiddleLine[i] = MiddleLine[i + 1] + RightEdge[i] - RightEdge[i + 1];
				}
				else
				{
					MiddleLine[i] = RightEdge[i] - Width[i] / 2;
				}
			}
			else if (LeftEdge[i]<effective_left && RightEdge[i]>effective_right)//两边丢线
			{
				if (!signalFlag_row)
				{
					AllLose++;
				}
				MiddleLine[i] = MiddleLine[i + 1];
			}
		}
		uint16_t m = MiddleLine[i];
		if (m < effective_left)
		{
			m = effective_left;
		}
		if (m > effective_right)
		{
			m = effective_right;
		}
	}
	return 0;
}

//int BlackEndMR = 0;
//int BlackEndML = 0;
//int BlackEndLL = 0;
//int BlackEndRR = 0;
//int BlackEndL = 0;
//int BlackEndM = 0;
//int BlackEndR = 0;
//int BlackEndMaxMax = 0;
//int BlackEndMax = 0;
//int DropRow = 0;

////提取图像的特征
////选取几列，从图像底部往上扫描
////获取黑线截止行
//static void GetBlackEndParam()
//{
//	unsigned char LEndFlag = 0;//标志位
//	unsigned char MEndFlag = 0;
//	unsigned char REndFlag = 0;
//	unsigned char MREndFlag = 0;
//	unsigned char MLEndFlag = 0;
//	unsigned char LLEndFlag = 0;
//	unsigned char RREndFlag = 0;
//
//	int i = 0;
//
//	BlackEndMR = 0;//清零
//	BlackEndML = 0;
//	BlackEndLL = 0;
//	BlackEndRR = 0;
//	BlackEndL = 0;
//	BlackEndM = 0;
//	BlackEndR = 0;
//
//	for (i = ROW - 1; i >= 25; i--)
//	{
//		if (bin_image.at<uchar>(i, COL / 2) == WHITE && !MEndFlag)//94
//		{
//			BlackEndM++;//中黑线截至行
//		}
//		else if (bin_image.at<uchar>(i-1, COL / 2) == BLACK && bin_image.at<uchar>(i-2, COL / 2) == BLACK)//连续两行是黑色
//		{
//			MEndFlag = 1;
//		}
//		if (bin_image.at<uchar>(i, COL / 4) == WHITE && !LEndFlag)//47
//		{
//			BlackEndL++;//左黑线截至行
//		}
//		else if (bin_image.at<uchar>(i-1, COL / 4) == BLACK &&bin_image.at<uchar>(i-2, COL / 4) == BLACK)
//		{
//			LEndFlag = 1;
//		}
//		if (bin_image.at<uchar>(i, COL * 3 / 4) == WHITE && !REndFlag)//141
//		{
//			BlackEndR++;//右黑线截至行
//		}
//		else if (bin_image.at<uchar>(i-1, COL * 3 / 4) == BLACK && bin_image.at<uchar>(i-2, COL * 3 / 4) == BLACK)
//		{
//			REndFlag = 1;
//		}
//		if (bin_image.at<uchar>(i, COL * 3 / 8) == WHITE && !MLEndFlag)//偏左
//		{
//			BlackEndML++;
//		}
//		else if (bin_image.at<uchar>(i-1, COL * 3 / 8) == BLACK && bin_image.at<uchar>(i-2, COL * 3 / 8) == BLACK)
//		{
//			MLEndFlag = 1;
//		}
//		if (bin_image.at<uchar>(i, COL * 5 / 8) == WHITE && !MREndFlag)
//		{
//			BlackEndMR++;
//		}
//		else if (bin_image.at<uchar>(i-1, COL * 5 / 8) == BLACK && bin_image.at<uchar>(i-2, COL * 5 / 8) == BLACK)
//		{
//			MREndFlag = 1;
//		}
//		if (bin_image.at<uchar>(i, COL / 8) == WHITE && !LLEndFlag)
//		{
//			BlackEndLL++;
//		}
//		else if (bin_image.at<uchar>(i-1, COL / 8) == BLACK && bin_image.at<uchar>(i-2, COL / 8) == BLACK)
//		{
//			LLEndFlag = 1;
//		}
//		if (bin_image.at<uchar>(i, COL * 7 / 8) == WHITE && !RREndFlag)
//		{
//			BlackEndRR++;
//		}
//		else if (bin_image.at<uchar>(i-1, COL * 7 / 8) == BLACK && bin_image.at<uchar>(i - 2, COL * 7 / 8) == BLACK)
//		{
//			RREndFlag = 1;
//		}
//	}
//
//	BlackEndMax = MAX(BlackEndL, BlackEndM);//取大值
//	BlackEndMax = MAX(BlackEndMax, BlackEndR);
//	BlackEndMaxMax = MAX(BlackEndMax, BlackEndMR);
//	BlackEndMaxMax = MAX(BlackEndMax, BlackEndML);
//	BlackEndMaxMax = MAX(BlackEndMax, BlackEndLL);
//	BlackEndMaxMax = MAX(BlackEndMax, BlackEndRR);
//	DropRow = ROW - BlackEndMaxMax;//黑线截止行
//}

/****************************************判断赛道类型****************************************/
typedef struct
{
	unsigned char InflectionPointRow;//拐点的行坐标
	unsigned char InflectionPointCol;//拐点的列坐标
	unsigned char InflectionPointFlag;//拐点的标志位
	unsigned char ScanInflectionPointStart;//扫描拐点的起始行
	unsigned char ScanInflectionPointEnd;//扫描拐点结束行 
	unsigned char InflectionPointNumber;//拐点的个数

}InflectionPoint;//建立拐点结构体

typedef struct
{
	unsigned char CrossLR;//十字
	unsigned char CrossL;//左岔路
	unsigned char CrossR;//右岔路

}CrossType;//建立十字类型的结构体

//对右边线进行操作，如果存在多个拐点寻找最近的拐点
//prt:传入一个结构体变量的地址
//Start：扫描的起始行
//End ：扫描的结束行
void FindInflectionPointR(InflectionPoint *prt, unsigned char Start, unsigned char End)
{
	int i;
	//初始化结构体变量
	prt->ScanInflectionPointStart = Start;//设置扫描的起始行
	prt->ScanInflectionPointEnd = End;//设置扫描的结束行
	prt->InflectionPointRow = 0;
	prt->InflectionPointCol = 0;
	prt->InflectionPointFlag = 0;

	for (i = prt->ScanInflectionPointStart; i >= prt->ScanInflectionPointEnd; i--) //不能扫描太远，否则会误判
	{
		if (RightEdge[i] != COL - 1 && RightEdge[i + 1] != COL - 1)
		{
			if ((RightEdge[i] - RightEdge[i + 1] <= 0) && (RightEdge[i - 1] - RightEdge[i]) >= 1 && (RightEdge[i - 2] - RightEdge[i]) >= 2)//找到右边线有拐点
			{
				prt->InflectionPointRow = i;//记录拐点的行
				prt->InflectionPointCol = RightEdge[i];//记录拐点的列
				prt->InflectionPointFlag = 1;//标记找到右拐点
				break;//退出for
			}
		}
	}
}

//对左边线进行操作，如果存在多个拐点寻找最近的拐点
//prt:传入一个结构体变量的地址
//Start：扫描的起始行
//End ：扫描的结束行
void FindInflectionPointL(InflectionPoint *prt, unsigned char Start, unsigned char End)
{
	int i;
	//初始化结构体变量
	prt->ScanInflectionPointStart = Start;//设置扫描的起始行
	prt->ScanInflectionPointEnd = End;//设置扫描的结束行
	prt->InflectionPointRow = 0;
	prt->InflectionPointCol = 0;
	prt->InflectionPointFlag = 0;

	//左拐点
	for (i = prt->ScanInflectionPointStart; i > prt->ScanInflectionPointEnd; i--)
	{
		if (LeftEdge[i] != 0 && LeftEdge[i + 1] != 0)
		{
			if ((LeftEdge[i] - LeftEdge[i + 1] >= 0) && (LeftEdge[i] - LeftEdge[i - 1] >= 3) && (LeftEdge[i] - LeftEdge[i - 2] >= 5))//找到拐点
			{
				prt->InflectionPointRow = i;//记录该拐点的行
				prt->InflectionPointCol = LeftEdge[i];//记录该拐点的列
				prt->InflectionPointFlag = 1;//标记找到左拐点
				break;//找到退出
			}
		}
	}
}

InflectionPoint  InflectionPointL;//建立左边拐点结构体变量
InflectionPoint  InflectionPointR;//建立右边拐点结构体变量
uint8_t RCross_flag = 0;
uint8_t LCross_flag = 0;
uint8_t LRCross_flag = 0;
uint8_t turn_count = 0;
unsigned char LeftScanCol[COL];
unsigned char RightScanCol[COL];

//void CrossRecognition(CrossType *prt)
//{
//	int i, j, ScanEndCol;
//
//	//初始化变量
//	prt->CrossLR = 0;
//	prt->CrossL = 0;
//	prt->CrossR = 0;
//
//	//寻找左右拐点
//	FindInflectionPointR(&InflectionPointR, ROW - 4, 25);
//	FindInflectionPointL(&InflectionPointL, ROW - 4, 25);
//	//如果左边界和右边界同时丢线大于10行，就认为遇到了十字
//	if (AllLose >= 10)
//	{
//		cout << "十字" << std::endl;
//		prt->CrossLR = 1;//十字
//	}
//	//判断右分岔路
//	else if (InflectionPointR.InflectionPointFlag //存在右边拐点
//		&&InflectionPointL.InflectionPointFlag == 0//并且不存在左边拐点
//		&& RightLose >= 5)
//	{
//		cout << "右拐点" << std::endl;
//		for (i = 0; i < COL - 1; i++)
//		{
//			RightScanCol[i] = 0;//初始化数组
//		}
//		//选择最远的那一列
//		if (BlackEndM >= BlackEndML && BlackEndM >= BlackEndLL)//中间的视野最远
//		{
//			ScanEndCol = COL / 2;
//		}
//		else if (BlackEndML > BlackEndM&&BlackEndML > BlackEndLL)//中间偏左视野最远
//		{
//			ScanEndCol = COL * 3 / 8;
//		}
//		else if (BlackEndLL > BlackEndM&&BlackEndLL > BlackEndML)//最左视野最远
//		{
//			ScanEndCol = COL / 4;
//		}
//		//以右拐点的列作为基准，视野最远的一列最为结束，从右往左竖向扫描
//		//寻找十字上面的直角
//		for (j = InflectionPointR.InflectionPointCol; j > ScanEndCol; j--)
//		{
//			//以拐点的行为起始基准，最远列的行作为结束行
//			for (i = InflectionPointR.InflectionPointRow; i > DropRow; i--)
//			{
//				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i-1, j) == BLACK && bin_image.at<uchar>(i-2, j) == BLACK)
//				{
//					RightScanCol[j] = i - 1;
//					break;
//				}
//			}
//		}
//		for (j = InflectionPointR.InflectionPointCol; j > ScanEndCol; j--)
//		{
//			if (RightScanCol[j] >= 2 && RightScanCol[j - 1] >= 2 && RightScanCol[j + 1] >= 2)
//				if (RightScanCol[j] - RightScanCol[j + 1] >= 0 && RightScanCol[j] - RightScanCol[j - 1] >= 1)
//				{
//					prt->CrossR = 1;
//					break;
//				}
//		}
//	}
//	//判断左分岔路
//	else if (InflectionPointR.InflectionPointFlag == 0//并且不存在右边拐点
//		&& InflectionPointL.InflectionPointFlag//存在左边拐点
//		&& LeftLose >= 5)
//	{
//		cout << "左拐点" << std::endl;
//		for (i = 0; i < COL - 1; i++)
//		{
//			LeftScanCol[i] = 0;//初始化数组
//		}
//		//选择最远的那一列
//		if (BlackEndM >= BlackEndMR && BlackEndM >= BlackEndRR)
//		{
//			ScanEndCol = COL / 2;
//		}
//		else if (BlackEndMR > BlackEndM&&BlackEndMR > BlackEndRR)
//		{
//			ScanEndCol = COL * 5 / 8;
//		}
//		else if (BlackEndRR > BlackEndM&&BlackEndRR > BlackEndMR)
//		{
//			ScanEndCol = COL * 3 / 4;
//		}
//		for (j = InflectionPointL.InflectionPointCol; j < ScanEndCol; j++)
//		{
//			//以拐点的行为起始基准，最远列的行作为结束行
//			for (i = InflectionPointL.InflectionPointRow; i > DropRow; i--)
//			{
//				if (bin_image.at<uchar>(i, j) == WHITE && bin_image.at<uchar>(i-1, j) == BLACK && bin_image.at<uchar>(i-2, j) == BLACK)
//				{
//					LeftScanCol[j] = i - 1;
//					break;
//				}
//			}
//		}
//		for (j = InflectionPointL.InflectionPointCol; j < ScanEndCol; j++)
//		{
//			if (LeftScanCol[j] >= 2 && LeftScanCol[j - 1] >= 2 && LeftScanCol[j + 1] >= 2)
//				if (LeftScanCol[j] - LeftScanCol[j + 1] >= 1 && LeftScanCol[j] - LeftScanCol[j - 1] >= 0)
//				{
//					prt->CrossL = 1;//标志左岔路
//					break;
//				}
//		}
//	}
//}

void CrossRecognition(CrossType *prt)
{
	//初始化变量
	prt->CrossLR = 0;
	prt->CrossL = 0;
	prt->CrossR = 0;

	//寻找左右拐点
	FindInflectionPointR(&InflectionPointR, ROW - 1, 30);
	FindInflectionPointL(&InflectionPointL, ROW - 1, 30);
	//如果左边界和右边界同时丢线大于5行，就认为遇到了十字
	if (InflectionPointR.InflectionPointFlag
		&&InflectionPointL.InflectionPointFlag
		&&AllLose >= 3)
	{
		prt->CrossLR = 1;//十字
	}
	//判断右分岔路
	else if (InflectionPointR.InflectionPointFlag //存在右边拐点
		&& RightLose >= 5 && LeftLose < 3 && AllLose < 3)
	{
		prt->CrossR = 1;
	}
	//判断左分岔路
	else if (InflectionPointL.InflectionPointFlag//存在左边拐点
		&& LeftLose >= 5 && RightLose < 3 && AllLose < 3)
	{
		prt->CrossL = 1;//标志左岔路
	}
}

CrossType Cross;//建立十字结构体变量
//岔路口需要的指令
#define cross_straight  1
#define cross_right     2
#define cross_left      3
int cross_state = cross_straight;

//得到十字类型的相关标志作为控制标志位
static void Crossflag(void)
{
	if (Cross.CrossLR)//十字
	{
		cout << "十字" << std::endl;
		if (cross_state == cross_straight)//直行信号
		{
			cout << "十字控制直行" << std::endl;
			LRCross_flag = cross_straight;
			cross_state = 0;
		}
		else if (cross_state == cross_right)//右转信号
		{
			cout << "十字控制右转" << std::endl;
			LRCross_flag = cross_right;
			cross_state = 0;
		}
		else if (cross_state == cross_left)//左转信号
		{
			cout << "十字控制左转" << std::endl;
			LRCross_flag = cross_left;
			cross_state = 0;
		}
	}
	else if (Cross.CrossR)//右分叉
	{
		cout << "右分叉" << std::endl;
		if (cross_state == cross_straight)//直行信号
		{
			cout << "右分叉控制直行" << std::endl;
			RCross_flag = cross_straight;
			cross_state = 0;
		}
		else if (cross_state == cross_right)//右转信号
		{
			cout << "右分叉控制右转" << std::endl;
			RCross_flag = cross_right;
			cross_state = 0;
		}
	}
	else if (Cross.CrossL)//左分叉
	{
		cout << "左分叉" << std::endl;
		if (cross_state == cross_straight)//直行信号
		{
			cout << "左分叉控制直行" << std::endl;
			LCross_flag = cross_straight;
			cross_state = 0;
		}
		else if (cross_state == cross_left)//左转信号
		{
			cout << "左分叉控制左转" << std::endl;
			LCross_flag = cross_left;
			cross_state = 0;
		}
	}
}

//对十字进行处理，得出舵机的控制中线
void CrossProcess(void)
{
	Crossflag();
	//十字
	if (LRCross_flag == cross_straight) {//直行

	}
	else if (LRCross_flag == cross_right) {//右转
		if (turn_count >= 2)
		{
		}
		else
		{
			if (InflectionPointR.InflectionPointFlag == 0
				&& AllLose >= 3) {
				turn_count++;
			}
			else {
				turn_count = 0;
			}
		}
	}
	else if (LRCross_flag == cross_left) {//左转
		if (turn_count >= 2)
		{
		}
		else
		{
			if (InflectionPointL.InflectionPointFlag == 0
				&& AllLose >= 3) {
				turn_count++;
			}
			else {
				turn_count = 0;
			}
		}
	}
	//右岔路
	if (RCross_flag == cross_straight) {//直行
	}
	else if (RCross_flag == cross_right) {//右转
		if (turn_count >= 7)
		{
		}
		else
		{
			if (InflectionPointR.InflectionPointFlag == 0
				&& InflectionPointL.InflectionPointFlag == 0
				&& RightLose >= 5 && LeftLose < 3 && AllLose < 3) {
				turn_count++;
			}
			else {
				turn_count = 0;
			}
		}
	}
	//左岔路
	if (LCross_flag == cross_straight) {//直行
	}
	else if (LCross_flag == cross_left) {//左转
		if (turn_count >= 7)
		{
		}
		else
		{
			if (InflectionPointR.InflectionPointFlag == 0
				&& InflectionPointL.InflectionPointFlag == 0
				&& LeftLose >= 5 && RightLose < 3 && AllLose < 3) {
				turn_count++;
			}
			else {
				turn_count = 0;
			}
		}
	}

	//在图像恢复正常后重置标志位
	if (LeftLose < 3 && RightLose < 3 && AllLose < 3
		&& fabs(LeftEdge[40] + Width[40] / 2 - MidLine) < 13 && fabs(RightEdge[40] - Width[40] / 2 - MidLine) < 13
		&& fabs(LeftEdge[70] + Width[70] / 2 - MidLine) < 13 && fabs(RightEdge[70] - Width[70] / 2 - MidLine) < 13
		&& fabs(LeftEdge[100] + Width[100] / 2 - MidLine) < 13 && fabs(RightEdge[100] - Width[100] / 2 - MidLine) < 13
		&& (RCross_flag || LCross_flag || LRCross_flag))
	{
		RCross_flag = 0; LCross_flag = 0; LRCross_flag = 0;
		turn_count = 0;
		signalFlag_frame = 0;
		cout << "重置成功" << std::endl;
	}
}


int movieProcess()
{
	VideoCapture capVideo("..\\movie\\test2.mp4");
	//如果视频打开失败
	if (!capVideo.isOpened()) {
		std::cout << "Unable to open video!" << std::endl;
		return -1;
	}

	double scale = 0.1;

	Mat frame;
	Mat rframe;
	Mat frame_gray;
	Mat frame_bin;

	std::vector<cv::Point> points;

	while (1)
	{
		capVideo >> frame;
		if (frame.empty()) break;//若视频结束，则退出
		Size ResImgSize = Size(frame.cols*scale, frame.rows*scale);
		rframe = Mat(ResImgSize, frame.type());
		resize(frame, rframe, ResImgSize, INTER_LINEAR);
		cvtColor(rframe, frame_gray, COLOR_BGR2GRAY);
		threshold(frame_gray, frame_bin, 100, 255, THRESH_OTSU);//大津法二值化

		bin_image = frame_bin;

		findTrack();
		CrossRecognition(&Cross);
		CrossProcess();

		//cout << "leftlose:" << LeftLose << " rightlose:" << RightLose << " alllose:" << AllLose << std::endl;
		points.clear();
		for (int i = ROW - 1; i >= 25; i--)
		{
			if (fabs(MiddleLine[i] - MiddleLine[i + 1]) > 3 && i != ROW - 1)//平滑中线
			{
				MiddleLine[i] = MiddleLine[i + 1];
			}
			//cout << "row" << i << " leftEdge:" << LeftEdge[i] << " rightEdge:" << RightEdge[i] << " middle:" << MiddleLine[i] << std::endl;
			points.push_back(Point(MiddleLine[i], i));
			//cout << "row" << i << " rightEdge-leftEdge:" << RightEdge[i] - LeftEdge[i] << std::endl;
		}

		polylines(rframe, points, false, Scalar(255, 255, 0), 2, 8, 0);//画拟合线

		imshow("rframe", rframe);
		imshow("bin_frame", frame_bin);
		waitKey(30);
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