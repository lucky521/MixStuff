
// mymfcView.cpp : CmymfcView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "mymfc.h"
#endif

#include "mymfcDoc.h"
#include "mymfcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmymfcView

IMPLEMENT_DYNCREATE(CmymfcView, CView)

BEGIN_MESSAGE_MAP(CmymfcView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CmymfcView ����/����

CmymfcView::CmymfcView()
{
	// TODO: �ڴ˴���ӹ������

}

CmymfcView::~CmymfcView()
{
}

BOOL CmymfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CmymfcView ����

void CmymfcView::OnDraw(CDC* /*pDC*/)
{
	CmymfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}

void CmymfcView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CmymfcView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CmymfcView ���

#ifdef _DEBUG
void CmymfcView::AssertValid() const
{
	CView::AssertValid();
}

void CmymfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmymfcDoc* CmymfcView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmymfcDoc)));
	return (CmymfcDoc*)m_pDocument;
}
#endif //_DEBUG


// CmymfcView ��Ϣ�������
