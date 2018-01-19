
// hkvscap1Dlg.h : ͷ�ļ�
//

#pragma once
#include "highgui.h"  
#include "cv.h"
#include "CvvImage.h"
#include "ptz.h"
#include "HCNetSDK.h"



// Chkvscap1Dlg �Ի���
class Chkvscap1Dlg : public CDialogEx
{
// ����
public:
	Chkvscap1Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HKVSCAP1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton2();
	// �豸id
	long lUserID;
	long lUserID1;
	// ���ž��
	long m_lPlayHandle;
	long m_lPlayHandle1;


	// ������ͼƬ
	IplImage* image1;
	// ��ʾͼ��picture�ؼ�
	void ShowImage(IplImage * img, UINT ID);
	// �Ի����ȫ�ֱ��������ڴ���һЩȫ�ֶ���Ҫ������
//	Chkvscap1Dlg *globaldlg;
	// ������ת��ť
	PTZ btnptz_left;
	LONG GETPLAYHANDLE(void);
	// �豸��Ϣ
	NET_DVR_CLIENTINFO ClientInfo;
	// �豸��Ϣ
	NET_DVR_CLIENTINFO ClientInfo1;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
