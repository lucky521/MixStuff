
// mymfcView.h : CmymfcView ��Ľӿ�
//

#pragma once


class CmymfcView : public CView
{
protected: // �������л�����
	CmymfcView();
	DECLARE_DYNCREATE(CmymfcView)

// ����
public:
	CmymfcDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CmymfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // mymfcView.cpp �еĵ��԰汾
inline CmymfcDoc* CmymfcView::GetDocument() const
   { return reinterpret_cast<CmymfcDoc*>(m_pDocument); }
#endif

