class CScribbleDoc : public CDocument
{
protected:
	CScribbleDoc();
	DECLARE_DYNCREATE(CScribbleDoc)

public:
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

public:
	virtual ~CScribbleDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	DECLARE_MESSAGE_MAP()
};