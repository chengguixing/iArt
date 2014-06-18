class CMyWinApp : public CWinApp
{
public:
	virtual BOOL InitInstance();

	virtual BOOL OnIdle(LONG lCount);
};


class CMyFrameWnd : public CFrameWnd
{
public:
	CMyFrameWnd();
	afx_msg void OnPaint();
	afx_msg void OnAbout();
	void IdleTimeHandler(LONG lCount);
private:
	DECLARE_MESSAGE_MAP()
	static VOID CALLBACK LineDDACallback(int, int, LPARAM);
};