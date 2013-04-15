#pragma once
#include "afxcmn.h"


// MysqlDlg 对话框

class MysqlDlg : public CDialog
{
	DECLARE_DYNAMIC(MysqlDlg)

public:
	MysqlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MysqlDlg();

// 对话框数据
	enum { IDD = IDD_MYSQLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonConnect();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_list;
};
