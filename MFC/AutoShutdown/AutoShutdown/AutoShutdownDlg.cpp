
// AutoShutdownDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoShutdown.h"
#include "AutoShutdownDlg.h"
#include "afxdialogex.h"

#include "tab1Dlg.h"
#include "tab2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoShutdownDlg 对话框

CAutoShutdownDlg::CAutoShutdownDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoShutdownDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoShutdownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, mTabCtrl);


}

BEGIN_MESSAGE_MAP(CAutoShutdownDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAutoShutdownDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON1, &CAutoShutdownDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CAutoShutdownDlg 消息处理程序

BOOL CAutoShutdownDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO: 在此添加额外的初始化代码
	mTabCtrl.InsertItem(0, _T("特定时间关机"));
	mTabCtrl.InsertItem(1, _T("倒计时间关机"));

	//创建两个对话框，与资源对应，指定父对话框。
	m_tab1.Create(IDD_TAB1DLG, &mTabCtrl);
	m_tab2.Create(IDD_TAB2DLG, &mTabCtrl);

	//设定在Tab内显示的范围
	CRect rc;
	mTabCtrl.GetClientRect(rc);
	rc.top += 22;
	rc.bottom -= 2;
	rc.left += 1;
	rc.right -= 2;
	m_tab1.MoveWindow(&rc);
	m_tab2.MoveWindow(&rc);
 
	m_tab1.ShowWindow(SW_SHOW);
	m_tab2.ShowWindow(SW_HIDE);

	mTabCtrl.SetCurSel(0);

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoShutdownDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoShutdownDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAutoShutdownDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int num = mTabCtrl.GetCurSel();
    switch(num)
    {
    case 0:
		m_tab1.ShowWindow(SW_SHOW);
		m_tab2.ShowWindow(SW_HIDE);
		break;
    case 1:
		m_tab1.ShowWindow(SW_HIDE);
		m_tab2.ShowWindow(SW_SHOW);
    }

	*pResult = 0;
}



void CAutoShutdownDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cmd("@echo off & for /f \"tokens=1,4*\" %%i in (\'at\') do ( if /i \"%%j\"==\"shutdown\" at %%i /delete )");

	CFile file("temp.bat",CFile::modeCreate | CFile::modeWrite);
	file.Write(cmd,strlen(cmd));
	file.Close();
	system("temp.bat");
	file.Remove("temp.bat");

}
