
// lucky003Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "lucky003.h"
#include "lucky003Dlg.h"
#include "afxdialogex.h"

#include "luckyMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Clucky003Dlg 对话框


Clucky003Dlg::Clucky003Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Clucky003Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Clucky003Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, Way_Combo);
	DDX_Control(pDX, IDC_EDIT1, username);
}

BEGIN_MESSAGE_MAP(Clucky003Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Clucky003Dlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Clucky003Dlg 消息处理程序

BOOL Clucky003Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	//关于combo的初始化设置
	Way_Combo.AddString(_T("大牛"));
	Way_Combo.AddString(_T("笨蛋"));
	Way_Combo.AddString(_T("苏打绿"));
	Way_Combo.SetCurSel(1);





	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Clucky003Dlg::OnPaint()
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
HCURSOR Clucky003Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Clucky003Dlg::OnBnClickedOk()
{
	//获取登陆界面的输入数据
	CString str_username, str_keyword;
	int index;

	username.GetWindowText(str_username);

	CEdit* keyword = (CEdit*) GetDlgItem(IDC_EDIT2);
	keyword->GetWindowText(str_keyword);

	index = Way_Combo.GetCurSel();
	CString strI;  
    strI.Format(_T("%d"),index);  
    CString strC;  
    Way_Combo.GetLBText(index,strC); 

	if (str_username == "" ){
		MessageBox(_T("亲：请输入用户名"), _T("提示"));
		return;
	}
	else if(str_keyword == ""){
		MessageBox(_T("亲：请输入密码"), _T("提示"));
		return;
	}


	//1、连接数据库 
	//2、查询数据库
	//3、校验用户名和密码

	//如果验证失败，提示错误
	
	if (index == 0 && str_username == "安璐"){
		//如果验证正确
		CDialogEx::OnOK();//先关闭登录对话框，再打开新对话框。
		luckyMainDlg mainDlg;
		mainDlg.DoModal();
	}
	else {
		MessageBox("请输入正确的用户信息！");
	}
	
		
}
