#pragma once


// mydlg 对话框

class mydlg : public CDialog
{
	DECLARE_DYNAMIC(mydlg)

public:
	mydlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~mydlg();

// 对话框数据
	enum { IDD = IDD_MYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	afx_msg void OnMydlgMydlg();
	afx_msg void OnClickedButtonMybtn();
	afx_msg void OnBnClickedCheck1();
};
