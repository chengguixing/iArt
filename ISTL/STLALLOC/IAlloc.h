#ifndef _IALLOC_H_5755FF38_8D1F_4709_BB0F_0D8F8146AB69
#define _IALLOC_H_5755FF38_8D1F_4709_BB0F_0D8F8146AB69


#include <cstddef>
#include <climits>


namespace ISTL
{
	template <typename T>
	inline T* iAllocate(ptrdiff_t size, T*)
	{
		cout << __FUNCTION__ << ':' << size << endl;
		set_new_handler(0);
		T* tmp = (T*)::operator new((size_t)(size * sizeof(T)));
		if (0 == tmp)
		{
			cerr << "out of memory" << endl;
			exit(1);
		}
		return tmp;
	}

	template <typename T>
	inline void iDeallocate(T* buffer)
	{
		cout << __FUNCTION__ << endl;
		::operator delete(buffer);
	}

	template <typename T1, typename T2>
	inline void iConstruct(T1* p, const T2& value)
	{
		cout << __FUNCTION__ << ":" << value << endl;
		new(p) T1(value);
	}

	template <typename T>
	inline void iDestroy(T* ptr)
	{
		cout << __FUNCTION__ << endl;
		ptr->~T();
	}

	template <typename T>
	class iAllocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		template <typename U>
		struct rebind
		{
			typedef iAllocator<U> other;
		};

		pointer allocate(size_type n, const void* hint = 0)
		{
			cout << __FUNCTION__ << ':' << n << endl;
			return iAllocate(difference_type(n), (pointer)0);
		}
		void deallocate(pointer p, size_type n)
		{
			cout << __FUNCTION__ << ':' << endl;
			iDeallocate(p);
		}
		void construct(pointer p, const T& value)
		{
			cout << __FUNCTION__ << endl;
			iConstruct(p, value);
		}
		void destroy(pointer p)
		{
			cout << __FUNCTION__ << endl;
			iDestroy(p);
		}

		pointer address(reference x)
		{
			cout << __FUNCTION__ << endl;
			return (pointer)&x;
		}

		const_pointer const_address(const_reference x)
		{
			cout << __FUNCTION__ << endl;
			return (const_pointer)&x;
		}
		size_type max_size() const
		{
			cout << __FUNCTION__ << endl;
			return size_type(UINT_MAX / sizeof(T));
		}
	};
}

#endif