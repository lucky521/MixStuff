#pragma once


// mydlg �Ի���

class mydlg : public CDialog
{
	DECLARE_DYNAMIC(mydlg)

public:
	mydlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~mydlg();

// �Ի�������
	enum { IDD = IDD_MYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	afx_msg void OnMydlgMydlg();
	afx_msg void OnClickedButtonMybtn();
	afx_msg void OnBnClickedCheck1();
};
