#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

template <typename CountedType>
class ObjectCounter
{
private:
	static size_t count;

protected:
	ObjectCounter()
	{
		++ObjectCounter<CountedType>::count;
	}
	ObjectCounter(ObjectCounter<CountedType> const&)
	{
		++ObjectCounter<CountedType>::count;
	}
	~ObjectCounter()
	{
		--ObjectCounter<CountedType>::count;
	}
public:
	static size_t live()
	{
		return ObjectCounter<CountedType>::count;
	}
};

template <typename CountedType>
size_t ObjectCounter<CountedType>::count = 0;


template <typename CharT>
class MyString : public ObjectCounter<MyString<CharT> >
{

};

int main(void)
{
	MyString<char> s1, s2;
	MyString<wchar_t> ws;

	cout << MyString<char>::live() << endl;
	cout << MyString<wchar_t>::live() << endl;

	system("pause");
	return 0;
}