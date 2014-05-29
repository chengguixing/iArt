#include "stdafx.h"
#include "Item.h"


#define FONTHEIGHT 20


CItem::CItem(CWnd* pParent) : CDialog(CItem::IDD, pParent), m_strItem(TEXT("")), m_nIndex(0), m_nSelected(-1)
{
}

void CItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


void CItem::InitControl(void)
{
	int nIndex = -1;
	nIndex = m_strItem.Find('|');
	if (-1 != nIndex)
	{
		CRect rect;
		GetClientRect(&rect);
		int ctlWidth = (float)rect.Width() / 4;

		CString strIndex = m_strItem.Left(nIndex + 1);
		m_nIndex = atoi(strIndex);

		CString strRight = m_strItem.Mid(nIndex + 1);


		nIndex = strRight.Find('|');
		CString strTemp;
		strTemp.Format("%d", m_nIndex);
		strTemp += ". ";
		strTemp += strRight.Left(nIndex);


		CRect rectctl;
		rectctl.left = rect.left;
		rectctl.top = rect.top;
		rectctl.right = rect.right;
		rectctl.bottom = rect.bottom / 2;
		m_static.Create(strTemp, WS_CHILD | WS_VISIBLE, rectctl, this, 100);


		strTemp = strRight.Mid(nIndex + 1);
		nIndex = strTemp.Find('|');
		CString strLeft = strTemp.Left(nIndex);
		CString strAnswer("A: ");
		strAnswer += strLeft;


		rectctl.left = rect.left;
		rectctl.top = rect.top + FONTHEIGHT;
		rectctl.right = rectctl.left + ctlWidth * 1;
		rectctl.bottom = rectctl.top + FONTHEIGHT;
		m_bt[0].Create(strAnswer, WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, rectctl, this, 1);

		strRight = strTemp.Mid(nIndex + 1);
		nIndex = strRight.Find('|');
		strLeft = strRight.Left(nIndex);
		strAnswer = "B: ";
		strAnswer += strLeft;


		rectctl.left = rect.left + ctlWidth * 1;
		rectctl.top = rect.top + FONTHEIGHT;
		rectctl.right = rectctl.left + ctlWidth;
		rectctl.bottom = rectctl.top + FONTHEIGHT;

		m_bt[1].Create(strAnswer, WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON ,rectctl, this, 2);


		strTemp = strRight.Mid(nIndex + 1);
		nIndex = strTemp.Find('|');
		strLeft = strTemp.Left(nIndex);
		strAnswer = "C: ";
		strAnswer += strLeft;


		rectctl.left = rect.left + ctlWidth * 2;
		rectctl.top = rect.top + FONTHEIGHT;
		rectctl.right = rectctl.left + ctlWidth;
		rectctl.bottom = rectctl.top + FONTHEIGHT;
		m_bt[2].Create(strAnswer, WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, rectctl, this, 2);


		strRight = strTemp.Mid(nIndex + 1);
		strAnswer = "D: ";
		strAnswer += strRight;

		rectctl.left = rect.left + ctlWidth * 3;
		rectctl.top = rect.top + FONTHEIGHT;
		rectctl.right = rectctl.left + ctlWidth;
		rectctl.bottom = rectctl.top + FONTHEIGHT;
		m_bt[3].Create(strAnswer, WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, rectctl, this, 4);

		m_font.CreateFontA(12, 
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			0,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			"ËÎÌו");
		CStatic* pStatic = (CStatic*)GetDlgItem(100);
		pStatic->SetFont(&m_font);
		CButton* pBt = (CButton*)GetDlgItem(1);
		pBt->SetFont(&m_font);
		pBt = (CButton*)GetDlgItem(2);
		pBt->SetFont(&m_font);
		pBt = (CButton*)GetDlgItem(3);
		pBt->SetFont(&m_font);
		pBt = (CButton*)GetDlgItem(4);
		pBt->SetFont(&m_font);

	}
}
BEGIN_MESSAGE_MAP(CItem, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, 1, 4, OnSelected)
END_MESSAGE_MAP()


BOOL CItem::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitControl();
	return TRUE;
}

afx_msg void CItem::OnSelected(UINT nCmdID)
{
	switch(nCmdID)
	{
	case 1:
		m_bt[0].SetCheck(1);
		m_bt[1].SetCheck(0);
		m_bt[2].SetCheck(0);
		m_bt[3].SetCheck(0);
		m_nSelected = 0;	
	case 2:
		m_bt[0].SetCheck(0);
		m_bt[1].SetCheck(1);
		m_bt[2].SetCheck(0);
		m_bt[3].SetCheck(0);
		m_nSelected = 1;	
	case 3:
		m_bt[0].SetCheck(0);
		m_bt[1].SetCheck(0);
		m_bt[2].SetCheck(1);
		m_bt[3].SetCheck(0);
		m_nSelected = 2;	
	case 4:
		m_bt[0].SetCheck(0);
		m_bt[1].SetCheck(0);
		m_bt[2].SetCheck(0);
		m_bt[3].SetCheck(1);
		m_nSelected = 3;	
	default:
		break;
	}
}