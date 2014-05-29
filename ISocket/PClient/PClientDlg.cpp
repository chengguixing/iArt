
// PClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PClient.h"
#include "PClientDlg.h"
#include "afxdialogex.h"
#include "Item.h"
#include "ClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPClientDlg �Ի���



CPClientDlg::CPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPClientDlg::IDD, pParent), m_bPulsePacket(FALSE), m_state(UNKNOWN), m_pClientSocket(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_SEP_STATIC, m_ctlSep);
	DDX_Control(pDX, IDC_CLIENT_TIME_STATIC, m_ctlTime);
	DDX_Control(pDX, IDC_CLIENT_SN_STATIC, m_ctlSN);
	DDX_Control(pDX, IDC_CLIENT_NAME_STATIC, m_ctlName);

}

BEGIN_MESSAGE_MAP(CPClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPClientDlg ��Ϣ�������

BOOL CPClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pClientSocket = new CClientSocket(this);

	if (FALSE == ConnectServerAndGetPaper())
	{
		EndDialog(FALSE);
		return FALSE;
	}

	InitPaperTitle();
	InitPaper();
	BeginExam();

	GetClientRect(m_rect);
	SetScrollRange(SB_VERT, 0, 100);
	SetTimer(1, 1000 * 60, NULL);
	SetTimer(2, 1000 * 7, NULL);
	m_bPulsePacket = TRUE;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CPClientDlg::ConnectServerAndGetPaper(void)
{
	if (IDOK != m_loginDlg.DoModal())
	{
		return FALSE;
	}

	if (FALSE == m_pClientSocket->ConnectServer(m_loginDlg.m_lservIP, m_loginDlg.m_servPort))
	{
		AfxMessageBox(TEXT("���ӷ�����ʧ��!"));
		return FALSE;
	}

	HDR hdr;
	hdr.ustype = STATE;
	hdr.usLen = sizeof(u_short) + m_loginDlg.m_strSN.GetLength();
	m_pClientSocket->SendPacket(&hdr, CPClientDlg::LOGIN, (char*)(LPCTSTR)m_loginDlg.m_strSN);

	m_state = CPClientDlg::LOGIN;

	return (m_pClientSocket->RecvPaper(m_strName, &m_listItem));
}

void CPClientDlg::InitPaper(void)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	POSITION pos = NULL;
	int i = 0; 
	for (pos = m_listItem.GetHeadPosition(); NULL != pos;)
	{
		CItem* pItem = (CItem*)m_listItem.GetNext(pos);

		CRect rect;
		rect.left = ITEMLEFT;
		rect.top = PAPERHEADHEIGHT + ITEMHEIGHT * i;
		rect.right = rect.left + clientRect.Width() - ITEMLEFT;
		rect.bottom = rect.top + ITEMHEIGHT;

		pItem->Create(IDD_ITEM_DLG, this);
		pItem->MoveWindow(rect, TRUE);
		pItem->ShowWindow(SW_SHOW);
		++i;
	}
	m_offsetHeight = PAPERHEADHEIGHT + ITEMHEIGHT * (m_listItem.GetCount()) - clientRect.Height();
}


void CPClientDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nCurPos;
	int nPrevPos;
	CRect rect;


	GetClientRect(&rect);
	nPrevPos = GetScrollPos(SB_VERT);
	nCurPos = nPrevPos;

	switch (nSBCode)
	{
	case SB_PAGELEFT:
		{
			nCurPos = nPrevPos - 20;
			if (nCurPos < 0)
			{
				nCurPos = 0;
			}
			SetScrollPos(SB_VERT, nCurPos);
			break;
		}
	case SB_PAGERIGHT:
		{
			nCurPos = nPrevPos + 20;
			if (nCurPos > 100)
			{
				nCurPos = 100;
			}
			SetScrollPos(SB_VERT, nCurPos);
			break;
		}
	case SB_THUMBPOSITION:
		{
			SetScrollPos(SB_VERT, nPos);
			break;
		}
	case SB_THUMBTRACK:
		{
			break;
		}
	case SB_LINELEFT:
		{
			nCurPos = nPrevPos - 5;
			if (nCurPos < 0)
			{
				nCurPos = 0;
			}
			SetScrollPos(SB_VERT, nCurPos);
			break;
		}

	case SB_LINERIGHT:
		{
			nCurPos = nPrevPos + 5;
			if (nCurPos > 100)
			{
				nCurPos = 100;
			}
			SetScrollPos(SB_VERT, nCurPos);
			break;
		}
	case SB_ENDSCROLL:
		break;
	}

	nCurPos = GetScrollPos(SB_VERT);
	ScrollWindow(0, m_offsetHeight * (nPrevPos - nCurPos) / 100);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CPClientDlg::OnTimer(UINT nIDEvent)
{
	if (1 == nIDEvent)
	{
		static int nTimes = 1;
		int nLeftTime = 60 - nTimes;

		CString strLeftTime;
		strLeftTime.Format("%s%d%s", TEXT("ʣ��ʱ��:"), nLeftTime, TEXT("����"));
		CStatic* pStatic;
		pStatic = (CStatic*)GetDlgItem(IDC_CLIENT_TIME_STATIC);
		pStatic->SetWindowText(strLeftTime);
		++nTimes;

		if (60 == nTimes)
		{
			m_bPulsePacket = FALSE;
			AfxMessageBox(TEXT("����ʱ�����"));
			SendResult();
			OnOK();
			return ;
		}
	}
	else if (2 == nIDEvent)
	{
		if (TRUE == m_bPulsePacket)
		{
			m_pClientSocket->SendPulsePacket();
		}
	}
	CDialog::OnTimer(nIDEvent);
}

CPClientDlg::~CPClientDlg(void)
{
	POSITION pos = NULL;
	for (pos = m_listItem.GetHeadPosition(); NULL != pos;)
	{
		CItem* pItem = (CItem*)m_listItem.GetNext(pos);
		delete pItem;
		pItem = NULL;
	}
	m_listItem.RemoveAll();

	delete m_pClientSocket;
	m_pClientSocket = NULL;
}


void CPClientDlg::BeginExam(void)
{
	HDR hdr;
	hdr.ustype = STATE;
	hdr.usLen = sizeof(u_short);
	m_pClientSocket->SendPacket(&hdr, CPClientDlg::DOING, NULL);

	m_state = CPClientDlg::DOING;
}


void CPClientDlg::InitPaperTitle(void)
{
	SetWindowText(TEXT("����ϵͳ"));

	CString strTemp;
	strTemp = TEXT("ѧ��");
	strTemp += m_loginDlg.m_strSN;
	m_ctlSN.SetWindowTextA(strTemp);

	strTemp.Empty();
	strTemp = TEXT("����");
	strTemp += m_strName;
	m_ctlName.SetWindowTextA(strTemp);


	strTemp.Empty();
	CString strLeftTime;
	strLeftTime.Format("%s%s", TEXT("ʣ��ʱ��"), TEXT("60����"));
	strTemp += strLeftTime;
	m_ctlTime.SetWindowTextA(strTemp);
	m_ctlSep.SetWindowText("=======================================================================================================================");
}


void CPClientDlg::SendResult(void)
{
	CString strResult;
	CString strItem;

	POSITION pos = NULL;
	for (pos = m_listItem.GetHeadPosition(); NULL != pos;)
	{
		CItem* pItem = (CItem*)m_listItem.GetNext(pos);
		strItem.Format("%d", pItem->m_nSelected);
		strResult += strItem;
	}


	HDR hdr;
	hdr.ustype = STATE;
	hdr.usLen = sizeof(u_short) + strResult.GetLength();
	m_pClientSocket->SendPacket(&hdr, CPClientDlg::DONE, (char*)(LPCTSTR)strResult);
}


void CPClientDlg::OnOK()
{
	if (IDYES == AfxMessageBox(TEXT("��ȷ�Ͻ���ô?"), MB_YESNO))
	{
		SendResult();
	}
	else
	{
		return ;
	}


	KillTimer(1);
	KillTimer(2);
	CDialog::OnOK();
}