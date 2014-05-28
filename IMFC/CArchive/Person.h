#pragma once
#include"afx.h"

class CPerson : public CObject
{
	DECLARE_SERIAL(CPerson)

private:
	CString _name;
	int _age;
	bool _gender;
	CString _words;




public:
	CPerson(void);
	CPerson(CString name, int age, bool gender = true);
	CString getName();
	CString getWords();
	int getAge();
	void setWords(CString words);
	bool isMale();
	void say();
	void say(CString msg);
	virtual void Serialize(CArchive& ar);
};

