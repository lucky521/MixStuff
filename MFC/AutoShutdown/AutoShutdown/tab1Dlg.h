#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// tab1Dlg 对话框

class tab1Dlg : public CDialog
{
	DECLARE_DYNAMIC(tab1Dlg)

public:
	tab1Dlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~tab1Dlg();

// 对话框数据
	enum { IDD = IDD_TAB1DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
