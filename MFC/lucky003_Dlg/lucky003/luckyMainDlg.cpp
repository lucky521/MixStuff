// luckyMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "lucky003.h"
#include "luckyMainDlg.h"
#include "afxdialogex.h"

#include "luckyThirdDlg.h"

// luckyMainDlg 对话框

IMPLEMENT_DYNAMIC(luckyMainDlg, CDialog)

luckyMainDlg::luckyMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(luckyMainDlg::IDD, pParent)
{

}

luckyMainDlg::~luckyMainDlg()
{
}

void luckyMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, mProcessBar);
	//  DDX_Control(pDX, IDC_BUTTON1, acc_btn);
}


BEGIN_MESSAGE_MAP(luckyMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &luckyMainDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// luckyMainDlg 消息处理程序



BOOL luckyMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	mProcessBar.SetRange(0,100);
	mProcessBar.SetPos(0);
	mProcessBar.SetStep(1);



	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void luckyMainDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	int pos = mProcessBar.GetPos();
	if (pos == 100){
		MessageBox(_T("加速已完成!"),_T("恭喜"));
		
		luckyMainDlg::OnOK();//先关闭登录对话框，再打开新对话框。
		luckyThirdDlg thirdDlg;
		thirdDlg.DoModal();
	}
	pos = pos + 10;
	mProcessBar.SetPos(pos);

	
}
