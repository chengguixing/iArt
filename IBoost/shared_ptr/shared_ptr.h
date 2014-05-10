#ifndef _SHARED_PTR_H_7753F4B3_96E5_4220_91D4_68A3B4FF78BE
#define _SHARED_PTR_H_7753F4B3_96E5_4220_91D4_68A3B4FF78BE


template <class T>
class shared_ptr
{
private:
	typedef shared_ptr<T> this_type;

public:
	typedef T element_type;

	shared_ptr() : px(0), pn()
	{
		;
	}
	template <typename U>
	explicit shared_ptr(U* p) : px(p), pn()
	{
		;
	}
};



#endif