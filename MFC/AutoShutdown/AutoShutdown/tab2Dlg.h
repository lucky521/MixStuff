#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// tab2Dlg �Ի���

class tab2Dlg : public CDialog
{
	DECLARE_DYNAMIC(tab2Dlg)

public:
	tab2Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~tab2Dlg();

// �Ի�������
	enum { IDD = IDD_TAB2DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
//	CComboBox minute_combo;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	CDateTimeCtrl m_date;
};
