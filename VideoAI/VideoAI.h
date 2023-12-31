﻿
// VideoAI.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CVideoAIApp:
// 有关此类的实现，请参阅 VideoAI.cpp
//

class CVideoAIApp : public CWinApp
{
public:
	CVideoAIApp();

// 重写
public:
	virtual BOOL InitInstance();
	BOOL IsInstalled();
	BOOL GetLimitTime(CTime& t);  //t用来存储从注册表中读到的时间值
	BOOL SetLimitTime(DWORD nYear, DWORD nMonth, DWORD nDay,
		DWORD nHour, DWORD nMinute, DWORD nSecond);


// 实现

	DECLARE_MESSAGE_MAP()
};

extern CVideoAIApp theApp;
