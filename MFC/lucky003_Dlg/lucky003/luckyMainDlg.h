#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// luckyMainDlg �Ի���

class luckyMainDlg : public CDialog
{
	DECLARE_DYNAMIC(luckyMainDlg)

public:
	luckyMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~luckyMainDlg();

// �Ի�������
	enum { IDD = IDD_LUCKYMAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl mProcessBar;
	virtual BOOL OnInitDialog();
//	CButton acc_btn;
	afx_msg void OnBnClickedButton1();
};
