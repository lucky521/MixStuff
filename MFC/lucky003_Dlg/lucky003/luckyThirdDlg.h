#pragma once


// luckyThirdDlg �Ի���

class luckyThirdDlg : public CDialog
{
	DECLARE_DYNAMIC(luckyThirdDlg)

public:
	luckyThirdDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~luckyThirdDlg();

// �Ի�������
	enum { IDD = IDD_LUCKYTHIRDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
