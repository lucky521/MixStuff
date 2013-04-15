// mydlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mymfc.h"
#include "mydlg.h"
#include "afxdialogex.h"

#include "MainFrm.h"


// mydlg 对话框

IMPLEMENT_DYNAMIC(mydlg, CDialog)

mydlg::mydlg(CWnd* pParent /*=NULL*/)
	: CDialog(mydlg::IDD, pParent)
{

}

mydlg::~mydlg()
{
}

void mydlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(mydlg, CDialog)
	ON_BN_CLICKED(IDOK, &mydlg::OnBnClickedOk)
//	ON_COMMAND(ID_MYDLG_MYDLG, &mydlg::OnMydlgMydlg)
ON_BN_CLICKED(IDC_BUTTON_MYBTN, &mydlg::OnClickedButtonMybtn)

END_MESSAGE_MAP()


// mydlg 消息处理程序


void mydlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *p = (CMainFrame*)AfxGetMainWnd();
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK1) )
	{
		//这两种方式都可以
		//p->SetStatus(false);
		p->my_status = false;
	}
	else
	{
		p->SetStatus(true);
	}
	CDialog::OnOK();
}





void mydlg::OnClickedButtonMybtn()
{
	// TODO: 在此添加控件通知处理程序代码
	//mydlg.visible = false;
	//HWND mydlg = ::FindWindow(NULL, LPCWSTR("我的对话框"));
	//HWND mypic = ::GetDlgItem(mydlg ,IDB_BITMAP_THATCHER);
	if (GetDlgItem(IDC_PIC)->IsWindowVisible() == 1 )
		GetDlgItem(IDC_PIC)->ShowWindow(SW_HIDE);
	else 
		GetDlgItem(IDC_PIC)->ShowWindow(SW_SHOW);
}

