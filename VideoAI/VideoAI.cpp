
// VideoAI.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "VideoAI.h"
#include "VideoAIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoAIApp

BEGIN_MESSAGE_MAP(CVideoAIApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVideoAIApp 构造

CVideoAIApp::CVideoAIApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CVideoAIApp 对象

CVideoAIApp theApp;


// CVideoAIApp 初始化

BOOL CVideoAIApp::InitInstance()
{

	CTime CurTime = CTime::GetCurrentTime();//先获得当前的系统时间
	if (!IsInstalled())//检查软件是否第一次运行，如果是则设定限制时间
	{
		CTime t = CurTime + CTimeSpan(0, 0, 5, 0); //使用时限为5分钟  
		SetLimitTime(t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond());
	}
	else
	{
		//如果软件不是第一次运行，读取注册表中的时间值，与当前时间比较
		CTime LimitTime;
		GetLimitTime(LimitTime);
		if (CurTime > LimitTime)//超过了使用期限
		{
			AfxMessageBox("试用版已到期，请联系购买一个正版使用", MB_OK);
			return FALSE;//退出
		}
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("daito VideoAI Application"));

	//读取ini文件确认设备号
	int nCount = ::GetPrivateProfileInt("deviceID", "deviceName", 0, ".\\device.ini");
	if (!nCount)
	{
		CString strKeyName, strKeyValue;
		::GetPrivateProfileString("deviceName", strKeyName, NULL, strKeyName.GetBuffer(128), 128, ".\\device.ini");

		AfxMessageBox(strKeyName);
	}

	CVideoAIDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CVideoAIApp::IsInstalled()
{
	BOOL bInstalled = FALSE;
	DWORD dwDisposition;
	HKEY m_hKey;
	//打开注册表
	LONG ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\TimeLimit", 0L, NULL,
		REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition);
	if (ret != ERROR_SUCCESS)    //打开注册表失败
		return FALSE;
	//从注册表中读入特征值
	DWORD dwVal, dwType, dwLen;
	ret = RegQueryValueEx(m_hKey, "Installed", NULL, &dwType, (unsigned char*)&dwVal, &dwLen);
	if (ret != ERROR_SUCCESS)      //读取数据失败
		return FALSE;
	//检查读到的特征值是否为1，如果是，则表明软件以前已运行过
	bInstalled = (dwVal == 1);
	RegCloseKey(m_hKey);//关闭注册表
	return bInstalled;
}

BOOL CVideoAIApp::GetLimitTime(CTime& t)
{
	DWORD dwDisposition;
	HKEY m_hKey;
	//打开注册表
	LONG ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\TimeLimit", 0L, NULL,
		REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition);
	if (ret != ERROR_SUCCESS)    //打开注册表失败
		return FALSE;
	//从注册表中读入试用期限数据
	DWORD nYear, nMonth, nDay, nHour, nMinute, nSecond;
	DWORD dwType;
	DWORD dwLen;
	ret = RegQueryValueEx(m_hKey, "Year", NULL,    //读取年份
		&dwType, (unsigned char*)&nYear, &dwLen);
	if (ret != ERROR_SUCCESS)      //读取数据失败
		return FALSE;
	ret = RegQueryValueEx(m_hKey, "Month", NULL,    //读取月份
		&dwType, (unsigned char*)&nMonth, &dwLen);
	if (ret != ERROR_SUCCESS)      //读取数据失败
		return FALSE;
	ret = RegQueryValueEx(m_hKey, "Day", NULL,    //读取天
		&dwType, (unsigned char*)&nDay, &dwLen);
	if (ret != ERROR_SUCCESS)      //读取数据失败
		return FALSE;
	ret = RegQueryValueEx(m_hKey, "Hour", NULL,    //读取小时
		&dwType, (unsigned char*)&nHour, &dwLen);
	if (ret != ERROR_SUCCESS)      //读取数据失败
		return FALSE;
	ret = RegQueryValueEx(m_hKey, "Minute", NULL,    //读取分钟
		&dwType, (unsigned char*)&nMinute, &dwLen);
	if (ret != ERROR_SUCCESS)      //读取数据失败
		return FALSE;
	ret = RegQueryValueEx(m_hKey, "Second", NULL,    //读取秒
		&dwType, (unsigned char*)&nSecond, &dwLen);
	if (ret != ERROR_SUCCESS)                         //读取数据失败
		return FALSE;
	//关闭注册表
	RegCloseKey(m_hKey);
	t = CTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	return TRUE;
}

BOOL CVideoAIApp::SetLimitTime(DWORD nYear, DWORD nMonth, DWORD nDay, DWORD nHour, DWORD nMinute, DWORD nSecond)
{
	DWORD dwDisposition;
	HKEY m_hKey;
	//打开注册表
	LONG ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\TimeLimit", 0L, NULL,
		REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition);
	if (ret != ERROR_SUCCESS)    //打开注册表失败
		return FALSE;
	//向注册表中写入试用期限数据
	ret = RegSetValueEx(m_hKey, "Year", NULL,      //写入年
		REG_DWORD, (CONST BYTE*) & nYear, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)                       //写入数据失败       
		return FALSE;
	ret = RegSetValueEx(m_hKey, "Month", NULL,     //写入月份
		REG_DWORD, (CONST BYTE*) & nMonth, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)                       //写入数据失败
		return FALSE;
	ret = RegSetValueEx(m_hKey, "Day", NULL,       //写入天
		REG_DWORD, (CONST BYTE*) & nDay, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)                       //写入数据失败
		return FALSE;
	ret = RegSetValueEx(m_hKey, "Hour", NULL,      //写入小时
		REG_DWORD, (CONST BYTE*) & nHour, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)                       //写入数据失败
		return FALSE;
	ret = RegSetValueEx(m_hKey, "Minute", NULL,    //读取分钟数据
		REG_DWORD, (CONST BYTE*) & nMinute, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)                       //写入数据失败
		return FALSE;
	ret = RegSetValueEx(m_hKey, "Second", NULL,    //写入秒
		REG_DWORD, (CONST BYTE*) & nSecond, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)                       //写入数据失败
		return FALSE;
	//写入特征数据，表明不是第一次使用
	DWORD dwVal = 1;
	ret = RegSetValueEx(m_hKey, "Installed", NULL,    //写入特征值
		REG_DWORD, (CONST BYTE*) & dwVal, sizeof(DWORD));
	if (ret != ERROR_SUCCESS)				//写入数据失败
		return FALSE;
	RegCloseKey(m_hKey);	//关闭注册表
	return TRUE;
}

