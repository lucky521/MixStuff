#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// tab1Dlg �Ի���

class tab1Dlg : public CDialog
{
	DECLARE_DYNAMIC(tab1Dlg)

public:
	tab1Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~tab1Dlg();

// �Ի�������
	enum { IDD = IDD_TAB1DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
//	CComboBox hour_combo;
//	CComboBox minute_combo;
	virtual BOOL OnInitDialog();
	CDateTimeCtrl m_date;
//	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
//	afx_msg void OnBnClickedButton2();
};
