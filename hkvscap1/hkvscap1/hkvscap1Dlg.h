
// hkvscap1Dlg.h : 头文件
//

#pragma once
#include "highgui.h"  
#include "cv.h"
#include "CvvImage.h"
#include "ptz.h"
#include "HCNetSDK.h"



// Chkvscap1Dlg 对话框
class Chkvscap1Dlg : public CDialogEx
{
// 构造
public:
	Chkvscap1Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HKVSCAP1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton2();
	// 设备id
	long lUserID;
	long lUserID1;
	// 播放句柄
	long m_lPlayHandle;
	long m_lPlayHandle1;


	// 测试用图片
	IplImage* image1;
	// 显示图像到picture控件
	void ShowImage(IplImage * img, UINT ID);
	// 对话框的全局变量，用于传递一些全局都需要的数据
//	Chkvscap1Dlg *globaldlg;
	// 向右旋转按钮
	PTZ btnptz_left;
	LONG GETPLAYHANDLE(void);
	// 设备信息
	NET_DVR_CLIENTINFO ClientInfo;
	// 设备信息
	NET_DVR_CLIENTINFO ClientInfo1;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
