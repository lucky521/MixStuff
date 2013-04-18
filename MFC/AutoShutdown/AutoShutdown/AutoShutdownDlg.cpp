
// AutoShutdownDlg.cpp : ʵ���ļ�
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


// CAutoShutdownDlg �Ի���

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


// CAutoShutdownDlg ��Ϣ�������

BOOL CAutoShutdownDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	mTabCtrl.InsertItem(0, _T("�ض�ʱ��ػ�"));
	mTabCtrl.InsertItem(1, _T("����ʱ��ػ�"));

	//���������Ի�������Դ��Ӧ��ָ�����Ի���
	m_tab1.Create(IDD_TAB1DLG, &mTabCtrl);
	m_tab2.Create(IDD_TAB2DLG, &mTabCtrl);

	//�趨��Tab����ʾ�ķ�Χ
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

	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoShutdownDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAutoShutdownDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAutoShutdownDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString cmd("@echo off & for /f \"tokens=1,4*\" %%i in (\'at\') do ( if /i \"%%j\"==\"shutdown\" at %%i /delete )");

	CFile file("temp.bat",CFile::modeCreate | CFile::modeWrite);
	file.Write(cmd,strlen(cmd));
	file.Close();
	system("temp.bat");
	file.Remove("temp.bat");

}
