
// VideoAIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VideoAI.h"
#include "VideoAIDlg.h"
#include "afxdialogex.h"


#define HAVE_STRUCT_TIMESPEC


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVideoAIDlg 对话框

CVideoAIDlg::CVideoAIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEOAI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_flipH = false;
	m_isScreenShot = false;
	m_yolov5_fire = false;
	m_thresh = 0.25;
	hasfire = false;

	//初始化yolov5
	detector.init();
}
CVideoAIDlg::~CVideoAIDlg()
{
	delete pThreadProcess;
	delete pThreadPlayVideo;
	delete pThreadPlayWecam;
	delete pThreadPlayRtsp;
}
//------------------这两个涉及CvvImage版本太旧-------------------
//新版opencv的Mat格式显示函数--CvvImage--老旧
void CVideoAIDlg::DrawMat2HDC(cv::Mat img, UINT ID)
{
	//CDC* pDC = GetDlgItem(ID)->GetDC();
	//HDC hDC = pDC->GetSafeHdc();
	//CRect rect;
	//GetDlgItem(ID)->GetClientRect(&rect);//根据控件ID--得到控件句柄--得到控件的相关参数(绘图设备，区域大小等)
	//CvvImage* pCImg = new CvvImage();
	//IplImage ipm = img; //Mat-->IplImage
	//pCImg->CopyOf(&ipm);//从IplImage-->CvvImage
	//pCImg->DrawToHDC_SCALE(hDC, &rect);
	//ReleaseDC(pDC);//释放绘图设备
	//delete pCImg;
}
//旧版opencv的IplImage格式显示函数--CvvImage--老旧
void CVideoAIDlg::DrawPic2HDC(IplImage* img, UINT ID)
{
	/*CDC* pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage* pCImg = new CvvImage();
	pCImg->CopyOf(img);
	pCImg->DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);
	delete pCImg;*/
}
//--------------------------------------------------------------
//前面两个使用老旧接口,已弃用
void CVideoAIDlg::DrawMat(cv::Mat& img, UINT ID, bool bRoom2ControlSize)
{
	cv::Mat tempImg;
	//cv::flip(img, tempImg, 0);//0--垂直;1--水平
	tempImg = img;
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);//获取控件大小
	//if (bRoom2ControlSize)
	//{
	//	cv::resize(img, tempImg, cv::Size(rect.Width(), rect.Height()));//缩小或者放大Mat，并备份
	//}
	//else
	//{
	//	img.copyTo(tempImg);
	//}
	
	switch (tempImg.channels())
	{
	case 1:
		cv::cvtColor(tempImg, tempImg, cv::COLOR_GRAY2BGR);//单通道
		break;
	case 3:
		cv::cvtColor(tempImg, tempImg, cv::COLOR_RGB2RGBA);//BGR3通道映射
		break;
	default:
		break;
	}
	
	int pixelBytes = tempImg.channels() * (tempImg.depth() + 1);//计算一个像素占用多少个字节

	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = tempImg.cols;
	bitInfo.bmiHeader.biHeight = tempImg.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	//Mat.data-->bitmap数据头-->MFC
	CDC* pDC = GetDlgItem(ID)->GetDC();
	pDC->SetStretchBltMode(COLORONCOLOR);
	if (bRoom2ControlSize)
	{
		::StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, 
						tempImg.cols, tempImg.rows, tempImg.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);

	}
	else
	{
		int minWidth = tempImg.cols > rect.Width() ? rect.Width() : tempImg.cols;
		int minHeight = tempImg.rows > rect.Height() ? rect.Height() : tempImg.rows;
		::StretchDIBits(pDC->GetSafeHdc(), 0, 0, minWidth, minHeight, 0, 0, 
						tempImg.cols, tempImg.rows, tempImg.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	ReleaseDC(pDC);
}
//跟前一个没区别,主要想解决图像垂直镜像的问题
void CVideoAIDlg::DrawMat2(cv::Mat& img, UINT ID, bool bRoom2ControlSize)
{
	cv::Mat tempImg;
	//cv::flip(img, tempImg, 0);//0--垂直;1--水平
	tempImg = img;
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);//获取控件大小
	//if (bRoom2ControlSize)
	//{
	//	cv::resize(img, tempImg, cv::Size(rect.Width(), rect.Height()));//缩小或者放大Mat，并备份
	//}
	//else
	//{
	//	img.copyTo(tempImg);
	//}

	//switch (tempImg.channels())
	//{
	//case 1:
	//	cv::cvtColor(tempImg, tempImg, cv::COLOR_GRAY2BGR);//单通道
	//	break;
	//case 3:
	//	cv::cvtColor(tempImg, tempImg, cv::COLOR_RGB2RGBA);//BGR3通道映射
	//	break;
	//default:
	//	break;
	//}

	int pixelBytes = tempImg.channels() * (tempImg.depth() + 1);//计算一个像素占用多少个字节

	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = tempImg.cols;
	bitInfo.bmiHeader.biHeight = tempImg.rows;
	
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	bitInfo.bmiHeader.biPlanes = 1;
	//Mat.data-->bitmap数据头-->MFC
	CDC* pDC = GetDlgItem(ID)->GetDC();
	pDC->SetStretchBltMode(COLORONCOLOR);
	if (bRoom2ControlSize)
	{
		::StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0,
			tempImg.cols, tempImg.rows, tempImg.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);

	}
	else
	{
		::StretchDIBits(pDC->GetSafeHdc(),rect.left, rect.top, rect.Width(), rect.Height(), 
			0, 0, tempImg.cols, tempImg.rows, tempImg.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);

		/*int minWidth = tempImg.cols > rect.Width() ? rect.Width() : tempImg.cols;
		int minHeight = tempImg.rows > rect.Height() ? rect.Height() : tempImg.rows;
		::StretchDIBits(pDC->GetSafeHdc(), 0, 0, minWidth, minHeight, 0, 0,
			tempImg.cols, tempImg.rows, tempImg.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);*/
	}
	ReleaseDC(pDC);
		
	
}


void CVideoAIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RTSP_URL, m_inputURL);
	DDX_Control(pDX, IDC_COMBO_MODEL, m_listModel);
}


BEGIN_MESSAGE_MAP(CVideoAIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CVideoAIDlg::OnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVideoAIDlg::OnClickedBtnStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_IMG, &CVideoAIDlg::OnClickedBtnImg)
	ON_BN_CLICKED(IDC_BTN_CAMERA, &CVideoAIDlg::OnClickedBtnCamera)
	ON_BN_CLICKED(IDC_BTN_VIDEO, &CVideoAIDlg::OnClickedBtnVideo)
	ON_BN_CLICKED(IDC_BTN_FLIPH, &CVideoAIDlg::OnClickedBtnFliph)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CVideoAIDlg::OnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_RTSP, &CVideoAIDlg::OnClickedBtnRtsp)
	
	ON_BN_CLICKED(IDC_BTN_SCREENSHOT, &CVideoAIDlg::OnClickedBtnScreenshot)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CVideoAIDlg::OnClickedBtnClose)
END_MESSAGE_MAP()

//播放视频流线程--执行函数
UINT Thread_Play(LPVOID lpParam)
{
	CVideoAIDlg* dlg = (CVideoAIDlg*)lpParam;
	char matParam[10];
	//int fps = 1;
	//int index = 1;

	//视频参数
	_itoa(dlg->m_Mat1.cols, matParam, 10);
	dlg->GetDlgItem(IDC_VIDEO_WIDTH)->SetWindowText(matParam);
	_itoa(dlg->m_Mat1.rows, matParam, 10);
	dlg->GetDlgItem(IDC_VIDEO_HEIGHT)->SetWindowText(matParam);
	dlg->GetDlgItem(IDC_VIDEO_FPS)->SetWindowText("25");
	//dlg->DrawMat2HDC(dlg->m_Mat1, IDC_DISPLAY);
	dlg->DrawMat2(dlg->m_Mat1, IDC_DISPLAY, false);

	//std::vector<cv::Mat>vecFrame;
	//std::vector<std::vector<yolov5::Detection>> detectionBatch;
	//while (1)
	//{
	//	if (m_ExitThread)
	//	{
	//		break;
	//	}
	//	if (fps % 25 != 0)
	//	{
	//		fps++;
	//		continue;
	//	}
	//	dlg->capture >> dlg->m_Mat1;//主线程中的
	//	
	//	//视频参数
	//	_itoa(dlg->m_Mat1.cols, matParam, 10);
	//	dlg->GetDlgItem(IDC_VIDEO_WIDTH)->SetWindowText(matParam);
	//	_itoa(dlg->m_Mat1.rows, matParam, 10);
	//	dlg->GetDlgItem(IDC_VIDEO_HEIGHT)->SetWindowText(matParam);
	//	dlg->GetDlgItem(IDC_VIDEO_FPS)->SetWindowText("25");
	//	//dlg->DrawMat2HDC(dlg->m_Mat1, IDC_DISPLAY);
	//	dlg->DrawMat2(dlg->m_Mat1, IDC_DISPLAY, false);
	//	//视频水平翻转
	//	if (dlg->m_flipH)
	//	{
	//		dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
	//		cv::flip(dlg->m_Mat_Process, dlg->m_Mat_Process, 1);//0--垂直;1--水平
	//		dlg->DrawMat(dlg->m_Mat_Process, IDC_RESULT, true);
	//	}
	//	//火焰识别
	//	if (dlg->m_yolov5_fire)
	//	{
	//		dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
	//		//ReadRtspBatch(dlg->detector,dlg->m_Mat_Process,dlg->m_thresh,dlg->hasfire,index);

	//		for (int i = 0; i < 3; i++)//行--高
	//		{
	//			for (int j = 0; j < 3; j++)//列--宽
	//			{
	//				cv::Rect rect(j * 640, i * 360, 640, 360);
	//				vecFrame.emplace_back(dlg->m_Mat_Process(rect).clone());
	//			}
	//		}
	//		dlg->detector.detectBatch(vecFrame, &detectionBatch, 0);
	//		for (int i = 0; i < detectionBatch.size(); i++)
	//		{
	//			for (int j = 0; j < detectionBatch[i].size(); j++)
	//			{
	//				if (detectionBatch[i][j].classId() == 0 && detectionBatch[i][j].score() > dlg->m_thresh)
	//				{
	//					dlg->hasfire = true;
	//					cv::imwrite("source/" + std::to_string(index) + ".jpg", vecFrame[i]);
	//					cv::rectangle(vecFrame[i], detectionBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
	//					//得分和类别暂时不画
	//					//保存
	//					if (dlg->hasfire)
	//					{
	//						cv::imwrite("save/" + std::to_string(index) + ".jpg", vecFrame[i]);
	//						index++;
	//					}
	//					//绘图
	//					//dlg->DrawMat2HDC(vecFrame[i], IDC_RESULT);
	//					dlg->DrawMat(vecFrame[i], IDC_RESULT, false);
	//				}
	//			}
	//		}
	//	}
	//	dlg->hasfire = false;
	//	vecFrame.clear();
	//	//m_Mat1.release();//先释放m_Mat1
	//	//m_Mat1 = m_Mat_Process.clone();
	//	
	//	fps = fps % 25 + 1;
	//}
	
	return 0;
}

//线程处理函数--推理所有类型，图像、本地视频、本地摄像头、rtsp流
UINT Thread_Process(LPVOID lpParam)
{
	CVideoAIDlg* dlg = (CVideoAIDlg*)lpParam;
	char matParam[10];
	int index = 1;
	std::vector<cv::Mat>vecFrame;//存放输入vector
	std::vector<std::vector<yolov5::Detection>> detectionBatch;//检测结果vector
	//处理线程一次就要遍历所有操作(对应功能点)
	while (1)
	{
		//水平翻转
		if (dlg->m_flipH)
		{
			dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
			cv::flip(dlg->m_Mat_Process, dlg->m_Mat_Process, 1);//0--垂直;1--水平
			dlg->DrawMat(dlg->m_Mat_Process, IDC_RESULT, true);
		}
		int curSel = dlg->m_listModel.GetCurSel();
		switch (curSel)
		{
		case 0://火焰检测
		{
			if (m_ExitThread)
			{
				break;
			}
			
			//火焰识别
			if (dlg->m_yolov5_fire)
			{
				yolov5::Result ret = dlg->detector.loadEngine("yolov5-fire-727.engine");
				if (ret)
				{
					AfxMessageBox("加载火焰模型失败，请确认后重试！");
					break;
				}
				
				dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
				//ReadRtspBatch(dlg->detector,dlg->m_Mat_Process,dlg->m_thresh,dlg->hasfire,index);

				for (int i = 0; i < 3; i++)//行--高
				{
					for (int j = 0; j < 3; j++)//列--宽
					{
						cv::Rect rect(j * 640, i * 360, 640, 360);
						vecFrame.emplace_back(dlg->m_Mat_Process(rect).clone());
					}
				}
				dlg->detector.detectBatch(vecFrame, &detectionBatch, 0);
				for (int i = 0; i < detectionBatch.size(); i++)
				{
					for (int j = 0; j < detectionBatch[i].size(); j++)
					{
						if (detectionBatch[i][j].classId() == 0 && detectionBatch[i][j].score() > dlg->m_thresh)
						{
							dlg->hasfire = true;
							cv::imwrite("source/" + std::to_string(index) + ".jpg", vecFrame[i]);
							cv::rectangle(vecFrame[i], detectionBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
							//得分和类别暂时不画
							//保存
							if (dlg->hasfire)
							{
								cv::imwrite("save/" + std::to_string(index) + ".jpg", vecFrame[i]);
								index++;
							}
							//绘图
							//dlg->DrawMat2HDC(vecFrame[i], IDC_RESULT);
							dlg->DrawMat(vecFrame[i], IDC_RESULT, true);
						}
					}
				}
			}	
			dlg->hasfire = false;
			dlg->m_yolov5_fire = false;
			vecFrame.clear();//清空当次的输入vector
			detectionBatch.clear();//清空档次的检测结果
		}
		break;
		case 1://安全帽识别
		{

		}
		break;
		case 2://行人检测
		{

		}
		break;
		case 3://烟雾检测
		{

		}
		break;
		case 4://行人计数
		{

		}
		break;
		default:
			dlg->detector.loadEngine("yolov5-fire-727.engine");//这里应该由选择的算法模型来加载对应的engine--目前只有一种就默认
			while (1)
			{
				if (m_ExitThread)
				{
					break;//停止推理
				}
			}
			break;
		}
	}
	return 0;
}

//播放本地摄像头线程--执行函数
UINT Thread_Play_Video(LPVOID lpParam)
{
	CVideoAIDlg* dlg = (CVideoAIDlg*)lpParam;
	char matParam[10];
	int fps = 1;
	int index = 1;

	std::vector<cv::Mat>vecFrame;
	std::vector<std::vector<yolov5::Detection>> detectionBatch;
	while (1)
	{
		if (m_ExitThread)
		{
			break;
		}
		if (fps % 25 != 0)
		{
			fps++;
			continue;
		}
		dlg->capture >> dlg->m_Mat1;//主线程中的

		//视频参数
		_itoa(dlg->m_Mat1.cols, matParam, 10);
		dlg->GetDlgItem(IDC_VIDEO_WIDTH)->SetWindowText(matParam);
		_itoa(dlg->m_Mat1.rows, matParam, 10);
		dlg->GetDlgItem(IDC_VIDEO_HEIGHT)->SetWindowText(matParam);
		dlg->GetDlgItem(IDC_VIDEO_FPS)->SetWindowText("25");
		//dlg->DrawMat2HDC(dlg->m_Mat1, IDC_DISPLAY);
		dlg->DrawMat(dlg->m_Mat1, IDC_DISPLAY, true);
		//视频水平翻转
		if (dlg->m_flipH)
		{
			dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
			cv::flip(dlg->m_Mat_Process, dlg->m_Mat_Process, 1);//0--垂直;1--水平
			dlg->DrawMat(dlg->m_Mat_Process, IDC_RESULT, true);
		}
		//火焰识别
		if (dlg->m_yolov5_fire)
		{
			dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
			//ReadRtspBatch(dlg->detector,dlg->m_Mat_Process,dlg->m_thresh,dlg->hasfire,index);

			for (int i = 0; i < 3; i++)//行--高
			{
				for (int j = 0; j < 3; j++)//列--宽
				{
					cv::Rect rect(j * 640, i * 360, 640, 360);
					vecFrame.emplace_back(dlg->m_Mat_Process(rect).clone());
				}
			}
			dlg->detector.detectBatch(vecFrame, &detectionBatch, 0);
			for (int i = 0; i < detectionBatch.size(); i++)
			{
				for (int j = 0; j < detectionBatch[i].size(); j++)
				{
					if (detectionBatch[i][j].classId() == 0 && detectionBatch[i][j].score() > dlg->m_thresh)
					{
						dlg->hasfire = true;
						cv::imwrite("source/" + std::to_string(index) + ".jpg", vecFrame[i]);
						cv::rectangle(vecFrame[i], detectionBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
						cv::putText(vecFrame[i], "fire", cv::Point(detectionBatch[i][j].boundingBox().x, detectionBatch[i][j].boundingBox().y), 1, 1.0, cv::Scalar(0, 0, 255), 1);
						//得分和类别暂时不画
						//保存
						if (dlg->hasfire)
						{
							cv::imwrite("save/" + std::to_string(index) + ".jpg", vecFrame[i]);
							index++;
						}
						//绘图
						//dlg->DrawMat2HDC(vecFrame[i], IDC_RESULT);
						dlg->DrawMat(vecFrame[i], IDC_RESULT, true);
					}
				}
			}
		}
		dlg->hasfire = false;
		vecFrame.clear();
		//m_Mat1.release();//先释放m_Mat1
		//m_Mat1 = m_Mat_Process.clone();

		fps = fps % 25 + 1;
	}

	return 0;

}

//播放本地图像线程--执行函数
UINT Thread_Play_Image(LPVOID lpParam)
{
	CVideoAIDlg* dlg = (CVideoAIDlg*)lpParam;
	char matParam[10];
	int index = 1;

	std::vector<cv::Mat>vecFrame;
	std::vector<std::vector<yolov5::Detection>> detectionBatch;
	
	if (m_ExitThread)
	{
		return 0;
	}
	//dlg->capture >> dlg->m_Mat1;//主线程中的

	//图像参数
	_itoa(dlg->m_Mat1.cols, matParam, 10);
	dlg->GetDlgItem(IDC_VIDEO_WIDTH)->SetWindowText(matParam);
	_itoa(dlg->m_Mat1.rows, matParam, 10);
	dlg->GetDlgItem(IDC_VIDEO_HEIGHT)->SetWindowText(matParam);
	dlg->GetDlgItem(IDC_VIDEO_FPS)->SetWindowText("当前推理本地图像...");
	//dlg->DrawMat2HDC(dlg->m_Mat1, IDC_DISPLAY);
	dlg->DrawMat(dlg->m_Mat1, IDC_DISPLAY, true);
	//视频水平翻转
	if (dlg->m_flipH)
	{
		dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
		cv::flip(dlg->m_Mat_Process, dlg->m_Mat_Process, 1);//0--垂直;1--水平
		dlg->DrawMat(dlg->m_Mat_Process, IDC_RESULT, true);
	}
	//火焰识别
	if (dlg->m_yolov5_fire)
	{
		dlg->m_Mat_Process = dlg->m_Mat1.clone();//深拷贝
		//ReadRtspBatch(dlg->detector,dlg->m_Mat_Process,dlg->m_thresh,dlg->hasfire,index);

		for (int i = 0; i < 3; i++)//行--高
		{
			for (int j = 0; j < 3; j++)//列--宽
			{
				cv::Rect rect(j * 640, i * 360, 640, 360);
				vecFrame.emplace_back(dlg->m_Mat_Process(rect).clone());
			}
		}
		dlg->detector.detectBatch(vecFrame, &detectionBatch, 0);
		for (int i = 0; i < detectionBatch.size(); i++)
		{
			for (int j = 0; j < detectionBatch[i].size(); j++)
			{
				if (detectionBatch[i][j].classId() == 0 && detectionBatch[i][j].score() > dlg->m_thresh)
				{
					dlg->hasfire = true;
					cv::imwrite("source/" + std::to_string(index) + ".jpg", vecFrame[i]);
					cv::rectangle(vecFrame[i], detectionBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
					//得分和类别暂时不画
					//保存
					if (dlg->hasfire)
					{
						cv::imwrite("save/" + std::to_string(index) + ".jpg", vecFrame[i]);
						index++;
					}
					//绘图
					//dlg->DrawMat2HDC(vecFrame[i], IDC_RESULT);
					dlg->DrawMat(vecFrame[i], IDC_RESULT, true);
				}
			}
		}
	}
	dlg->hasfire = false;
	dlg->m_yolov5_fire = false;
	vecFrame.clear();
	//m_Mat1.release();//先释放m_Mat1
	//m_Mat1 = m_Mat_Process.clone();

	return 0;
}

//推理RTSP视频流batch-size=9(一个张图切分为9张)
int ReadRtspBatch(yolov5::Detector& detector,cv::Mat& img,float thresh,bool& hasfire,int& index)
{
	std::vector<cv::Mat>vecFrame;
	std::vector<std::vector<yolov5::Detection>> detectionBatch;

	for (int i = 0; i < 3; i++)//行--高
	{
		for (int j = 0; j < 3; j++)//列--宽
		{
			cv::Rect rect(j * 640, i * 360, 640, 360);
			vecFrame.emplace_back(img(rect).clone());
		}
	}
	detector.detectBatch(vecFrame, &detectionBatch, 0);
	for (int i = 0; i < detectionBatch.size(); i++)
	{
		for (int j = 0; j < detectionBatch[i].size(); j++)
		{
			if (detectionBatch[i][j].classId() == 0 && detectionBatch[i][j].score() > thresh)
			{
				hasfire = true;
				cv::imwrite("source/" + std::to_string(index) + ".jpg", vecFrame[i]);
				cv::rectangle(vecFrame[i], detectionBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
				//得分和类别暂时不画
				//保存
				if (hasfire)
				{
					cv::imwrite("save/" + std::to_string(index) + ".jpg", vecFrame[i]);
					index++;
				}
			}
		}
	}
	
}

// CVideoAIDlg 消息处理程序

BOOL CVideoAIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//设置主界面初始信息
	m_inputURL.SetWindowText("rtsp://");
	m_inputURL.SetFocus();
	
	m_listModel.SetCurSel(0);

	//启动推理线程
	pThreadProcess = AfxBeginThread(Thread_Process, this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVideoAIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoAIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//显示图像--这里是刷新窗口的地方
		if (m_Mat1.data)
		{
			DrawMat2HDC(m_Mat1, IDC_DISPLAY);
		}
		if (m_Mat_Process.data)
		{
			DrawMat2HDC(m_Mat_Process, IDC_RESULT);
		}
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoAIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//暂停播放
void CVideoAIDlg::OnClickedBtnStop()
{
	KillTimer(1);//停止定时器
	//capture.release();//关闭视频流
}

void CVideoAIDlg::OnTimer(UINT_PTR nIDEvent)
{
	capture >> m_Mat1;
	m_Mat_Process = m_Mat1.clone();//深拷贝
	DrawMat(m_Mat1, IDC_DISPLAY, true);
	//视频处理代码
	if (m_flipH)
	{
		cv::flip(m_Mat_Process, m_Mat_Process, 1);//0--垂直;1--水平
		DrawMat(m_Mat_Process, IDC_RESULT, true);
	}
	if (m_isScreenShot)
	{
		m_Mat_Process = m_Mat1.clone();//深拷贝
		cv::imwrite("E:\\Source\\MFC\\VideoAI\\screenshot\\screenshot.jpg", m_Mat_Process);
		DrawMat(m_Mat_Process, IDC_RESULT, true);
		m_isScreenShot = !m_isScreenShot;
	}
	//m_Mat1.release();//先释放m_Mat1
	//m_Mat1 = m_Mat_Process.clone();
	/*DrawMat2HDC(m_Mat1, IDC_DISPLAY);
	DrawMat2HDC(m_Mat_Process, IDC_RESULT);*/
	
	
	CDialogEx::OnTimer(nIDEvent);
}

//选择本地图像
void CVideoAIDlg::OnClickedBtnImg()
{
	//打开图像前先关闭视像头、网络流
	if (capture.isOpened())
	{
		capture.release();
	}
	
	CFileDialog* pFileDlg = new CFileDialog(TRUE, "*.jpg", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, NULL, NULL);
	pFileDlg->m_ofn.lpstrTitle = "选择本地图像文件...";
	pFileDlg->m_ofn.lpstrFilter = "image files (*.jpg) \0*.jpg\0image files (*.bmp)\0*.bmp\0All files (*.*) \0*.*";
	if (pFileDlg->DoModal() != IDOK)
	{
		return;//放弃选择图像
	}
	CString path = pFileDlg->GetPathName();//获取图像完整路径
	m_Mat1 = cv::imread(LPCSTR(path), 1);  //opencv读取后是[B,G,R]
	if (!m_Mat1.data)
	{
		MessageBox("读取失败");//读取失败
		return;
	}
	//DrawMat2HDC(m_Mat1,IDC_DISPLAY);//把图像画到控件上(已弃用)
	DrawMat(m_Mat1, IDC_DISPLAY, true);
	//显示图像参数
	char matParam[10];
	_itoa(m_Mat1.cols, matParam, 10);//列--宽
	SetDlgItemText(IDC_VIDEO_WIDTH, matParam);
	_itoa(m_Mat1.rows, matParam, 10);//行--高
	SetDlgItemText(IDC_VIDEO_HEIGHT, matParam);
	SetDlgItemText(IDC_VIDEO_FPS, "0");//图像没有帧率
	
	delete pFileDlg;
}


void CVideoAIDlg::OnClickedBtnCamera()
{
	//先关闭
	if (capture.isOpened())
	{
		capture.release();
	}
	if (!capture.open(0))
	{
		MessageBox("打开本地摄像头失败，请检查后重试。");
	}
	else
	{
		//capture >> m_Mat1;
		//char matParam[10];
		//_itoa(m_Mat1.cols, matParam, 10);
		//SetDlgItemText(IDC_VIDEO_WIDTH, matParam);
		//_itoa(m_Mat1.rows, matParam, 10);
		//SetDlgItemText(IDC_VIDEO_HEIGHT, matParam);
		//SetDlgItemText(IDC_VIDEO_FPS, LPCTSTR("25"));
		////刷新显示区
		//Invalidate(TRUE);
		//设置定时器--不用定时器--用多线程
		//SetTimer(1, 40, NULL);//40ms--对应25帧/s

		//pThreadPlayWecam = AfxBeginThread(Thread_Play_Video, this);
		//播放本地视频
	}
}


void CVideoAIDlg::OnClickedBtnVideo()
{
	CFileDialog* pFileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, NULL, NULL);
	pFileDlg->m_ofn.lpstrTitle = "打开本地视频...";
	pFileDlg->m_ofn.lpstrFilter = "*.AVI\0*.AVI\0ALL Files (*.*)\0*.*";
	if (pFileDlg->DoModal() != IDOK)
	{
		return;
	}
	CString path = pFileDlg->GetPathName();
	if (capture.isOpened())
	{
		capture.release();//每次读取前都要先判断摄像头是否打开--打开要释放
	}
	if (!capture.open(LPCSTR(path)))
	{
		MessageBox("打开本地视频出错，请检查后再试。");
	}
	else
	{
		//播放视频
	}
	delete pFileDlg;
}


void CVideoAIDlg::OnClickedBtnFliph()
{
	m_flipH = !m_flipH;
}


void CVideoAIDlg::OnClickedBtnExit()
{
	CDialogEx::OnOK();
}


void CVideoAIDlg::OnClickedBtnRtsp()
{
	CString rtsp_url = "";
	m_inputURL.GetWindowText(rtsp_url);
	if (rtsp_url == "rtsp://")
	{
		AfxMessageBox("请先输入rtsp地址!");
		return;
	}
	else
	{
		std::string str_url(rtsp_url.GetString());
		capture.open(str_url);
		//rtsp用多线程播放--不要用定时器
		//pThreadPlayRtsp = AfxBeginThread(Thread_Play, this);
		//播放rtsp视频
	}
}


void CVideoAIDlg::OnClickedBtnStart()
{
	detector.init();
	//yolov5::Result ret = detector.loadEngine("yolov5s.engine");
	yolov5::Result ret = detector.loadEngine("yolov5-fire-727.engine");
	if (ret != 0)
	{
		AfxMessageBox("加载engine模型失败，请检查后重试！");
		return;
	}
	m_yolov5_fire = !m_yolov5_fire;
}


void CVideoAIDlg::OnClickedBtnScreenshot()
{
	m_isScreenShot = !m_isScreenShot;
}


void CVideoAIDlg::OnClickedBtnClose()
{
	m_ExitThread = true;
	if (capture.isOpened())
	{
		capture.release();
	}
	KillTimer(1);
	//清楚控件区域
	CRect rc1, rc2;
	GetDlgItem(IDC_DISPLAY)->GetClientRect(&rc1);
	//RedrawWindow(CRect(0, 0, rc1.Width(), rc1.Height()));
	RedrawWindow(CRect(0, 0, 1280, 720));
	GetDlgItem(IDC_RESULT)->GetClientRect(&rc2);
	RedrawWindow(CRect(0, 0, rc2.Width(), rc2.Height()));
}
