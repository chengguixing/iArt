#include "stdafx.h"
#include "Scribble.h"

#include "ScribbleDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CScribbleDoc, CDocument)


BEGIN_MESSAGE_MAP(CScribbleDoc, CDocument)
END_MESSAGE_MAP()


CScribbleDoc::CScribbleDoc()
{
	;
}

CScribbleDoc::~CScribbleDoc()
{
	;
}


BOOL CScribbleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	return TRUE;
}

void CScribbleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		;
	}
	else
	{
		;
	}
}

#ifdef _DEBUG
void CScribbleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CScribbleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif

