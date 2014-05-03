template <typename T>
class auto_ptr
{
public:
	explicit auto_ptr(T* p = 0) : pointee(p)
	{
		NULL;
	}
	template <typename U>
	auto_ptr(auto_ptr<U>& rhs) : pointee(rhs.release())
	{
		NULL;
	}
	~auto_ptr()
	{
		delete pointee;
	}

	template <typename U>
	auto_ptr<T>& operator= (auto_ptr<U>& rhs)
	{
		if (this != &rhs)
		{
			reset(rhs.release());
		}
		return *this;
	}
	T& operator* () const
	{
		return *pointee;
	}
	T* operator-> ()const
	{
		return pointee;
	}
	T* get() const
	{
		return pointee;
	}

	T* release()
	{
		T* temp(pointee);
		pointee = NULL;
		return temp;
	}
	void reset(T* ptr = 0)
	{
		if (pointee != ptr)
		{
			delete pointee;
			pointee = ptr;
		}
	}
private:
	T* pointee;
};

int main(void)
{
	return 0;
}