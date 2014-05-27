#include "stdafx.h"
#include "Client.h"
#include "UserView.h"
#include "UserInfo.h"
#include "ClientView.h"
#include "MainFrame.h"




IMPLEMENT_DYNCREATE(CUserView, CListView)

CUserView::CUserView(void)
{
}


CUserView::~CUserView(void)
{
}


BEGIN_MESSAGE_MAP(CUserView, CListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CUserView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

#ifdef _DEBUG
void CUserView::AssertValid() const
{
	CListView::AssertValid();
}

void CUserView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif


int CUserView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	return 0;
}


void CUserView::UpdateUserList(CString strUserList)
{
	DeleteUserList();


	int nListen = strUserList.GetLength();
	CString strUserInfo;

	for (int nIndex = 0; nIndex < nListen; ++nIndex)
	{
		TCHAR c = strUserList.GetAt(nIndex);
		if (c == '<')
		{
			strUserInfo.Empty();
		}
		else if (c == '>')
		{
			ParseUserInfo(strUserInfo);
		}
		else
		{
			strUserInfo += c;
		}
	}

	UpdateListCtrl();
}

void CUserView::InitListCtrlSetting(void)
{
	CListCtrl& listCtrl = GetListCtrl();
	LONG lStyle;
	lStyle = GetWindowLong(listCtrl.m_hWnd, GWL_STYLE);
	lStyle |= LVS_REPORT;
	SetWindowLong(listCtrl.m_hWnd, GWL_STYLE, lStyle);
	listCtrl.SetExtendedStyle(LVS_EX_SUBITEMIMAGES);


	m_pImageList = new CImageList();

	m_pImageList->Create(32, 32, ILC_COLOR8|ILC_MASK, 2, 2);	
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_USER_PLAY_ICON));	//��Ϸͼ��
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_USER_WAIT_ICON));	//�ȴ�ͼ��		
	listCtrl.SetImageList(m_pImageList, LVSIL_SMALL);				//��ֵ���б�ؼ�

	//�����б�ͷ
	listCtrl.InsertColumn(0, _T("����"), LVCFMT_CENTER);
	listCtrl.InsertColumn(1, _T("״̬"), LVCFMT_CENTER);

	//���ø��п��
	CRect rect;
	GetClientRect(&rect);//��ȡ�ͻ������
	int nWidth = rect.Width();
	listCtrl.SetColumnWidth(0, nWidth * 70 / 100);//����50%
	listCtrl.SetColumnWidth(1, nWidth * 30 / 100);//״̬50%	
}


/*
 * �����û��б����
 */
void CUserView::UpdateListCtrl( void )
{
	//��ȡ�б���ͼ�ؼ�
	CListCtrl &listctl = GetListCtrl();
	listctl.DeleteAllItems();	//ɾ��ԭ��������
	
	m_csList.Lock();
	POSITION	pos = NULL;		//λ�ñ���
	int			nIndex = 0;		//���
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		//ȡ���û���Ϣ
		CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);		
		//��һ��
		listctl.InsertItem(nIndex, pUserInfo->m_strName, -1);//����
		
		//�ڶ���
		LVITEM item;
		item.mask = LVIF_IMAGE;
		item.iItem = nIndex;
		item.iSubItem = 1;
		if (pUserInfo->m_state == CUserInfo::PLAY)	//��Ϸ״̬
		{
			item.iImage = 0;
		}else										//�ȴ���Ϸ״̬
		{
			item.iImage = 1;
		}
		listctl.SetItem(&item);		
		nIndex++;									//��ż�1									
	}
	m_csList.Unlock();	
}


/*
 * �����û���Ϣ
 */
void CUserView::ParseUserInfo( CString strUserInfo )
{
	CUserInfo *pUserInfo = new CUserInfo;
	int nFind = strUserInfo.Find('|');
	ASSERT(-1 != nFind);
	pUserInfo->m_strName = strUserInfo.Left(nFind);						//����
	CString strRight = strUserInfo.Mid(nFind + 1);						//ʣ���ַ���
	
	nFind = strRight.Find('|');
	pUserInfo->m_lIP = atol(strRight.Left(nFind));						//IP
		
	strRight = strRight.Mid(nFind + 1);
	nFind = strRight.Find('|');
	pUserInfo->m_uPort = atoi(strRight.Left(nFind));					//�˿�

	pUserInfo->m_state = (CUserInfo::USERSTATE)(atoi(strRight.Right(1)));//״̬

	m_csList.Lock();
	m_UserList.AddTail(pUserInfo);//��������
	m_csList.Unlock();
}

/*
 * ɾ���û��б�
 */
void CUserView::DeleteUserList( void )
{
	m_csList.Lock();
	POSITION pos;
	for (pos = m_UserList.GetHeadPosition(); NULL != pos; )
	{
		CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		delete pUserInfo;
	}
	m_UserList.RemoveAll();
	m_csList.Unlock();
}


/*
 * ��ʼ��
 */
void CUserView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	//��ʼ���û��б����
	InitListCtrlSetting();	
	
}

/*
 * ˫���б���ͼ�ؼ���Ϣ��Ӧ����
 */
void CUserView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *pListView = (NM_LISTVIEW*)pNMHDR;
	if (pListView->iItem != -1)
	{
		//��ȡ��ѡ��������
		int nSelectItem = pListView->iItem;
		//��ȡ�û�����
		CString strUserName = GetListCtrl().GetItemText(nSelectItem, 0);
		//������Ϸ
		PreparePlay(strUserName);
	}

	*pResult = 0;
}

/*
 * ׼����Ϸ
 */
void CUserView::PreparePlay(CString strName)
{
	//��ȡ���ָ��
	CMainFrame *pMF = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CClientView *pClientView = pMF->GetClientView();
	CUserInfo hostInfo = pClientView->m_UserInfo;
	//�ж��Ƿ�Ϊ�û�����
	if (hostInfo.m_strName == strName)
	{
		AfxMessageBox("�����ܺ��Լ�����");
		return;
	}

	//��ȡ�û���Ϣ
	m_csList.Lock();
	CUserInfo *pUserInfo = NULL; 
	POSITION pos = NULL;
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		if (pUserInfo->m_strName == strName)//�ҵ����û�
		{
			if(pUserInfo->m_state == CUserInfo::PLAY)//���û�������Ϸ״̬
			{
				AfxMessageBox("�Է�������Ϸ�����Ժ������롣");
				m_csList.Unlock();				
				return;
			}else//���û����ڵȴ�״̬
			{
				break;				
			}
		}
	}
	m_csList.Unlock();
	pClientView->RequestPlay(*pUserInfo);//������Ϸ
}

void CUserView::OnDestroy() 
{
	CListView::OnDestroy();
	
	UnInit();
}
/*
 * ����ʼ��
 */
void CUserView::UnInit( void )
{
	DeleteUserList();
	delete m_pImageList;
	m_pImageList = NULL;
}