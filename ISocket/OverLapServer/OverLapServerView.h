
// OverLapServerView.h : COverLapServerView ��Ľӿ�
//

#pragma once


class COverLapServerView : public CView
{
protected: // �������л�����
	COverLapServerView();
	DECLARE_DYNCREATE(COverLapServerView)

// ����
public:
	COverLapServerDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~COverLapServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // OverLapServerView.cpp �еĵ��԰汾
inline COverLapServerDoc* COverLapServerView::GetDocument() const
   { return reinterpret_cast<COverLapServerDoc*>(m_pDocument); }
#endif

