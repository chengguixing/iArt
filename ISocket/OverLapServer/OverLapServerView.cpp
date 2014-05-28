
// OverLapServerView.cpp : COverLapServerView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "OverLapServer.h"
#endif

#include "OverLapServerDoc.h"
#include "OverLapServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COverLapServerView

IMPLEMENT_DYNCREATE(COverLapServerView, CView)

BEGIN_MESSAGE_MAP(COverLapServerView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// COverLapServerView ����/����

COverLapServerView::COverLapServerView()
{
	// TODO: �ڴ˴���ӹ������

}

COverLapServerView::~COverLapServerView()
{
}

BOOL COverLapServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// COverLapServerView ����

void COverLapServerView::OnDraw(CDC* /*pDC*/)
{
	COverLapServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// COverLapServerView ��ӡ

BOOL COverLapServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void COverLapServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void COverLapServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// COverLapServerView ���

#ifdef _DEBUG
void COverLapServerView::AssertValid() const
{
	CView::AssertValid();
}

void COverLapServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COverLapServerDoc* COverLapServerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COverLapServerDoc)));
	return (COverLapServerDoc*)m_pDocument;
}
#endif //_DEBUG


// COverLapServerView ��Ϣ�������
