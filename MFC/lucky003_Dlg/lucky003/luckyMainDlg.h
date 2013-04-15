#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// luckyMainDlg 对话框

class luckyMainDlg : public CDialog
{
	DECLARE_DYNAMIC(luckyMainDlg)

public:
	luckyMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~luckyMainDlg();

// 对话框数据
	enum { IDD = IDD_LUCKYMAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl mProcessBar;
	virtual BOOL OnInitDialog();
//	CButton acc_btn;
	afx_msg void OnBnClickedButton1();
};
