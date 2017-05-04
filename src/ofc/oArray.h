///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef __OARRAY_H__
#define __OARRAY_H__

#include <afxtempl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// CoArray オブジェクト

template<class TYPE, class ARG_TYPE = TYPE&>
class CoArray : public CArray<TYPE, ARG_TYPE>
{
public:
	operator CoArray*()
	{
		return this;
	}
	CoArray& operator=(const CoArray& other)
	{
		Copy(other);
		return *this;
	}
	CoArray& operator+=(const CoArray& other)
	{
		Append(other);
		return *this;
	}
	int Find(const TYPE& target, const int start = 0) const
	{
		for (INT i = start, count = (INT)GetSize(); i < count; i++)
		{
			if ((*(CoArray*)this).ElementAt(i) == target)
			{
				return i;
			}
		}
		return -1;
	}
	int FindNoCase(const TYPE& target, const int start = 0) const
	{
		for (INT i = start, count = GetSize(); i < count; i++)
		{
			if ((*(CoArray*)this).ElementAt(i).FindNoCase(target) == 0)
			{
				return i;
			}
		}
		return -1;
	}
	int RemoveFind(const TYPE& target, const int start = 0)
	{
		int result = 0;
		for (int find = -1; (find = Find(target, start)) != -1; )
		{
			RemoveAt(find);
			result++;
		}
		return result;
	}
	int Uniq()
	{
		int result = 0;
		for (int i = 0; i < GetSize() - 1; i++)
		{
			result += RemoveFind(ElementAt(i), i + 1);
		}
		return result;
	}
	typedef void (* PROC)(ARG_TYPE target);
	void Each(PROC proc)
	{
		for (int i = 0, count GetSize(); i < count; i++)
		{
			proc(ElementAt(i));
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// CoCountNest オブジェクト

class CoCountNest
{
protected:
	INT& m_currentNest;
	INT& m_maxNest;
public:
	CoCountNest(INT& currentNest, INT& maxNest)
		: m_currentNest(currentNest)
		, m_maxNest(maxNest)
	{
		m_currentNest++;
		if (m_maxNest < m_currentNest)
		{
			m_maxNest = m_currentNest;
		}
	}
	virtual ~CoCountNest()
	{
		m_currentNest--;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// CoAlgorithm オブジェクト

#include <mmsystem.h>
#pragma comment(lib, "winmm")

template<class ARRAY_TYPE, class TYPE, class ARG_TYPE = TYPE&>
class CoAlgorithm
{
protected:
	INT m_swapCount;
	INT m_currentNest;
	INT m_maxNest;
	DWORD m_time;
public:
	CoAlgorithm()
		: m_swapCount(0)
		, m_currentNest(0)
		, m_maxNest(0)
		, m_time(0)
	{
	}
	virtual ~CoAlgorithm()
	{
		CoString text;
		text.Format("swapCount %5d maxNest %5d time %.2f\n", m_swapCount, m_maxNest, (DOUBLE)m_time / 1000);
		// OutputDebugString(text);
	}

protected:
	void Swap(ARRAY_TYPE& object, INT a, INT b)
	{
		m_swapCount++;

		TYPE tmp = object[a];
		object[a] = object[b];
		object[b] = tmp;
	}

	INT Partition(ARRAY_TYPE& object, INT left, INT right)
	{
		Swap(object, left, INT((left + right) / 2));
		INT last = left;
		for (INT i = left + 1; i <= right; i++)
		{
			if (object[i] < object[left])
			{
				last++;
				Swap(object, last, i);
			}
		}
		Swap(object, left, last);
		return last;
	}

public:
	// ソート処理
	void Sort(ARRAY_TYPE& object, INT base = 0, INT n = INFINITE)
	{
		if (n == INFINITE)
		{
			n = object.GetSize();
		}
		DWORD start = timeGetTime();

		AdvancedQuickSort(object, base, n);
		// SimpleQuickSort(object, base, n - 1);

		m_time = timeGetTime() - start;
	}
	// シンプルなクイックソート
	void SimpleQuickSort(ARRAY_TYPE& object, INT left, INT right)
	{
		CoCountNest nest(m_currentNest, m_maxNest);

		if (left < right)
		{
			INT p = Partition(object, left, right);
			SimpleQuickSort(object, p + 1, right);
			SimpleQuickSort(object, left, p - 1);
		}
	}
	// 高性能なクイックソート
	void AdvancedQuickSort(ARRAY_TYPE& object, INT base = 0, INT n = INFINITE)
	{
		CoCountNest nest(m_currentNest, m_maxNest);

		if (n == INFINITE)
		{
			n = object.GetSize();
		}

		if (n <= 1)
		{
			return;
		}

		INT p = base + 1;
		INT x = 0;

		if (n == 2) // 最後の2個
		{
			if (object[base] > object[p])
			{
				Swap(object, base, p);
			}
			return;
		}

		Swap(object, base, base + (n / 2));

		INT q = base;
		for (INT i = 1; i < n; i++)
		{
			if (object[p] < object[base])
			{
				x++;
				q += 1;
				if (p != q)
				{
					Swap(object, p, q);
				}
			}
			p += 1;
		}

		if (q != base)
		{
			Swap(object, base, q);
		}

		AdvancedQuickSort(object, base, x);
		AdvancedQuickSort(object, q + 1, n - x - 1);
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// CoConstSortArray オブジェクト

// 常にソートされている状態を保つ配列
template<class TYPE, class ARG_TYPE = TYPE&>
class CoConstSortArray : public CoArray<TYPE, ARG_TYPE>
{
public:
	CoConstSortArray()
	{
	}
	INT Find(ARG_TYPE target) const
	{
		return FindCore(target, 0, (INT)GetSize() - 1);
	}
	INT Add(ARG_TYPE newElement)
	{
		INT nIndex = FindCore(newElement, 0, (INT)GetSize() - 1, TRUE);
		InsertAt(nIndex, newElement);
		return nIndex;
	}

protected:
	INT FindCore(ARG_TYPE target, INT nStart, INT nEnd, BOOL maybe = FALSE) const
	{
		if (nStart > nEnd)
		{
			if (maybe) return nStart;
			return -1;
		}
		INT nPos = nStart + ((nEnd - nStart) / 2);
		if ((*(CoConstSortArray*)this).ElementAt(nPos) > target)
		{
			return FindCore(target, nStart, nPos - 1, maybe);
		}
		else if ((*(CoConstSortArray*)this).ElementAt(nPos) < target)
		{
			return FindCore(target, nPos + 1, nEnd, maybe);
		}
		else if ((*(CoConstSortArray*)this).ElementAt(nPos) == target)
		{
			return nPos;
		}
		ASSERT(FALSE);
		return -1;
	}
	void InsertAt(INT nIndex, ARG_TYPE newElement, INT nCount = 1)
	{
		CoArray<TYPE, ARG_TYPE>::InsertAt(nIndex, newElement, nCount);
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// CoSortArray オブジェクト

template<class TYPE, class ARG_TYPE = TYPE&>
class CoSortArray : public CoArray<TYPE, ARG_TYPE>
{
public:
	CoSortArray()
	{
	}
	virtual BOOL Sort(DWORD dwFlags = 0)
	{
		if (dwFlags == 0)
		{
			CoAlgorithm<CoSortArray, TYPE> q;
			q.Sort(*this);
		}

		return TRUE;
	}

};

/////////////////////////////////////////////////////////////////////////////
// CoSortList オブジェクト

template<class TYPE, class ARG_TYPE = TYPE&>
class CoSortList : public CList<TYPE, ARG_TYPE>
{
public:
	virtual BOOL Sort(DWORD dwFlags = 0)
	{
		if (dwFlags == 0)
		{
			CoAlgorithm<CoSortList, TYPE> q;
			q.Sort(*this);
		}

		return TRUE;
	}
	virtual ARG_TYPE operator[](INT nIndex) = 0;
	virtual DWORD GetSize()
	{
		return GetCount();
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// プロセス制御クラス

class CoProcess
{
protected:
	HANDLE m_hProcess;
public:
	CoProcess(DWORD dwProcess)
		: m_hProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess))
	{
	}
	virtual ~CoProcess()
	{
		CloseHandle(m_hProcess);
	}
	operator HANDLE()
	{
		return m_hProcess;
	}
	BOOL ReadProcessMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead = NULL)
	{
		return ::ReadProcessMemory(m_hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
	}
	BOOL WriteProcessMemory(LPVOID lpBaseAddress, LPVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesWritten = NULL)
	{
		return ::WriteProcessMemory(m_hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __OARRAY_H__
