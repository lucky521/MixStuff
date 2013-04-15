// luckyMainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lucky003.h"
#include "luckyMainDlg.h"
#include "afxdialogex.h"

#include "luckyThirdDlg.h"

// luckyMainDlg �Ի���

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


// luckyMainDlg ��Ϣ�������



BOOL luckyMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	mProcessBar.SetRange(0,100);
	mProcessBar.SetPos(0);
	mProcessBar.SetStep(1);



	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void luckyMainDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int pos = mProcessBar.GetPos();
	if (pos == 100){
		MessageBox(_T("���������!"),_T("��ϲ"));
		
		luckyMainDlg::OnOK();//�ȹرյ�¼�Ի����ٴ��¶Ի���
		luckyThirdDlg thirdDlg;
		thirdDlg.DoModal();
	}
	pos = pos + 10;
	mProcessBar.SetPos(pos);

	
}
