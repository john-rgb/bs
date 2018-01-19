#include "stdafx.h"
#include "JMPframe.h"

using namespace cv;


runsys::runsys()
{
	learningrate_a=0;


	t_framecount=0;
	N_history=30;
	f=2.0;q=30.0;c=0.5;//初始化


	T=0;
	Tmin=15;Tmax=100;
	v=3;//固定背景减法的背景更新参数

	pct1=0.008;pct2=0.005;//运动判定参数


	fymin=5;fymax=300;fy=0;
	lumda1=1.3;lumda2=0.55;
	learningrate_a0=0.005;lumda3=0.1;//未GMM的学习率参数


	Count_g=0;
	learningrate_a1=0.0025;
	lumda4=400;///GMM学习率参数

	Si=1;
	C0=60;Ci=0;
	lumda5=5;J0=2;
	J=0;Jmin=10;Jmax=90;//隔帧检测

	THDkey=10;

}


MyGMM mog;
Mat backgframe;
Mat DiffFrame;
int lastfy=0;

int get_T(int tmpj,float tmpv,int tmptmin,int tmptmax)
{
	int tmpt=tmpv*tmpj;
	tmpt=max(tmpt,tmptmin);
	tmpt=min(tmpt,tmptmax);
	return tmpt;
}
int get_fy(int _lastfy,int _fymin,int _fymax,int _counts,float _lumda1,float _lumda2)
{
	if(_counts<=1)
		return _fymin;
	if(_counts<=_lumda1*_lastfy)
		return _lastfy;
	else
		return min((int)((1+_lumda2)*_lastfy+1),_fymax);

}

int get_J(int _ci,int _c0,int _jmin,int _jmax,int _j0,int _lumda5)
{
	if(_ci<=_c0)return 0;
	else return min(_jmin+(_ci-_c0)/_lumda5,_jmax); 
}

bool runsys::operator()(Mat& Frame,Mat& FrontFrame)
{
	t_framecount++;
	if(t_framecount<=N_history)//初始化
	{
		learningrate_a=f/(t_framecount+q)+c/N_history;
		mog(Frame,FrontFrame,learningrate_a,Frame,true);
	}
	else
	{
		if(J==0)

		{

			//以周期T更新固定背景
			if(T==0)
			{
				T=get_T(J,v,Tmin,Tmax);
				mog.GetGmmBackground(backgframe);
			}
			else
				T--;

			//固定背景减法
			absdiff(backgframe,Frame,DiffFrame);
			threshold(DiffFrame,DiffFrame,THDkey,255,THRESH_BINARY);
			Dct1=(countNonZero(DiffFrame)>pct1*DiffFrame.size().width*DiffFrame.size().height);


			//不进行GMM检测时
			if (!Dct1)
			{
				Count_s++;
				Count_g=0;
				Dct2=false;
				//	Ci++;

				if(fy==0)
				{
					fy=get_fy(lastfy,fymin,fymax,Count_s,lumda1,lumda2);
					lastfy=fy;
					learningrate_a=learningrate_a0+lumda3/fy;
					mog(Frame,FrontFrame,learningrate_a,FrontFrame,true);
				}
				else
					fy--;
				FrontFrame=0;
			}
			else//GMM检测
			{
				Count_g++;
				Count_s=0;
				learningrate_a=1/(lumda4+Count_g*Count_g)+learningrate_a1;
				mog(Frame,FrontFrame,learningrate_a,DiffFrame,false);
				Dct2=(countNonZero(FrontFrame)>pct2*DiffFrame.size().width*DiffFrame.size().height);
			}

			if(Dct2)
				Ci=0;
			else
				Ci++;
			J=get_J(Ci,C0,Jmin,Jmax,J0,lumda5);



// 			namedWindow("1");
// 			imshow("1",DiffFrame);
// 			waitKey(1);
		}
		else
		{
			FrontFrame=0;
			J--;
		}

	}



	return true;
}

runsys::~runsys()
{

}