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
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_USER_PLAY_ICON));	//游戏图标
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_USER_WAIT_ICON));	//等待图标		
	listCtrl.SetImageList(m_pImageList, LVSIL_SMALL);				//赋值给列表控件

	//设置列表头
	listCtrl.InsertColumn(0, _T("名称"), LVCFMT_CENTER);
	listCtrl.InsertColumn(1, _T("状态"), LVCFMT_CENTER);

	//设置各列宽度
	CRect rect;
	GetClientRect(&rect);//获取客户区宽度
	int nWidth = rect.Width();
	listCtrl.SetColumnWidth(0, nWidth * 70 / 100);//名称50%
	listCtrl.SetColumnWidth(1, nWidth * 30 / 100);//状态50%	
}


/*
 * 更新用户列表界面
 */
void CUserView::UpdateListCtrl( void )
{
	//获取列表视图控件
	CListCtrl &listctl = GetListCtrl();
	listctl.DeleteAllItems();	//删除原来数据项
	
	m_csList.Lock();
	POSITION	pos = NULL;		//位置变量
	int			nIndex = 0;		//序号
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		//取出用户信息
		CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);		
		//第一列
		listctl.InsertItem(nIndex, pUserInfo->m_strName, -1);//名称
		
		//第二列
		LVITEM item;
		item.mask = LVIF_IMAGE;
		item.iItem = nIndex;
		item.iSubItem = 1;
		if (pUserInfo->m_state == CUserInfo::PLAY)	//游戏状态
		{
			item.iImage = 0;
		}else										//等待游戏状态
		{
			item.iImage = 1;
		}
		listctl.SetItem(&item);		
		nIndex++;									//序号加1									
	}
	m_csList.Unlock();	
}


/*
 * 解析用户信息
 */
void CUserView::ParseUserInfo( CString strUserInfo )
{
	CUserInfo *pUserInfo = new CUserInfo;
	int nFind = strUserInfo.Find('|');
	ASSERT(-1 != nFind);
	pUserInfo->m_strName = strUserInfo.Left(nFind);						//名字
	CString strRight = strUserInfo.Mid(nFind + 1);						//剩余字符串
	
	nFind = strRight.Find('|');
	pUserInfo->m_lIP = atol(strRight.Left(nFind));						//IP
		
	strRight = strRight.Mid(nFind + 1);
	nFind = strRight.Find('|');
	pUserInfo->m_uPort = atoi(strRight.Left(nFind));					//端口

	pUserInfo->m_state = (CUserInfo::USERSTATE)(atoi(strRight.Right(1)));//状态

	m_csList.Lock();
	m_UserList.AddTail(pUserInfo);//加入链表
	m_csList.Unlock();
}

/*
 * 删除用户列表
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
 * 初始化
 */
void CUserView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	//初始化用户列表界面
	InitListCtrlSetting();	
	
}

/*
 * 双击列表视图控件消息响应函数
 */
void CUserView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *pListView = (NM_LISTVIEW*)pNMHDR;
	if (pListView->iItem != -1)
	{
		//获取被选中数据项
		int nSelectItem = pListView->iItem;
		//获取用户名称
		CString strUserName = GetListCtrl().GetItemText(nSelectItem, 0);
		//请求游戏
		PreparePlay(strUserName);
	}

	*pResult = 0;
}

/*
 * 准备游戏
 */
void CUserView::PreparePlay(CString strName)
{
	//获取框架指针
	CMainFrame *pMF = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CClientView *pClientView = pMF->GetClientView();
	CUserInfo hostInfo = pClientView->m_UserInfo;
	//判断是否为用户自身
	if (hostInfo.m_strName == strName)
	{
		AfxMessageBox("您不能和自己下棋");
		return;
	}

	//获取用户信息
	m_csList.Lock();
	CUserInfo *pUserInfo = NULL; 
	POSITION pos = NULL;
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		if (pUserInfo->m_strName == strName)//找到该用户
		{
			if(pUserInfo->m_state == CUserInfo::PLAY)//该用户正在游戏状态
			{
				AfxMessageBox("对方正在游戏，请稍候再邀请。");
				m_csList.Unlock();				
				return;
			}else//该用户处于等待状态
			{
				break;				
			}
		}
	}
	m_csList.Unlock();
	pClientView->RequestPlay(*pUserInfo);//请求游戏
}

void CUserView::OnDestroy() 
{
	CListView::OnDestroy();
	
	UnInit();
}
/*
 * 反初始化
 */
void CUserView::UnInit( void )
{
	DeleteUserList();
	delete m_pImageList;
	m_pImageList = NULL;
}