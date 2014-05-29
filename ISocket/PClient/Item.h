#pragma once
#include "afxwin.h"
#include "resource.h"



class CItem :
	public CDialog
{
public:
	CItem(CWnd* pParent = NULL);
	enum{IDD = IDD_ITEM_DLG};


public:
	void InitControl(void);


public:
	CStatic m_static;
	CButton m_bt[4];
	CString m_strItem;
	int m_nIndex;
	CFont m_font;
	int m_nSelected;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSelected(UINT nCmdID);
	DECLARE_MESSAGE_MAP()
};

