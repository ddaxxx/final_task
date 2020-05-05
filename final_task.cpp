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
{//��װ��
private:
	VideoCapture capture;
public:
	identify_send(VideoCapture capture1) { capture1 = capture; }//���캯��
	Point identify(VideoCapture capture);//Ѱ�����ĵ�
	Point send(Point point);//���͵�����
};
Point identify_send::identify(VideoCapture capture)
{
	Point center;
	while (1)
	{
		capture >> frame;	// ��һ��3ͨ��ͼ��ת����3����ͨ��ͼ��
		split(frame, channels);//����ɫ��ͨ��
		grayImg = channels.at(2) - channels.at(0);//��ɫ��ȥ��ɫ������µ�ͼ��
		Mat binBrightImg;
		//��ֵ��
		threshold(grayImg, binBrightImg, 100, 255, cv::THRESH_BINARY);
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		//����
		dilate(binBrightImg, binBrightImg, element);
		vector<vector<Point>> lightContours;

		//������
		vector<Vec4i> hierarchy;
		findContours(binBrightImg.clone(), lightContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		//Ѱ�����������
		for (int i = 0; i < lightContours.size(); i++)
		{
			//��������
			drawContours(binBrightImg, lightContours, i, Scalar(255), 1, 8, hierarchy);

			//������������С������
			RotatedRect rect = minAreaRect(lightContours[i]);
			Point2f P[4];
			rect.points(P);
			center = (P[1] + P[3]);//�ҵ����ĵ�
		}
	}
	return center;
};
Point identify_send::send(Point point)
{//��ʱ������дͨ�ű��

};
int main()
{
	Point center;
	VideoCapture capture;
	capture.open("1.mp4");
	identify_send task1(capture);
	if (!capture.isOpened())
	{
		cout << "��Ƶû�д�" << endl;
		return -1;
	}
	namedWindow("��ȡ����Ƶ1");
	namedWindow("��ȡ����Ƶ2");
	center = task1.identify;
	circle(frame, center, 3, Scalar(0, 0, 255), 1, 8, 0);//�������ĵ�

	thread identify1(task1.identify(capture));//�����߳�
	thread send1(task1.send(center));
	identify1.join();
	send1.join();
	return 0;
}
