
// hkvscap1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "hkvscap1.h"
#include "hkvscap1Dlg.h"
#include "afxdialogex.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include "CvvImage.h"
#include "highgui.h"  
#include "cv.h"  
#include "PTZ.h"
#include <cstring>  
#include <opencv2/opencv.hpp>
#include "JMPframe.h"
#include "myGMM.h"
using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
 
#endif
#define USECOLOR 1 

HWND hWnd=NULL; 
IplImage* pImg = cvCreateImage(cvSize(1280,720), 8, 3);
IplImage* pImg1 = cvCreateImage(cvSize(1280,720), 8, 3);
BOOL bRet1=0;
NET_DVR_PTZPOS ptz;
NET_DVR_PTZPOS *pptz=&ptz;
NET_DVR_PTZSCOPE fanwei;
NET_DVR_PTZSCOPE *pfanwei=&fanwei;
DWORD aaaa=100;
LPDWORD A=&aaaa;
DWORD S=1;
Mat frame,mask,thresholdImage, output;
BackgroundSubtractorMOG2 bgSubtractor(20,16,true);  
IplImage* maskIplImage=cvCreateImage(cvSize(1280,720), 8, 3);
runsys impGMM;




int ChangeDegree(float degree)//������ת���ɶ�Ӧ��ʮ������ת����ʮ����
{
	int a,wan,qian,bai,shi,ge,result;
	a=degree;
	qian=a/1000;
	bai=(a-qian*1000)/100;
	shi=(a-qian*1000-bai*100)/10;
	ge=(a-qian*1000-bai*100-shi*10);
	result=qian*4096+bai*16*16+shi*16+ge;
	return result;
}
int GetDegree(int ddegree)//��ȡʮ���ƶ�Ӧ��16���ƶ�Ӧ��
{
	int a,b,c,d,e,f;
	a=ddegree/4096;
	b=(ddegree-a*4096)/256;
	c=(ddegree-a*4096-b*256)/16;
	d=ddegree-a*4096-b*256-c*16;
	e=a*1000+b*100+c*10+d;
	return e;
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
void yv12toYUV(char *outYuv, char *inYv12, int width, int height,int widthStep)  
{  
   int col,row;  
   unsigned int Y,U,V;  
   int tmp;  
   int idx;  
  
  //printf("widthStep=%d.\n",widthStep);  
  
   for (row=0; row<height; row++)  
   {  
      idx=row * widthStep;  
      int rowptr=row*width;  
  
      for (col=0; col<width; col++)  
      {  
         //int colhalf=col>>1;  
         tmp = (row/2)*(width/2)+(col/2);  
//         if((row==1)&&( col>=1400 &&col<=1600))  
//         {   
//          printf("col=%d,row=%d,width=%d,tmp=%d.\n",col,row,width,tmp);  
//          printf("row*width+col=%d,width*height+width*height/4+tmp=%d,width*height+tmp=%d.\n",row*width+col,width*height+width*height/4+tmp,width*height+tmp);  
//         }   
         Y=(unsigned int) inYv12[row*width+col];  
         U=(unsigned int) inYv12[width*height+width*height/4+tmp];  
         V=(unsigned int) inYv12[width*height+tmp];  
//         if ((col==200))  
//         {   
//         printf("col=%d,row=%d,width=%d,tmp=%d.\n",col,row,width,tmp);  
//         printf("width*height+width*height/4+tmp=%d.\n",width*height+width*height/4+tmp);  
//         return ;  
//         }  
         if((idx+col*3+2)> (1200 * widthStep))  
         {  
          //printf("row * widthStep=%d,idx+col*3+2=%d.\n",1200 * widthStep,idx+col*3+2);  
         }   
         outYuv[idx+col*3]   = Y;  
         outYuv[idx+col*3+1] = U;  
         outYuv[idx+col*3+2] = V;  
      }  
   }  
   //printf("col=%d,row=%d.\n",col,row);  
}  
  
  
  
//����ص� ��ƵΪYUV����(YV12)����ƵΪPCM����  
void CALLBACK DecCBFun(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2)  
{  
    long lFrameType = pFrameInfo->nType;   
  
    if(lFrameType ==T_YV12)  
    {  
#if USECOLOR  
    //int start = clock();  
    IplImage* pImgYCrCb = cvCreateImage(cvSize(pFrameInfo->nWidth,pFrameInfo->nHeight), 8, 3);//�õ�ͼ���Y����    
    yv12toYUV(pImgYCrCb->imageData, pBuf, pFrameInfo->nWidth,pFrameInfo->nHeight,pImgYCrCb->widthStep);//�õ�ȫ��RGBͼ��  
	TRACE("nport:%d\n",nPort);
	if(nPort==0){
		cvCvtColor(pImgYCrCb,pImg,CV_YCrCb2RGB);
	}
	if(nPort==1){ 
		cvCvtColor(pImgYCrCb,pImg1,CV_YCrCb2RGB);	
	}
       
    //int end = clock();  
#else  
    IplImage* pImg = cvCreateImage(cvSize(pFrameInfo->nWidth,pFrameInfo->nHeight), 8, 1);    
    memcpy(pImg->imageData,pBuf,pFrameInfo->nWidth*pFrameInfo->nHeight);   
#endif  
    //printf("%d\n",end-start);  
    //cvShowImage("IPCamera",pImg);  
    //cvWaitKey(1);  
#if USECOLOR  
    cvReleaseImage(&pImgYCrCb);  
    //cvReleaseImage(&pImg);  
#else  
    cvReleaseImage(&pImg);  
#endif  
    //��ʱ��YV12��ʽ����Ƶ���ݣ�������pBuf�У�����fwrite(pBuf,nSize,1,Videofile);  
    //fwrite(pBuf,nSize,1,fp);  
    }  
    /*************** 
    else if (lFrameType ==T_AUDIO16) 
    { 
        //��ʱ����Ƶ���ݣ����ݱ�����pBuf�У�����fwrite(pBuf,nSize,1,Audiofile); 
         
    } 
    else 
    { 
 
    } 
    *******************/  
  
}  
  
  
///ʵʱ���ص�  
void CALLBACK fRealDataCallBack(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD dwBufSize,void *pUser)  
{  
    DWORD dRet;  
	LONG nPort=-1; 
	//nPort=lRealHandle;
    switch (dwDataType)  
    {  
    case NET_DVR_SYSHEAD:    //ϵͳͷ 

		
        if (!PlayM4_GetPort(&nPort)) //��ȡ���ſ�δʹ�õ�ͨ����  
        {  
            break;  
        }  

        if(dwBufSize > 0)  
        {  
            if (!PlayM4_OpenStream(nPort,pBuffer,dwBufSize,1024*1024))  
            {  
                dRet=PlayM4_GetLastError(nPort);  
                break;  
            }  
            //���ý���ص����� ֻ���벻��ʾ  

            if (!PlayM4_SetDecCallBack(nPort,DecCBFun))  
            {  
                dRet=PlayM4_GetLastError(nPort);  
                break;  
            }  

            //���ý���ص����� ��������ʾ  
            //if (!PlayM4_SetDecCallBackEx(nPort,DecCBFun,NULL,NULL))  
            //{  
            //  dRet=PlayM4_GetLastError(nPort);  
            //  break;  
            //}  
  
            //����Ƶ����  
            if (!PlayM4_Play(nPort,hWnd))  
            {  
                dRet=PlayM4_GetLastError(nPort);  
                break;  
            }  
  
            //����Ƶ����, ��Ҫ�����Ǹ�����  
/*            if (!PlayM4_PlaySound(nPort))  
            {  
                dRet=PlayM4_GetLastError(nPort);  
                break;  
            }   */      
        }  
        break;  
          
    case NET_DVR_STREAMDATA:   //��������  
		nPort=lRealHandle;
		TRACE("aaaaaaaanport:%d\n",nPort);
        if (dwBufSize > 0 && nPort != -1)  
        {  
            BOOL inData=PlayM4_InputData(nPort,pBuffer,dwBufSize);  
            while (!inData)  
            {  
                Sleep(10);  
                inData=PlayM4_InputData(nPort,pBuffer,dwBufSize);  
                OutputDebugString(L"PlayM4_InputData failed \n");     
            }  
        }  
        break;    
    }         
}  


  
//void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)  
//{  
//    char tempbuf[256] = {0};  
//    switch(dwType)   
//    {  
//    case EXCEPTION_RECONNECT:    //Ԥ��ʱ����  
//    printf("----------reconnect--------%d\n", time(NULL));  
//    break;  
//    default:  
//    break;  
//    }  
//} 

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// Chkvscap1Dlg �Ի���




Chkvscap1Dlg::Chkvscap1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Chkvscap1Dlg::IDD, pParent)
	, lUserID(0)
	, lUserID1(0)
	, m_lPlayHandle(-1)
	, m_lPlayHandle1(-1)
	, image1()
	//, globaldlg(NULL)
	//, m_lPlayHandle1(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Chkvscap1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Chkvscap1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Chkvscap1Dlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &Chkvscap1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &Chkvscap1Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &Chkvscap1Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &Chkvscap1Dlg::OnBnClickedButton6)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// Chkvscap1Dlg ��Ϣ�������

BOOL Chkvscap1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	btnptz_left.SubclassDlgItem(IDC_BUTTON3,this);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Chkvscap1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Chkvscap1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Chkvscap1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Chkvscap1Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	// ��ʼ��  
	NET_DVR_Init();  
	//��������ʱ��������ʱ��  
	NET_DVR_SetConnectTime(2000, 1);  


	//---------------------------------------  
	// ��ȡ����̨���ھ��  
	//HMODULE hKernel32 = GetModuleHandle((LPCWSTR)"kernel32");  
	//GetConsoleWindow = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32,"GetConsoleWindow");  

	//---------------------------------------  
	// ע���豸  
	
	//NET_DVR_DEVICEINFO_V30 struDeviceInfo;  	
	//lUserID = NET_DVR_Login_V30("192.168.1.100", 8000, "admin", "12345", &struDeviceInfo); 
	NET_DVR_DEVICEINFO_V30 struDeviceInfo1;
	lUserID1 = NET_DVR_Login_V30("192.168.1.107", 6000, "admin", "a12345678", &struDeviceInfo1);
/*	if (lUserID < 0||lUserID1<0)  
	{  
		printf("Login error, %d\n", NET_DVR_GetLastError());  
		NET_DVR_Cleanup();  
		 
	}*/  
	//ClientInfo.hPlayWnd     = GetDlgItem(IDC_STATIC1)->m_hWnd;
	ClientInfo.hPlayWnd = NULL;
	ClientInfo.lChannel     = 1;
	ClientInfo.lLinkMode    = 0;
	ClientInfo.sMultiCastIP = NULL;
	TRACE("Channel number:%d\n",ClientInfo.lChannel);
	LONG lRealPlayHandle; 
	ClientInfo1.hPlayWnd = NULL;
	ClientInfo1.lChannel     = 1;
	ClientInfo1.lLinkMode    = 0;
	ClientInfo1.sMultiCastIP = NULL;
	TRACE("Channel number:%d\n",ClientInfo1.lChannel);
	//LONG lRealPlayHandle; 
	//m_lPlayHandle = NET_DVR_RealPlay_V30(lUserID,&ClientInfo,fRealDataCallBack,NULL,TRUE);
	m_lPlayHandle1 = NET_DVR_RealPlay_V30(lUserID1,&ClientInfo1,fRealDataCallBack,NULL,TRUE); 
	//m_lPlayHandle1 = NET_DVR_RealPlay_V30(lUserID1,&ClientInfo1,fRealDataCallBack,NULL,TRUE); 

	//m_lPlayHandle = NET_DVR_RealPlay_V30(lUserID,&ClientInfo,NULL,NULL,TRUE);

}


void Chkvscap1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//nPort=0;
	/*
	//��ϸ�˹
	//frame=cvarrToMat(pImg);
	//bgSubtractor(frame,mask,0.001);
	//*maskIplImage=IplImage(mask);
	//ShowImage(maskIplImage,IDC_STATIC1 );
	*/
	frame=cvarrToMat(pImg);
	cvtColor(frame,frame,CV_RGB2GRAY);
	impGMM(frame,mask);
	*maskIplImage=IplImage(mask);
	ShowImage(maskIplImage,IDC_STATIC1);
	ShowImage( pImg,IDC_STATIC2 );
	
	CDialogEx::OnTimer(nIDEvent);
}


void Chkvscap1Dlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	image1=cvLoadImage("D:\\project\\hkvstest\\hkvscap1\\hkvscap1\\a01.jpg");
	ShowImage( image1,IDC_STATIC2 ); 
	NET_DVR_StopRealPlay(m_lPlayHandle);
	NET_DVR_Logout(lUserID);  
	NET_DVR_Cleanup();
	//image1=cvLoadImage("D:\\project\\hkvstest\\hkvscap1\\hkvscap1\\a01.jpg");
	ShowImage( image1,IDC_STATIC1 ); 
	NET_DVR_StopRealPlay(m_lPlayHandle1);
	NET_DVR_Logout(lUserID1);  
	NET_DVR_Cleanup();
	//cvReleaseImage(&image1);
	if(bRet1==1){
		bRet1=0;
		KillTimer(1);
	}
	
}


// ��ʾͼ��picture�ؼ�
void Chkvscap1Dlg::ShowImage(IplImage * img, UINT ID)
{
	IplImage *pDstImage = NULL; 
	CvSize czSize;              //Ŀ��ͼ��ߴ�
	
	CDC* pDC=GetDlgItem(ID)->GetDC();// �����ʾ�ؼ��� DC  
	HDC hDC=pDC->GetSafeHdc();// ��ȡ HDC(�豸���) �����л�ͼ����  

	CRect rect;  
	GetDlgItem(ID)->GetClientRect(&rect);  
	int rw=rect.right-rect.left;    // ���picture control�Ŀ�͸�  
	int rh=rect.bottom-rect.top; 
	czSize.width = rw;  
	czSize.height = rh;
	pDstImage = cvCreateImage(czSize, img->depth, img->nChannels);
	cvResize(img, pDstImage, CV_INTER_AREA);
	int iw=pDstImage->width;             // ��ȡͼƬ�Ŀ�͸�  
	int ih=pDstImage->height;  
	int tx = (int)(rw - iw)/2;                                       // ʹͼƬ����ʾλ�������ڿؼ�������  
	int ty = (int)(rh - ih)/2;  
	SetRect( rect, tx, ty, tx+iw, ty+ih );  

	CvvImage cimg;  
	cimg.CopyOf(pDstImage);    // ����ͼƬ  
	cimg.DrawToHDC(hDC,&rect);  

	ReleaseDC(pDC); 
}


// ��ȡ���ž��


LONG Chkvscap1Dlg::GETPLAYHANDLE()
{
	return m_lPlayHandle;
}


void Chkvscap1Dlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool aa=NET_DVR_CapturePicture(m_lPlayHandle1,"D:\\aa.jpg");
	//bool bb=NET_DVR_CapturePicture(m_lPlayHandle1,"D:\\bb.jpg");
	cvSaveImage("d:\\bb.jpg",pImg1);


}


void Chkvscap1Dlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
		if(-1 == m_lPlayHandle1)
	{
		DWORD err=NET_DVR_GetLastError();
		CString m_csErr;
		m_csErr.Format(_T("���ų����������%d"),err);
		MessageBox(m_csErr);
	}
		else if(m_lPlayHandle1>=0){
			if(bRet1==0){
			SetTimer(1,100,NULL);
			GetDlgItem(IDC_BUTTON5)->SetWindowText(_T("STOP"));
			bRet1=1;
			}
			else{
				KillTimer(1);
				bRet1=0;
				GetDlgItem(IDC_BUTTON5)->SetWindowText(_T("Ԥ��"));
			}
		}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
}


void Chkvscap1Dlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL ret;
	//CEdit *txt_box;
	//CString str;
	//txt_box=(CEdit*)GetDlgItem(IDC_EDIT2);
	//txt_box->GetWindowText(str);
	//WORD a;
	//strcpy(temp,str);
	//a = _ttoi(str);
	ret=NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,pptz,aaaa,A);
    ptz.wPanPos=0;
	ptz.wTiltPos=2304;
	//ptz.wZoomPos=0xFB;
	ret=NET_DVR_SetDVRConfig(lUserID,NET_DVR_SET_PTZPOS,1,pptz,aaaa);
}


void Chkvscap1Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	 CString str;
	 int a,b,e,f;
	 int posp,post;
	 int nowposp,nowpost,nextposp,nextpost;
	 float c,d;
	 BOOL retS;
	 retS=NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,pptz,aaaa,A);
	 posp=ptz.wPanPos;
	 post=ptz.wTiltPos;
    //ZeroMemory(&str, sizeof(CString));
    nowposp=GetDegree(posp);
	nowpost=GetDegree(post);
	a= point.x-12;
	b=point.y-12;
	c=(a*1280/444-640)*27.5/640;//��ȡ��Ե�ǰpλ�õ�ƫת�Ƕ�(���ڵļ��㷽ʽ�Ǹ���ͼ���Ĵ�С��ͼ����ӳ��)
	d=(b*720/426-360)*16/360;//��ȡ��Ե�ǰtλ�õ�ƫת�Ƕ�()
	nextposp=nowposp+c*10;
	nextpost=nowpost+d*10;
	e=ChangeDegree(nextposp);
	f=ChangeDegree(nextpost);
	ptz.wPanPos=e;
	ptz.wTiltPos=f;
	retS=NET_DVR_SetDVRConfig(lUserID,NET_DVR_SET_PTZPOS,1,pptz,aaaa);
	str.Format(_T("x=%d,y=%d,c=%f,d=%f,e=%d"), point.x-12, point.y-12,c,d,e);
    
    

    //AfxMessageBox(str);

	CDialogEx::OnLButtonDown(nFlags, point);
}
