#pragma once



class CMySplitterWnd : public CSplitterWnd  
{
public:
	CMySplitterWnd();
	virtual ~CMySplitterWnd();

	//{{AFX_MSG(CClientView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};