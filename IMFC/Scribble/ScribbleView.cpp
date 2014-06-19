#include "stdafx.h"
#include "Scribble.h"


#include "ScribbleDoc.h"
#include "ScribbleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CScribbleView, CView)
BEGIN_MESSAGE_MAP(CScribbleView, CView)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()


CScribbleView::CScribbleView()
{
	;
}

CScribbleView::~CScribbleView()
{
	;
}

BOOL CScribbleView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CScribbleView::OnDraw(CDC* pDC)
{
	CScribbleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}


BOOL CScribbleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CScribbleView::OnBeginPrinting(CDC*, CPrintInfo*)
{
	;
}

void CScribbleView::OnEndPrinting(CDC*, CPrintInfo*)
{
	;
}

#ifdef _DEBUG
void CScribbleView::AssertValid() const
{
	CView::AssertValid();	
}
void CScribbleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CScribbleDoc* CScribbleView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScribbleDoc)));
	return (CScribbleDoc*)m_pDocument;
}
#endif