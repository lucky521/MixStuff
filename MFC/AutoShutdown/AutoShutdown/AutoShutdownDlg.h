
// AutoShutdownDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "tab1Dlg.h"
#include "tab2Dlg.h"


// CAutoShutdownDlg 对话框
class CAutoShutdownDlg : public CDialogEx
{
// 构造
public:
	CAutoShutdownDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOSHUTDOWN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
