/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/******************************************************************************
Module:  BufferPool.h
Notices: Copyright (c) 2013 Bruce Liang
Purpose: �򵥵��ڴ滺���
Desc:
******************************************************************************/

#pragma once

#include "RWLock.h"
#include "Singleton.h"
#include "STLHelper.h"
#include "PrivateHeap.h"
#include "CriticalSection.h"

struct TItem
{
	template<typename T> 
	friend struct	TSimpleList;
	friend struct	TItemList;
	friend class	CItemPool;
	friend struct	TBuffer;

public:
	inline int Cat		(const BYTE* pData, int length);
	inline int Cat		(const TItem& other);
	inline int Fetch	(BYTE* pData, int length);
	inline int Reduce	(int length);
	inline void	Reset	(int first = 0, int last = 0);

	BYTE*		Ptr		()			{return begin;}
	const BYTE*	Ptr		()	const	{return begin;}
	int			Size	()	const	{return (int)(end - begin);}
	int			Remain	()	const	{return capacity - (int)(end - head);}
	int			Capacity()	const	{return capacity;}
	bool		IsEmpty	()	const	{return Size()	 == 0;}
	bool		IsFull	()	const	{return Remain() == 0;}

public:
	operator		BYTE*	()			{return Ptr();}
	operator const	BYTE*	() const	{return Ptr();}

public:
	static TItem* Construct(CPrivateHeap& heap,
							int		capacity	= DEFAULT_ITEM_CAPACITY,
							BYTE*	pData		= nullptr,
							int		length		= 0);

	static void Destruct(TItem* pItem);

private:
	TItem(CPrivateHeap& hp, int cap = DEFAULT_ITEM_CAPACITY, BYTE* pData = nullptr, int length = 0)
	: heap(hp), capacity(cap), begin(head), end(head), next(nullptr), last(nullptr)
	{
		if(pData != nullptr && length != 0)
			Cat(pData, length);
	}

	~TItem() {}

	DECLARE_NO_COPY_CLASS(TItem)

public:
	static const DWORD DEFAULT_ITEM_CAPACITY;

private:
	CPrivateHeap& heap;

private:
	TItem* next;
	TItem* last;

	BYTE*	head;
	BYTE*	begin;
	BYTE*	end;
	int		capacity;
};

template<class T> struct TSimpleList
{
public:
	T* PushFront(T* pItem)
	{
		if(pFront != nullptr)
		{
			pFront->last = pItem;
			pItem->next	 = pFront;
		}
		else
		{
			pItem->last = nullptr;
			pItem->next = nullptr;
			pBack		= pItem;
		}

		pFront = pItem;
		++size;

		return pItem;
	}

	T* PushBack(T* pItem)
	{
		if(pBack != nullptr)
		{
			pBack->next	= pItem;
			pItem->last	= pBack;
		}
		else
		{
			pItem->last = nullptr;
			pItem->next = nullptr;
			pFront		= pItem;
		}

		pBack = pItem;
		++size;

		return pItem;
	}

	T* PopFront()
	{
		T* pItem = pFront;

		if(pFront != pBack)
		{
			pFront = pFront->next;
			pFront->last = nullptr;
		}
		else if(pFront != nullptr)
		{
			pFront	= nullptr;
			pBack	= nullptr;
		}

		if(pItem != nullptr)
		{
			pItem->next = nullptr;
			pItem->last = nullptr;

			--size;
		}

		return pItem;
	}

	T* PopBack()
	{
		T* pItem = pBack;

		if(pFront != pBack)
		{
			pBack = pBack->last;
			pBack->next	= nullptr;
		}
		else if(pBack != nullptr)
		{
			pFront	= nullptr;
			pBack	= nullptr;
		}

		if(pItem != nullptr)
		{
			pItem->next = nullptr;
			pItem->last = nullptr;

			--size;
		}

		return pItem;
	}

	TSimpleList<T>& Shift(TSimpleList<T>& other)
	{
		if(other.size > 0)
		{
			if(size > 0)
			{
				pBack->next = other.pFront;
				other.pFront->last = pBack;
			}
			else
			{
				pFront = other.pFront;
			}

			pBack	 = other.pBack;
			size	+= other.size;

			other.Reset();
		}

		return *this;
	}

	void Clear()
	{
		if(size > 0)
		{
			T* pItem;
			while((pItem = PopFront()) != nullptr)
				T::Destruct(pItem);
		}
	}

	T*		Front	()	const	{return pFront;}
	T*		Back	()	const	{return pBack;}
	int		Size	()	const	{return size;}
	bool	IsEmpty	()	const	{return size == 0;}

public:
	TSimpleList()	{Reset();}
	~TSimpleList()	{Clear();}

	DECLARE_NO_COPY_CLASS(TSimpleList<T>)

private:
	void Reset()
	{
		pFront	= nullptr;
		pBack	= nullptr;
		size	= 0;
	}

private:
	T*	pFront;
	T*	pBack;
	int	size;
};

struct TItemList : public TSimpleList<TItem>
{
public:
	void Cat	(const BYTE* pData, int length);
	void Cat	(const TItem* pItem);
	void Cat	(const TItemList& other);
	int Fetch	(BYTE* pData, int length);
	int Reduce	(int length);

public:
	TItemList(CItemPool& pool) : itPool(pool)
	{
		
	}

private:
	CItemPool& itPool;
};

class CItemPool
{
public:
	void PutFreeItem	(TItem* pItem);
	void PutFreeItem	(TItemList& lsItem);
	TItem* PickFreeItem	();

	inline void Clear();

private:
	void CompressFreeItem(int size);

public:
	void SetItemCapacity(DWORD dwItemCapacity)	{m_dwItemCapacity	= dwItemCapacity;}
	void SetPoolSize	(DWORD dwPoolSize)		{m_dwPoolSize		= dwPoolSize;}
	void SetPoolHold	(DWORD dwPoolHold)		{m_dwPoolHold		= dwPoolHold;}
	DWORD GetItemCapacity	()					{return m_dwItemCapacity;}
	DWORD GetPoolSize		()					{return m_dwPoolSize;}
	DWORD GetPoolHold		()					{return m_dwPoolHold;}

public:
	CItemPool(	DWORD dwPoolSize	 = DEFAULT_POOL_SIZE,
				DWORD dwPoolHold	 = DEFAULT_POOL_HOLD,
				DWORD dwItemCapacity = DEFAULT_ITEM_CAPACITY)
	: m_dwPoolSize(dwPoolSize)
	, m_dwPoolHold(dwPoolHold)
	, m_dwItemCapacity(dwItemCapacity)
	, m_lsFreeItem(*this)
	{
	}

	~CItemPool()	{Clear();}

	DECLARE_NO_COPY_CLASS(CItemPool)

public:
	static const DWORD DEFAULT_ITEM_CAPACITY;
	static const DWORD DEFAULT_POOL_SIZE;
	static const DWORD DEFAULT_POOL_HOLD;

private:
	CPrivateHeap	m_heap;

	CCriSec			m_csFreeItem;
	TItemList		m_lsFreeItem;

	DWORD m_dwItemCapacity;
	DWORD m_dwPoolSize;
	DWORD m_dwPoolHold;
};

struct TItemPtr
{
public:
	TItem* Reset(TItem* pItem = nullptr)
	{
		if(m_pItem != nullptr)
			itPool.PutFreeItem(m_pItem);

		m_pItem = pItem;

		return m_pItem;
	}

	TItem* Attach(TItem* pItem)
	{
		return Reset(pItem);
	}

	TItem* Detach()
	{
		TItem* pItem = m_pItem;
		m_pItem		 = nullptr;

		return pItem;
	}

	bool IsValid			()				{return m_pItem != nullptr;}
	TItem* operator ->		()				{return m_pItem;}
	TItem* operator =		(TItem* pItem)	{return Reset(pItem);}
	operator TItem*			()				{return m_pItem;}
	TItem* Ptr				()				{return m_pItem;}
	const TItem* Ptr		()	const		{return m_pItem;}
	operator const TItem*	()	const		{return m_pItem;}

public:
	TItemPtr(CItemPool& pool, TItem* pItem = nullptr)
	: itPool(pool), m_pItem(pItem)
	{

	}

	~TItemPtr()
	{
		Reset();
	}

	DECLARE_NO_COPY_CLASS(TItemPtr)

private:
	CItemPool&	itPool;
	TItem*		m_pItem;
};

struct TBuffer
{
	template<typename T> friend struct TSimpleList;
	friend class CBufferPool;

public:
	static TBuffer* Construct(CBufferPool& pool, ULONG_PTR dwID);
	static void Destruct(TBuffer* pBuffer);

public:
	int Cat		(const BYTE* pData, int len);
	int Cat		(const TItem* pItem);
	int Cat		(const TItemList& other);
	int Fetch	(BYTE* pData, int length);
	int Reduce	(int len);

public:
	CCriSec&	CriSec	()	{return cs;}
	TItemList&	ItemList()	{return items;}

	ULONG_PTR ID		()	const	{return id;}
	int Length			()	const	{return length;}
	bool IsValid		()	const	{return id != 0;}

private:
	int IncreaseLength	(int len)	{return (length += len);}
	int DecreaseLength	(int len)	{return (length -= len);}

	inline void Reset	();

private:
	TBuffer(CPrivateHeap& hp, CItemPool& itPool, ULONG_PTR dwID = 0)
	: heap(hp), items(itPool), id(dwID), length(0)
	{
	}

	~TBuffer()	{}

	DECLARE_NO_COPY_CLASS(TBuffer)

private:
	CPrivateHeap&	heap;

private:
	ULONG_PTR		id;
	int				length;
	DWORD			freeTime;
	CCriSec			cs;

	TItemList		items;

private:
	TBuffer*		next;
	TBuffer*		last;
};

typedef TSimpleList<TBuffer>			TBufferList;

typedef hash_map<ULONG_PTR, TBuffer*>	TBufferPtrMap;
typedef TBufferPtrMap::iterator			TBufferPtrMapI;
typedef TBufferPtrMap::const_iterator	TBufferPtrMapCI;

class CBufferPool
{
public:
	void		PutFreeBuffer	(ULONG_PTR dwID);
	void		PutCacheBuffer	(ULONG_PTR dwID);
	TBuffer*	PickFreeBuffer	(ULONG_PTR dwID);
	TBuffer*	FindCacheBuffer	(ULONG_PTR dwID);

	void		Clear			();

private:
	void PutFreeBuffer		(TBuffer* pBuffer);
	void CompressFreeBuffer	(int size);

public:
	void SetItemCapacity	(DWORD dwItemCapacity)		{m_itPool.SetItemCapacity(dwItemCapacity);}
	void SetItemPoolSize	(DWORD dwItemPoolSize)		{m_itPool.SetPoolSize(dwItemPoolSize);}
	void SetItemPoolHold	(DWORD dwItemPoolHold)		{m_itPool.SetPoolHold(dwItemPoolHold);}

	void SetBufferLockTime	(DWORD dwBufferLockTime)	{m_dwBufferLockTime	= dwBufferLockTime;}
	void SetBufferPoolSize	(DWORD dwBufferPoolSize)	{m_dwBufferPoolSize	= dwBufferPoolSize;}
	void SetBufferPoolHold	(DWORD dwBufferPoolHold)	{m_dwBufferPoolHold	= dwBufferPoolHold;}

	DWORD GetItemCapacity	()							{return m_itPool.GetItemCapacity();}
	DWORD GetItemPoolSize	()							{return m_itPool.GetPoolSize();}
	DWORD GetItemPoolHold	()							{return m_itPool.GetPoolHold();}

	DWORD GetBufferLockTime	()							{return m_dwBufferLockTime;}
	DWORD GetBufferPoolSize	()							{return m_dwBufferPoolSize;}
	DWORD GetBufferPoolHold	()							{return m_dwBufferPoolHold;}

	TBuffer* operator []	(ULONG_PTR dwID)			{return FindCacheBuffer(dwID);}

public:
	CBufferPool(DWORD dwPoolSize	 = DEFAULT_BUFFER_POOL_SIZE,
				DWORD dwPoolHold	 = DEFAULT_BUFFER_POOL_HOLD,
				DWORD dwLockTime	 = DEFAULT_BUFFER_LOCK_TIME,
				DWORD dwItemCapacity = DEFAULT_ITEM_CAPACITY)
	: m_dwBufferPoolSize(dwPoolSize)
	, m_dwBufferPoolHold(dwPoolHold)
	, m_dwBufferLockTime(dwLockTime)
	{
		m_itPool.SetItemCapacity(dwItemCapacity);
	}

	~CBufferPool()	{Clear();}

	DECLARE_NO_COPY_CLASS(CBufferPool)

public:
	CPrivateHeap&	GetPrivateHeap()	{return m_heap;}
	CItemPool&		GetItemPool()		{return m_itPool;}

public:
	static const DWORD DEFAULT_ITEM_CAPACITY;
	static const DWORD DEFAULT_ITEM_POOL_SIZE;
	static const DWORD DEFAULT_ITEM_POOL_HOLD;
	static const DWORD DEFAULT_BUFFER_LOCK_TIME;
	static const DWORD DEFAULT_BUFFER_POOL_SIZE;
	static const DWORD DEFAULT_BUFFER_POOL_HOLD;

private:
	CPrivateHeap	m_heap;

	CItemPool		m_itPool;

	CCriSec			m_csFreeBuffer;
	CRWLock			m_csBufferMap;

	TBufferPtrMap	m_mpBuffer;
	TBufferList		m_lsFreeBuffer;

	DWORD m_dwBufferLockTime;
	DWORD m_dwBufferPoolSize;
	DWORD m_dwBufferPoolHold;
};
