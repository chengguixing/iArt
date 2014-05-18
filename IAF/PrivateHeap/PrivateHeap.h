


#pragma once


class CPrivateHeap
{
public:
	PVOID Alloc(SIZE_T dwSize, DWORD dwFlags = 0)
	{
		return ::HeapAlloc(m_hHeap, dwFlags, dwSize);
	}
	PVOID ReAlloc(PVOID pvMemory, SIZE_T dwSize, DWORD dwFlags = 0)
	{
		return ::HeapReAlloc(m_hHeap, dwFlags, pvMemory, dwSize);
	}
	SIZE_T Size(PVOID pvMemory, DWORD dwFlags = 0)
	{
		return ::HeapSize(m_hHeap, dwFlags, pvMemory);
	}
	BOOL Free(PVOID pvMemory, DWORD dwFlags = 0)
	{
		return ::HeapFree(m_hHeap, dwFlags, pvMemory);
	}
	SIZE_T Compact(DWORD dwFlags = 0)
	{
		return ::HeapCompact(m_hHeap, dwFlags);
	}
	BOOL IsValid()
	{
		return m_hHeap != NULL;
	}
	BOOL Reset()
	{
		if (IsValid())
		{
			::HeapDestroy();
		}
		m_hHeap = ::HeapCreate(m_dwOptions, m_dwInitSize, m_dwMaxSize);
		
		return IsValid();
	}
public:
	CPrivateHeap(DWORD dwOptions = 0, SIZE_T dwInitSize = 0, SIZE_T dwMaxSize = 0) : m_dwOptions(dwOptions), m_dwInitSize(dwInitSize), m_dwMaxSize(dwMaxSize)
	{
		m_heap = ::HeapCreate(m_dwOptions, m_dwInitSize, m_dwMaxSize);
	}
	~CPrivateHeap()
	{
		if (IsValid())
		{
			::HeapDestroy(m_hHeap);
		}
	}
	operator HANDLE()
	{
		return m_hHeap;
	}
private:
	CPrivateHeap(const CPrivateHeap&);
	CPrivateHeap operator=(const CPrivateHeap&);

private:
	HANDLE m_hHeap;
	DWORD m_dwOptions;
	SIZE_T m_dwInitSize;
	SIZE_T m_dwMaxSize;
};


template <typename T>
class CPrivateHeapBuffer
{
public:
	CPrivateHeapBuffer(CPrivateHeapBuffer& cPrivateHeap,
		SIZE_T dwSize = 0,
		DWORD dwAllocFlags = 0,
		DWORD dwFreeFlags = 0) : m_hpPrivate(hpPrivate), m_dwAllocFlags(dwAllocFlags), m_dwFreeFlags(dwFreeFlags), m_pvMemory(NULL)
	{
		ASSERT(m_hpPrivate.IsValid());
		Alloc(dwSize);
	}
	~CPrivateHeapBuffer()
	{
		Free();
	}
public:
	T* Alloc(SIZE_T dwSize)
	{
		if (IsValid())
		{
			Free();
		}
		return m_pvMemory = (T*)m_hpPrivate.Alloc(dwSize * sizeof(T), m_dwAllocFlags);
	}
	T* ReAlloc(SIZE_T dwSize, DWORD dwFlags = 0)
	{
		return m_pvMemory = (T*)m_hpPrivate.ReAlloc(m_pvMemory, dwSize * sizeof(T), dwFlags);
	}
	SIZE_T Size(DWORD dwFlags = 0)
	{
		return m_hpPrivate.Size(m_pvMemory, dwFlags);
	}
	BOOL Free()
	{
		BOOL isOK = TRUE;

		if (IsValid())
		{
			isOK = m_hpPrivate.Free(m_pvMemory, m_dwFreeFlags);
			m_pvMemory = NULL;
		}
		return isOK;
	}
	BOOL IsValid()
	{
		return m_pvMemory != NULL;
	}
	operator T*() const
	{
		return m_pvMemory;
	}
private:
	CPrivateHeapBuffer(const CPrivateHeapBuffer&);
	CPrivateHeapBuffer operator= (const CPrivateHeapBuffer&);

private:
	CPrivateHeap& m_hpPrivate;
	T* m_pvMemory;
	DWORD m_dwAllocFlags;
	DWORD m_dwFreeFlags;
};


typedef CPrivateHeapBuffer<BYTE>	CPrivateHeapByteBuffer;
typedef CPrivateHeapBuffer<TCHAR>	CPrivateHeapStrBuffer;
