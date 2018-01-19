
// hkvscap1.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "hkvscap1dlg.h"
#include "PTZ.h"

extern Chkvscap1Dlg * g_pMainDlg;


// Chkvscap1App:
// 有关此类的实现，请参阅 hkvscap1.cpp
//

class Chkvscap1App : public CWinApp
{
public:
	Chkvscap1App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	// 全局窗口变量
	//Chkvscap1Dlg globaldlg;
};

extern Chkvscap1App theApp;