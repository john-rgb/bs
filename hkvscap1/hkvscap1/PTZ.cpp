#include "StdAfx.h"
#include "PTZ.h"
#include "iostream"
#include "hkvscap1.h"
#include "HCNetSDK.h"

using namespace std;


PTZ::PTZ(void)
{
}


PTZ::~PTZ(void)
{
}
BEGIN_MESSAGE_MAP(PTZ, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void PTZ::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//PTZControlAll(lPlayHandle,23,0,5);
	LONG lPlayHandle = g_pMainDlg->GETPLAYHANDLE();
	PTZControlAll(lPlayHandle,23,0,5);
	CButton::OnLButtonDown(nFlags, point);
}


void PTZ::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	LONG lPlayHandle = g_pMainDlg->GETPLAYHANDLE();
	PTZControlAll(lPlayHandle,23,1,5);
	CButton::OnLButtonUp(nFlags, point);
}


// ��̨���ƺ���
void PTZ::PTZControlAll(long lRealHandle, DWORD dwPTZCommand, DWORD dwStop, int Speed)
{
	if(lRealHandle>=0)
	{
		BOOL ret;
		if(Speed>=1)
		{
			ret = NET_DVR_PTZControlWithSpeed(lRealHandle,dwPTZCommand,dwStop,Speed);
			if(!ret)
			{
				//MessageBox(_L("��̨����ʧ��!"));
				return;
			}
		}
		else
		{
			ret = NET_DVR_PTZControl(lRealHandle,dwPTZCommand,dwStop);
			if(!ret)
			{
				//MessageBox(_L("��̨����ʧ��!"));
				return;
			}
		}
	}
}
