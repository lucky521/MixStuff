// tab1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoShutdown.h"
#include "tab1Dlg.h"
#include "afxdialogex.h"


// tab1Dlg �Ի���

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


// tab1Dlg ��Ϣ�������


BOOL tab1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	/*
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	for(int i=0; i < 24; i++){ //���������������ôת�����ַ�����
		char t[5];
		CString str;
		sprintf(t, "%d", i);
		str = t;
		hour_combo.AddString(LPCTSTR(str));
	}
	hour_combo.SetCurSel(0);

	for(int i=0; i < 60; i++){ //���������������ôת�����ַ�����
		char t[5];
		CString str;
		sprintf(t, "%d", i);
		str = t;
		minute_combo.AddString(LPCTSTR(str));
	}
	minute_combo.SetCurSel(0);
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


//void tab1Dlg::OnBnClickedOk()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//
//
//	CDialog::OnOK();
//}


void tab1Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	system("explor: c:\WINDOWS\Tasks");
//}
