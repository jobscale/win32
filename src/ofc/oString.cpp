///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#define _OFC_EXPORT_
#include "oString.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

// �}���`�o�C�g�����̂P�o�C�g��
#define IS_BYTE_1(src) _ismbblead(src)
// �}���`�o�C�g�����̂Q�o�C�g��
#define IS_BYTE_2(src) _ismbbtrail(src)

CoString::CoString()
{
}

CoString::CoString(const CoString& other)
{
	*((CString*)this) = (LPCTSTR)other;
}

CoString::CoString(const CString& other)
{
	*((CString*)this) = (LPCTSTR)other;
}

CoString::CoString(LPCTSTR other)
{
	*((CString*)this) = other;
}

CoString::CoString(const CComBSTR& other)
{
	*((CString*)this) = other;
}

CoString::CoString(LPCWSTR other)
{
	*((CString*)this) = other;
}

const CoString& CoString::operator=(const CoString& other)
{
	*((CString*)this) = (LPCTSTR)other;

	return *this;
}

const CoString& CoString::operator=(const CString& other)
{
	*((CString*)this) = (LPCTSTR)other;

	return *this;
}

const CoString& CoString::operator=(LPCTSTR other)
{
	*((CString*)this) = other;

	return *this;
}

const CoString& CoString::operator=(const ATL::CComBSTR& other)
{
	*((CString*)this) = other;

	return *this;
}

const CoString& CoString::operator=(LPCWSTR other)
{
	*((CString*)this) = other;

	return *this;
}

// ������ targetText �̍Ōオ�}���`�o�C�g�����̂Q�o�C�g�ڂȂ�؂���
/*static */CoString CoString::Fix(CoString targetText)
{
	// �}���`�o�C�g�����̂P�o�C�g�ڂƂQ�o�C�g�ڂ͏d�Ȃ��Ă���̂Ő擪���珇�Ԃɂ��ׂČ��Ȃ��Ɣ��f�ł��Ȃ�
	DWORD nBase = targetText.GetLength();
	DWORD nCount;
	for (nCount = nBase; nCount && nCount != ULONG_MAX; nCount--)
	{
		UCHAR asciiCheck = ((LPCSTR)targetText)[nBase - nCount];
		// �}���`�o�C�g�����̂P�o�C�g��
		if (IS_BYTE_1(asciiCheck))
		{
			nCount--;
		}
	}
	if (nCount == ULONG_MAX)
	{
		targetText = targetText.Left(targetText.GetLength() - 1);
	}
	return targetText;
}

// ������ targetText �̐擪 cutLength �o�C�g�{����؂���
/*static */CoString CoString::FirstCut(CoString targetText, DWORD cutLength)
{
	for (; cutLength && cutLength != ULONG_MAX; cutLength--)
	{
		UCHAR* asciiCheck = (UCHAR*)(LPCSTR)targetText;
		// �擪���}���`�o�C�g�����Ȃ�Q�o�C�g����؂���
		if (IS_BYTE_1(asciiCheck[0]) && IS_BYTE_2(asciiCheck[1]))
		{
			targetText = targetText.CString::Right(targetText.GetLength() - 2);
			cutLength--;
		}
		else
		{
			targetText = targetText.CString::Right(targetText.GetLength() - 1);
		}
	}
	return targetText;
}

// ���Ԃ̎擾
CoString CoString::Mid(INT nFirst, INT nCount) const
{
	CoString result = Right(GetLength() - nFirst);
	return result.Left(nCount);
}

// �����擾
CoString CoString::Left(INT nCount) const
{
	return (CoString)Fix(CString::Left(nCount));
}

// �E���擾
CoString CoString::Right(INT nCount) const
{
	return (CoString)FirstCut(*this, GetLength() - nCount);
}

// ������̒u��
INT CoString::Replace(INT nFirst, INT nCount, LPCTSTR lpszNew)
{
	CoString left = CString::Left(nFirst);
	CoString right = CString::Right(GetLength() - (nFirst + nCount));
	CoString::operator=(left + lpszNew + right);
	return 1;
}

// ������̒u��
INT CoString::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	return CString::Replace(lpszOld, lpszNew);
}

// ������̌���
INT CoString::Find(TCHAR ch) const
{
	return CString::Find(ch);
}

// ������̌���
INT CoString::Find(LPCTSTR lpszSub, INT nStart/* = 0*/, CoString limit/* = ""*/) const
{
	ASSERT(AfxIsValidString(lpszSub));

	if (strlen(lpszSub) == 0)
	{
		return -1;
	}

	INT nLength = GetLength();
	if (nStart > nLength)
		return -1;

	INT temp;
	INT limited = limit.IsEmpty() ? GetLength() : (temp = Find(limit, nStart)) == -1 ? GetLength() : (temp + limit.GetLength());
	// find first matching substring
	LPTSTR lpsz = _tcsstr((LPTSTR)operator LPCTSTR() + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	INT result = (INT)(lpsz - operator LPCTSTR());
	return (lpsz == NULL) ? -1 : result < limited ? result : -1;
}

// ����������̓�������
INT CoString::FindPlus(LPCTSTR lpszSub, LPCTSTR lpszSubPlus, INT nStart/* = 0*/, CoString limit/* = ""*/) const
{
	INT result = Find(lpszSub, nStart, limit);
	INT resultPlus = Find(lpszSubPlus, nStart, limit);
	return (UINT(result) < UINT(resultPlus)) ? result : resultPlus;
}

// ���S�Ɉ�v���镶����̌���
INT CoString::FindWord(LPCTSTR lpszSub, INT nStart/* = 0*/, CoString limit/* = ""*/) const
{
	for (INT word = nStart; (word = Find(lpszSub, word, limit)) != -1; )
	{
		INT next;
		if ((next = FindOneOf(", \t\r\n", word)) == (INT)strlen(lpszSub) + word)
		{
			return word;
		}
		word = next;
	}
	return -1;
}

// �L�����N�^�[�̌���
INT CoString::FindOneOf(LPCTSTR lpszCharSet, INT nStart/* = 0*/) const
{
	ASSERT(AfxIsValidString(lpszCharSet));
	LPTSTR lpsz = _tcspbrk((LPTSTR)operator LPCTSTR() + nStart, lpszCharSet);
	return (lpsz == NULL) ? -1 : (INT)(lpsz - operator LPCTSTR());
}

// �r���I�ɃL�����N�^�[�̌���
INT CoString::FindOneOfNot(LPCTSTR lpszCharSet, INT nStart/* = 0*/) const
{
	INT count = GetLength();
	for (INT i = nStart; i < count; i++)
	{
		INT countSet = (INT)strlen(lpszCharSet);
		INT iSet;
		for (iSet = 0; iSet < countSet; iSet++)
		{
			if (operator[](i) == lpszCharSet[iSet])
			{
				break;
			}
		}
		if (!(iSet < countSet))
		{
			return i;
		}
	}

	return -1;
}

// ��납��O���Ɍ���
INT CoString::ReverseFind(TCHAR ch) const
{
	return CString::ReverseFind(ch);
}

// ��납��O���Ɍ���
INT CoString::ReverseFind(LPCTSTR lpszSub, INT nStart/* = -1*/) const
{
	if (nStart == -1)
	{
		nStart = GetLength();
	}
	CoString sub(lpszSub);
	INT length = sub.GetLength();
	for (nStart = (nStart < length) ? 0 : nStart - length; nStart > -1 && sub.Compare(CoString(&operator LPCTSTR()[nStart]).Left(length)); nStart--)
	{
	}
	return nStart;
}

// �啶�����������ӎ����Ȃ�����
INT CoString::FindNoCase(LPCTSTR lpszSub, INT nStart/* = 0*/) const
{
	CoString dummy = *this;
	CoString find = lpszSub;
	dummy.MakeLower();
	find.MakeLower();
	return dummy.Find(find, nStart);
}

// ��납��O���Ƀ_�u���N�H�[�g������
INT CoString::AbstractReversePosition(INT nStart) const
{
	if (nStart == -1)
	{
		nStart = GetLength();
	}

	CoString target = CString::Left(nStart);
	INT result;
	for (; (result = target.ReverseFind('"')) != -1 && IS_BYTE_1(target[result - 1]); )
	{
	}
	if (result == -1)
	{
		return -1;
	}
	target = target.Left(result);
	return target.ReverseFind('\n');
}

// �͂܂�Ă��镶����̎擾�i���g�����j
CoString CoString::AbstractSearchSubstance(INT& start, const CoString ce/* = "\""*/, CoString limit/* = ""*/, BOOL bCSV/* = FALSE*/) const
{
	INT nStart = start;
	INT temp;
	INT limited = limit.IsEmpty() ? GetLength() : (temp = Find(limit, start)) == -1 ? GetLength() : (temp + limit.GetLength());
	// ������̊J�n
	CoString c = ce.Mid(0, 1);
	CoString e = ce.Mid(1, 1);
	if (e.GetLength() == 0)
	{
		e = c;
	}
	start = Find(c, start);
	if (start == -1 || !(start < limited))
	{
		if (!bCSV)
		{
			start = limited;
			// �����̏I��
			return CoString();
		}

		start = nStart + 1;
		INT end = Find("\r\n", start);
		if (end == -1 || !(start < limited))
		{
			start = limited;
			// �����̏I��
			return CoString();
		}
		INT nFirst = start + 1;
		INT nCount = end - start - 2;
		start = end;

		// ��������������
		return CString::Mid(nFirst, nCount);
	}

	// ������̏I��
	for (INT end = start + 1; (end = Find(e, end)) != -1; end++)
	{
		CHAR cCheck = operator[](end + 1);
		if (cCheck == e[0])
		{
			end++;
			continue;
		}
		end++;
		INT nFirst = start + 1;
		INT nCount = end - start - 2;
		start = end;

		// ��������������
		return CString::Mid(nFirst, nCount);
	}

	start = -1;

	// �����̏I��
	return CoString();
}

/////////////////////////////////////////////////////////////////////////////
// CoStringArray �I�u�W�F�N�g

int CoStringArray::Add(LPCTSTR element)
{
	CoString work = element;
	return CoArray::Add(work);
}

int CoStringArray::Find(LPCTSTR element) const
{
	const CoString work = element;

	return CoArray::Find(work);
}

BOOL OFCL_API GetStringValue(CoString& text, const CoStringArray& value, LPCTSTR separator)
{
	text.Empty();
	for (INT i = 0, count = (INT)value.GetSize(); i < count; i++)
	{
		if (i != 0)
		{
			text += separator;
		}
		text += value[i];
	}

	return TRUE;
}

BOOL OFCL_API GetArrayValue(CoStringArray& value, const CoString text, LPCTSTR separator, BOOL isFast/* = FALSE*/)
{
	value.RemoveAll();

	INT sepLength = (INT)strlen(separator);
	INT length = text.GetLength();
	for (INT nBegin = 0; nBegin < length; )
	{
		INT nEnd = text.Find(separator, nBegin);
		if (nEnd == -1)
		{
			nEnd = length;
		}
		INT size = nEnd - nBegin;
		if (size)
		{
			CoString result = isFast ? text.CString::Mid(nBegin, size) : text.Mid(nBegin, size);
			value.Add(result);
		}
		nBegin = nEnd + sepLength;
	}

	return TRUE;
}

BOOL IsFindArrayCore(const CoString& text, const CoStringArray& ignore_value, const BOOL ignoreNoCase = TRUE)
{
	BOOL isFindArray = TRUE;

	for (INT nPos = 0, i = 0, count = (INT)ignore_value.GetSize(); i < count; i++)
	{
		CoString findText = ignore_value[i];
		BOOL isTop = FALSE;
		if (findText[0] == '^')
		{
			isTop = TRUE;
			findText = findText.Right(findText.GetLength() - 1);
		}
		BOOL isBottom = FALSE;
		if (findText[findText.GetLength() - 1] == '$')
		{
			isBottom = TRUE;
			findText = findText.Left(findText.GetLength() - 1);
		}
		nPos = ignoreNoCase ? text.FindNoCase(findText, nPos) : text.Find(findText, nPos);
		if (nPos == -1)
		{
			isFindArray = FALSE;
			break;
		}
		if (isTop && nPos != 0)
		{
			isFindArray = FALSE;
			break;
		}
		if (isBottom && nPos != nPos + findText.GetLength() == text.GetLength())
		{
			isFindArray = FALSE;
			break;
		}
		nPos += (INT)strlen(findText);
	}

	return isFindArray;
}

BOOL OFCL_API IsFindArray(const CoString& text, const CoStringArray& ignore_list, const BOOL ignoreNoCase/* = TRUE*/)
{
	for (INT i = 0, count = (INT)ignore_list.GetSize(); i < count; i++)
	{
		CoStringArray ignore_value;
		GetArrayValue(ignore_value, ignore_list[i], "*");
		if (IsFindArrayCore(text, ignore_value, ignoreNoCase))
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ���X�g����
BOOL OFCL_API IsFindArrayAsArray(const CoStringArray& target, const CoStringArray& source, const INT flag, const BOOL ignoreNoCase/* = TRUE*/)
{
	// flag = 1 target �̒��� source ���ЂƂł��܂܂�Ă���ꍇ�� TRUE
	// flag = 2 target �̒��� source ���S�Ċ܂܂�Ă���ꍇ�� TRUE
	ASSERT(flag == 1 || flag == 2);

	INT find_count = 0;

	for (INT i = 0, count = (INT)source.GetSize(); i < count; i++)
	{
		CoStringArray source_value;
		GetArrayValue(source_value, source[i], "*");
		for (INT i = 0, count = (INT)target.GetSize(); i < count; i++)
		{
			if (IsFindArrayCore(target[i], source_value))
			{
				find_count++;
				break;
			}
		}
		if (flag == 1 && find_count)
		{
			return TRUE;
		}
	}

	return find_count == source.GetSize();
}
