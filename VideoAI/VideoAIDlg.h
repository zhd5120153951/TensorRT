
// VideoAIDlg.h: 头文件
//

#pragma once
//#include "opencv.hpp"
//#include "highgui.hpp"
//#include "video.hpp"
#include "cv.h"
#include "highgui.h"
//#include "CvvImage.h"

#include "yolov5_builder.hpp"
#include "yolov5_detector.hpp"
#include <ctime>
#include <chrono>
#include <algorithm>
#include <thread>
#include <Windows.h>

#include "pthread.h"
#pragma comment(lib,"pthreadVC2.lib")

static bool m_ExitThread = false;
// CVideoAIDlg 对话框
class CVideoAIDlg : public CDialogEx
{
// 构造
public:
	CVideoAIDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CVideoAIDlg();

	cv::Mat m_Mat1, m_Mat_Process, m_Mat_Display;
	cv::VideoCapture capture;//主界面全局摄像头捕捉类对象
	bool m_flipH;			 //是否水平翻转(还可以加垂直翻转)
	bool m_isScreenShot;	 //是否截图
	bool m_yolov5_fire;		 //是否火焰识别
	yolov5::Detector detector;//火焰识别检测器
	float m_thresh;			  //得分阈值
	bool hasfire;			  //是否有火

	void DrawMat2HDC(cv::Mat img, UINT ID);
	void DrawPic2HDC(IplImage* img, UINT ID);
	//画图用这个
	void DrawMat(cv::Mat& img, UINT ID, bool bRoom2ControlSize);
	//画图
	void DrawMat2(cv::Mat& img, UINT ID, bool bRoom2ControlSize);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEOAI_DIALOG };
#endif

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
	// rtsp地址
	CEdit m_inputURL;
	CWinThread* pThreadPlay;
	afx_msg void OnClickedBtnStop();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedBtnImg();
	afx_msg void OnClickedBtnCamera();
	afx_msg void OnClickedBtnVideo();
	afx_msg void OnClickedBtnFliph();
	afx_msg void OnClickedBtnExit();
	afx_msg void OnClickedBtnRtsp();
	afx_msg void OnClickedBtnStart();
	// 算法模型列表
	CComboBox m_listModel;
	afx_msg void OnClickedBtnScreenshot();
	afx_msg void OnClickedBtnClose();
};
