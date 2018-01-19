#include <opencv2\opencv.hpp> 
#include "myGMM.h"
#include "stdafx.h"
using namespace cv;

class runsys
{
public:
	runsys();
	~runsys();
	bool operator()(Mat& Frame,Mat& FrontFrame);
private:
	float learningrate_a;//ʵ��ѧϰ��

	float f,q,c;
	int t_framecount,N_history;//��ʼ��


	int T,Tmin,Tmax;float v;//�̶����������ı������²���

	int s;bool Dct1,Dct2;float pct,pct1,pct2;//�˶��ж�����

	int fy,fymin,fymax;
	int Count_s;
	float learningrate_a0;
	float lumda1,lumda2,lumda3;//δGMM��ѧϰ�ʲ���

	float lumda4,learningrate_a1;
	int Count_g;//GMMѧϰ�ʲ���

	int Si;int J0;
	int J,Jmin,Jmax;
	int Ci,C0;
	int lumda5;//��֡������


	int THDkey;
};

