#pragma once
#include "afxwin.h"
class PTZ :
	public CButton
{
public:
	PTZ(void);
	~PTZ(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	// ��̨���ƺ���
	void PTZControlAll(long lRealHandle, DWORD dwPTZCommand, DWORD dwStop, int Speed);
};

