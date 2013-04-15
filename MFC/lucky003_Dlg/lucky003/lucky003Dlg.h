
// lucky003Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// Clucky003Dlg 对话框
class Clucky003Dlg : public CDialogEx
{
// 构造
public:
	Clucky003Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LUCKY003_DIALOG };

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
	CComboBox Way_Combo;
	CEdit username;
	afx_msg void OnBnClickedOk();
};
