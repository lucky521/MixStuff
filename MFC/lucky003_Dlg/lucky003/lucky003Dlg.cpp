
// lucky003Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lucky003.h"
#include "lucky003Dlg.h"
#include "afxdialogex.h"

#include "luckyMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Clucky003Dlg �Ի���


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


// Clucky003Dlg ��Ϣ�������

BOOL Clucky003Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	//����combo�ĳ�ʼ������
	Way_Combo.AddString(_T("��ţ"));
	Way_Combo.AddString(_T("����"));
	Way_Combo.AddString(_T("�մ���"));
	Way_Combo.SetCurSel(1);





	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Clucky003Dlg::OnPaint()
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
HCURSOR Clucky003Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Clucky003Dlg::OnBnClickedOk()
{
	//��ȡ��½�������������
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
		MessageBox(_T("�ף��������û���"), _T("��ʾ"));
		return;
	}
	else if(str_keyword == ""){
		MessageBox(_T("�ף�����������"), _T("��ʾ"));
		return;
	}


	//1���������ݿ� 
	//2����ѯ���ݿ�
	//3��У���û���������

	//�����֤ʧ�ܣ���ʾ����
	
	if (index == 0 && str_username == "���"){
		//�����֤��ȷ
		CDialogEx::OnOK();//�ȹرյ�¼�Ի����ٴ��¶Ի���
		luckyMainDlg mainDlg;
		mainDlg.DoModal();
	}
	else {
		MessageBox("��������ȷ���û���Ϣ��");
	}
	
		
}
