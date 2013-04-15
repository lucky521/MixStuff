
// mymfcView.cpp : CmymfcView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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

// CmymfcView 构造/析构

CmymfcView::CmymfcView()
{
	// TODO: 在此处添加构造代码

}

CmymfcView::~CmymfcView()
{
}

BOOL CmymfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CmymfcView 绘制

void CmymfcView::OnDraw(CDC* /*pDC*/)
{
	CmymfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
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


// CmymfcView 诊断

#ifdef _DEBUG
void CmymfcView::AssertValid() const
{
	CView::AssertValid();
}

void CmymfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmymfcDoc* CmymfcView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmymfcDoc)));
	return (CmymfcDoc*)m_pDocument;
}
#endif //_DEBUG


// CmymfcView 消息处理程序
