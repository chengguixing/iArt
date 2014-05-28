
#include "Person.h"


IMPLEMENT_SERIAL(CPerson, CObject, 1)


CPerson::CPerson(void)
{
	_name = _T("������");
	_age = 0;
	_gender = true;
}

CPerson::CPerson(CString name, int age, bool gender)
{
	_name = name;
	_age = age;
	_gender = gender;
}

CString CPerson::getName()
{
	return _name;
}

CString CPerson::getWords()
{
	return _words;
}
int CPerson::getAge()
{
	return _age;
}
void CPerson::setWords(CString words)
{
	_words = words;
}
bool CPerson::isMale()
{
	return _gender;
}
void CPerson::say()
{
	say(_words);
}
void CPerson::say(CString msg)
{
	_tprintf(_T("%s : %s\r\n"), _name, msg);
}
void CPerson::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << this->_name << this->_age << this->_gender << this->_words;
	}
	else
	{
		ar >> this->_name >> this->_age >> this->_gender >> this->_words;
	}
}