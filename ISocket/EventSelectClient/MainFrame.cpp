#include "stdafx.h"
#include "MainFrame.h"
#include "UserView.h"
#include "ClientView.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

	BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
		//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame construction/destruction

	CMainFrame::CMainFrame()
	{
		// TODO: add member initialization code here

	}

	CMainFrame::~CMainFrame()
	{
	}

	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;

		//�������̴��ڴ�С
		CRect rect (0, 0, CELLSIZE * (DIVISIONS + 1), CELLSIZE * (DIVISIONS + 1));

		int cyCaption = ::GetSystemMetrics(SM_CYCAPTION);	//�������߶�
		int cyMenu = ::GetSystemMetrics(SM_CYMENU);			//�˵��߶�
		int cxEdge = ::GetSystemMetrics(SM_CYEDGE) ;		//ˮƽ�߿��
		int cyEdge = ::GetSystemMetrics(SM_CYEDGE) ;		//��ֱ�߸߶�

		//���ÿ�ܴ��ڸ߶ȺͿ��
		SetWindowPos(NULL, 0, 0, 
			rect.Width() + cxEdge * 3 + USERWIEWWIDTH, 
			rect.Height() + cyMenu + cyCaption + cxEdge *2 + 10,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW);
		return 0;
	}

	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CFrameWnd::PreCreateWindow(cs) )
			return FALSE;

		cs.style &=~WS_MAXIMIZEBOX;

		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame diagnostics

#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CFrameWnd::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CFrameWnd::Dump(dc);
	}

#endif //_DEBUG

	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame message handlers


	BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
	{
		//������̬�зִ���
		if (!m_wndSplitter.CreateStatic(this, 1, 2))
			return FALSE;

		//�������̴��ڴ�С
		CRect rect (0, 0, CELLSIZE * (DIVISIONS + 1), CELLSIZE * (DIVISIONS + 1));

		//�������̴�����û��б���
		if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CClientView), 
			rect.Size(), pContext) ||
			!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CUserView), 
			CSize(USERWIEWWIDTH, USERWIEWWIDTH), pContext))
		{
			m_wndSplitter.DestroyWindow();
			return FALSE;
		}
		return TRUE;
	}



	CClientView* CMainFrame::GetClientView( void )
	{
		CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
		return DYNAMIC_DOWNCAST(CClientView, pWnd);
	}

	CUserView* CMainFrame::GetUserView( void )
	{
		CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
		return DYNAMIC_DOWNCAST(CUserView, pWnd);
	}
