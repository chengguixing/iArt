#pragma once
class CUserView : public CListView
{
public:
	CUserView(void);
	DECLARE_DYNCREATE(CUserView)

public:
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);


protected:
	virtual ~CUserView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

public:
	void UpdateUserList(CString strUserList);
	void UnInit(void);

private:
	void InitListCtrlSetting(void);
	void UpdateListCtrl(void);
	void ParseUserInfo(CString strUserList);
	void DeleteUserList(void);
	void PreparePlay(CString strName);

private:
	CImageList* m_pImageList;
	CObList m_UserList;
	CCriticalSection m_csList;
};

