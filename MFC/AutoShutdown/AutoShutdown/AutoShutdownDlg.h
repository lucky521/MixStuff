
// AutoShutdownDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "tab1Dlg.h"
#include "tab2Dlg.h"


// CAutoShutdownDlg �Ի���
class CAutoShutdownDlg : public CDialogEx
{
// ����
public:
	CAutoShutdownDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTOSHUTDOWN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl mTabCtrl;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	tab1Dlg m_tab1; 
	tab2Dlg m_tab2; 
//	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
};
