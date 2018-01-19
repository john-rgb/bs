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
	float learningrate_a;//实际学习率

	float f,q,c;
	int t_framecount,N_history;//初始化


	int T,Tmin,Tmax;float v;//固定背景减法的背景更新参数

	int s;bool Dct1,Dct2;float pct,pct1,pct2;//运动判定参数

	int fy,fymin,fymax;
	int Count_s;
	float learningrate_a0;
	float lumda1,lumda2,lumda3;//未GMM的学习率参数

	float lumda4,learningrate_a1;
	int Count_g;//GMM学习率参数

	int Si;int J0;
	int J,Jmin,Jmax;
	int Ci,C0;
	int lumda5;//隔帧检测参数


	int THDkey;
};

