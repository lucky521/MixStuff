#pragma once
#include "afxcmn.h"


// MysqlDlg �Ի���

class MysqlDlg : public CDialog
{
	DECLARE_DYNAMIC(MysqlDlg)

public:
	MysqlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MysqlDlg();

// �Ի�������
	enum { IDD = IDD_MYSQLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonConnect();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_list;
};
