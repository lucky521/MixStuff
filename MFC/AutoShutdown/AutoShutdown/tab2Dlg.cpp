// tab2Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoShutdown.h"
#include "tab2Dlg.h"
#include "afxdialogex.h"


// tab2Dlg �Ի���

IMPLEMENT_DYNAMIC(tab2Dlg, CDialog)

tab2Dlg::tab2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(tab2Dlg::IDD, pParent)
{

}

tab2Dlg::~tab2Dlg()
{
}

void tab2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_COMBO2, minute_combo);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date);
}


BEGIN_MESSAGE_MAP(tab2Dlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &tab2Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// tab2Dlg ��Ϣ�������


BOOL tab2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	/*
	for(int i=0; i < 60; i++){ //���������������ôת�����ַ�����
		char t[5];
		CString str;
		sprintf(t, "%d", i);
		str = t;
		minute_combo.AddString(LPCTSTR(str));
	}
	minute_combo.SetCurSel(0);
	*/

	//m_date.SetFormat( "00:00:00");
	CTime tmp(1988, 5, 21, 00, 00, 00);
	m_date.SetTime(&tmp);


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void tab2Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CString hour;
	//int minute = minute_combo.GetCurSel();

	//CEdit* hour_edit = (CEdit*) GetDlgItem(IDC_EDIT1);
	//hour_edit->GetWindowText(hour);

	//int time_left =  minute;

	CTime time;
	m_date.GetTime(time);
	int h = time.GetHour();
	int m = time.GetMinute();
	int s = time.GetSecond();

	CTime now = CTime::GetCurrentTime();

	CTimeSpan ts(0, h, m, s);
	CTime ttNew = now + ts;
	CString strTime = ttNew.Format("%H:%M:%S");
	//MessageBox(sDate);

	CString s1("at ");
	CString s2(" shutdown -s -f -t 0");
	CString cmd = s1 + strTime + s2;
	system((const char*)(cmd));
}
