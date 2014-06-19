class CScribbleView : public CView
{
protected:
	CScribbleView();
	DECLARE_DYNCREATE(CScribbleView)

public:
	CScribbleDoc* GetDocument();

public:
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
	virtual ~CScribbleView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
protected:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
	inline CScribbleDoc* CScribbleView::GetDocument()
	{
		return (CScribbleDoc*)m_pDocument;
	}
#endif