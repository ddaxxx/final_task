#include<opencv2\opencv.hpp>
#include<iostream>
#include<vector>
#include<thread>
using namespace cv;
using namespace std;
Mat frame;
Scalar color;
vector<Mat> channels;
Mat grayImg;
class identify_send
{//封装类
private:
	VideoCapture capture;
public:
	identify_send(VideoCapture capture1) { capture1 = capture; }//构造函数
	Point identify(VideoCapture capture);//寻找中心点
	Point send(Point point);//发送点坐标
};
Point identify_send::identify(VideoCapture capture)
{
	Point center;
	while (1)
	{
		capture >> frame;	// 把一个3通道图像转换成3个单通道图像
		split(frame, channels);//分离色彩通道
		grayImg = channels.at(2) - channels.at(0);//红色减去蓝色，获得新的图像
		Mat binBrightImg;
		//阈值化
		threshold(grayImg, binBrightImg, 100, 255, cv::THRESH_BINARY);
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		//膨胀
		dilate(binBrightImg, binBrightImg, element);
		vector<vector<Point>> lightContours;

		//找轮廓
		vector<Vec4i> hierarchy;
		findContours(binBrightImg.clone(), lightContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		//寻找最外层轮廓
		for (int i = 0; i < lightContours.size(); i++)
		{
			//绘制轮廓
			drawContours(binBrightImg, lightContours, i, Scalar(255), 1, 8, hierarchy);

			//绘制轮廓的最小外结矩形
			RotatedRect rect = minAreaRect(lightContours[i]);
			Point2f P[4];
			rect.points(P);
			center = (P[1] + P[3]);//找到中心点
		}
	}
	return center;
};
Point identify_send::send(Point point)
{//暂时还不会写通信编程

};
int main()
{
	Point center;
	VideoCapture capture;
	capture.open("1.mp4");
	identify_send task1(capture);
	if (!capture.isOpened())
	{
		cout << "视频没有打开" << endl;
		return -1;
	}
	namedWindow("提取的视频1");
	namedWindow("提取的视频2");
	center = task1.identify;
	circle(frame, center, 3, Scalar(0, 0, 255), 1, 8, 0);//绘制中心点

	thread identify1(task1.identify(capture));//两个线程
	thread send1(task1.send(center));
	identify1.join();
	send1.join();
	return 0;
}
