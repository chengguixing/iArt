#include "stdafx.h"
#include "Scribble.h"

#include "ChildFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
END_MESSAGE_MAP()


CChildFrame::CChildFrame()
{
	;
}

CChildFrame::~CChildFrame()
{
	;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CMDIChildWnd::PreCreateWindow(cs);
}

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif
