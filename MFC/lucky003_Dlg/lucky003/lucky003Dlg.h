
// lucky003Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// Clucky003Dlg �Ի���
class Clucky003Dlg : public CDialogEx
{
// ����
public:
	Clucky003Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LUCKY003_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox Way_Combo;
	CEdit username;
	afx_msg void OnBnClickedOk();
};
