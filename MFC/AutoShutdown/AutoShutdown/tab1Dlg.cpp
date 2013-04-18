// tab1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoShutdown.h"
#include "tab1Dlg.h"
#include "afxdialogex.h"


// tab1Dlg 对话框

IMPLEMENT_DYNAMIC(tab1Dlg, CDialog)

tab1Dlg::tab1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(tab1Dlg::IDD, pParent)
{

}

tab1Dlg::~tab1Dlg()
{
}

void tab1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_COMBO1, hour_combo);
	//  DDX_Control(pDX, IDC_COMBO2, minute_combo);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date);
}


BEGIN_MESSAGE_MAP(tab1Dlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &tab1Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// tab1Dlg 消息处理程序


BOOL tab1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	/*
	// TODO:  在此添加额外的初始化
	for(int i=0; i < 24; i++){ //看看这里，整数是怎么转换成字符串的
		char t[5];
		CString str;
		sprintf(t, "%d", i);
		str = t;
		hour_combo.AddString(LPCTSTR(str));
	}
	hour_combo.SetCurSel(0);

	for(int i=0; i < 60; i++){ //看看这里，整数是怎么转换成字符串的
		char t[5];
		CString str;
		sprintf(t, "%d", i);
		str = t;
		minute_combo.AddString(LPCTSTR(str));
	}
	minute_combo.SetCurSel(0);
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//void tab1Dlg::OnBnClickedOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//
//
//	CDialog::OnOK();
//}


void tab1Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	//int hour, minute;
	//hour = hour_combo.GetCurSel();
	//minute = minute_combo.GetCurSel();
	
	CTime time;
	m_date.GetTime(time);
	CString strTime = time.Format("%H:%M:%S");
	//int h = time.GetHour();
	//int m = time.GetMinute();
	//int s = time.GetSecond();

	//MessageBox(strTime);
	CString s1("at ");
	CString s2(" shutdown -s -f -t 0");
	CString cmd = s1 + strTime + s2;
	system((const char*)(cmd));

}


//void tab1Dlg::OnBnClickedButton2()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	system("explor: c:\WINDOWS\Tasks");
//}
