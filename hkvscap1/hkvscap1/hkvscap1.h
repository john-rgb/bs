
// hkvscap1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "hkvscap1dlg.h"
#include "PTZ.h"

extern Chkvscap1Dlg * g_pMainDlg;


// Chkvscap1App:
// �йش����ʵ�֣������ hkvscap1.cpp
//

class Chkvscap1App : public CWinApp
{
public:
	Chkvscap1App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	// ȫ�ִ��ڱ���
	//Chkvscap1Dlg globaldlg;
};

extern Chkvscap1App theApp;