#include "stdafx.h"
#include "SizeFilter.h"
#include <math.h>

CSizeFilter::CSizeFilter(LPCTSTR lpszName /* = nullptr */, ESizeFilterCriteria eSizeCriteria /* = ESizeLessThan */, __int64 nSize /* = 0 */, ESizeUnit eUnit /* = eByte */)
	: CFileFolderFilter(EFilter_Size, lpszName)
{
	m_eFilterCriteria = eSizeCriteria;
	m_nSize = nSize;
	m_eUnit = eUnit;
}

CSizeFilter::~CSizeFilter(void)
{
}


BOOL CSizeFilter::OnFilter(const CString& strPath)
{
	__int64 nSize = 0;
	SECURITY_ATTRIBUTES sa={0};
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;
	sa.nLength = sizeof(sa);
	HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if(hFile != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER liSize = {0};
		if(GetFileSizeEx(hFile, &liSize)) {
			nSize = static_cast<__int64>(liSize.QuadPart);
		}
		CloseHandle(hFile);
	}
	else
		return FALSE;

	__int64 nFilterSize = m_nSize*static_cast<__int64>( pow(static_cast<double>(1024), static_cast<double>(m_eUnit)) );
	

	switch (m_eFilterCriteria)
	{
	case ESizeLessThan:
		if(nSize < nFilterSize)
			return TRUE;
		break;
	case ESizeEqual:
		if(nSize == nFilterSize)
			return TRUE;
		break;
	case ESizeGreaterThan:
		if(nSize > nFilterSize)
			return TRUE;

		break;
	default:
		return FALSE;
		break;
	}

	return FALSE;
}

BOOL CSizeFilter::OnFilter(HANDLE handle) 
{
	if(handle == INVALID_HANDLE_VALUE)
		return FALSE;

	__int64 nSize = 0;
	
	if(handle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER liSize = {0};
		if(GetFileSizeEx(handle, &liSize)) {
			nSize = static_cast<__int64>(liSize.QuadPart);
		}
	}

	__int64 nFilterSize = m_nSize*static_cast<__int64>( pow((int)1024, static_cast<double>(m_eUnit)) );

	switch (m_eFilterCriteria)
	{
	case ESizeLessThan:
		if(nSize < nFilterSize)
			return TRUE;
		break;
	case ESizeEqual:
		if(nSize == nFilterSize)
			return TRUE;
		break;
	case ESizeGreaterThan:
		if(nSize > nFilterSize)
			return TRUE;

		break;
	default:
		return FALSE;
		break;
	}

	return FALSE;
}