#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// tab2Dlg 对话框

class tab2Dlg : public CDialog
{
	DECLARE_DYNAMIC(tab2Dlg)

public:
	tab2Dlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~tab2Dlg();

// 对话框数据
	enum { IDD = IDD_TAB2DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CComboBox minute_combo;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	CDateTimeCtrl m_date;
};
