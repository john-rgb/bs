#ifndef Improved___G__M__M
#define Improved___G__M__M
#include <opencv2/opencv.hpp>
#include <math.h>
#include <list>
#include "stdafx.h"
using namespace cv;


class MyGMM
{
public:
	//! the default constructor
	MyGMM();
	//! the full constructor that takes the length of the history, the number of gaussian mixtures, the background ratio parameter and the noise strength
	MyGMM(int history, int nmixtures, double backgroundRatio, double noiseSigma=0);
	//! the destructor
	~MyGMM();
	//! the update operator
	void operator()(InputArray image, OutputArray fgmask, double learningRate=0,InputArray GMM_mask=NULL,bool nomask=true);
	void GetGmmBackground(Mat& _BackgroundImage);
	//! re-initiaization method
	void initialize(Size frameSize, int frameType);

	//	virtual AlgorithmInfo* info() const;

protected:    
	Size frameSize;
	int frameType;
	Mat bgmodel;
	int nframes;
	//int history;//利用历史帧数计算学习速率，不是主要参数
	int nmixtures;//高斯模型的个数
	double varThreshold;//方差门限
	double backgroundRatio;//背景门限
	double noiseSigma;//噪声方差
};	
#endif