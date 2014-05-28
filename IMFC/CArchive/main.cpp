
#include "Person.h"
#include <locale.h>
#include <stdlib.h>


int main(void)
{
	setlocale(LC_ALL, "chs");

	CPerson tong(_T("١����"), 28, false);
	tong.setWords(_T("�����"));

	CPerson bai(_T("��չ��"), 27, true);
	bai.setWords(_T("������Ѩ��!"));

	CFile oFile(_T("persons.archive"), CFile::modeCreate | CFile::modeWrite);
	CArchive oar(&oFile, CArchive::store);

	oar << &tong;
	oar << &bai;
	
	
	
	oar.Close();  
	oFile.Close(); 



	CFile iFile(_T("persons.archive"), CFile::modeRead);
	CArchive iar(&iFile, CArchive::load);

	CPerson *p1 = NULL;
	CPerson *p2 = NULL;

	iar >> p1;
	iar >> p2;


	p1->say();
	p2->say();

	delete p1;
	delete p2;


	iar.Close();  
	iFile.Close(); 


	system("pause");
	return 0;
}