enum AfxSig
{
	AfxSig_end = 0, 
	AfxSig_vv,
};


#define ON_COMMAND(id, memberFxn) \
	{WM_COMMAND, 0, (WORD)id, (WORD)id, AfxSig_vv, (AFX_PMSG)memberFxn},