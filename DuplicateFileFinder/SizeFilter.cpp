#include "stdafx.h"
#include "SizeFilter.h"


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
	HANDLE hFile = CreateFile(strPath, GENERIC_ALL, FILE_SHARE_READ, &sa, OPEN_EXISTING, 0, nullptr);
	if(hFile != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER liSize = {0};
		if(GetFileSizeEx(hFile, &liSize)) {
			nSize = static_cast<__int64>(liSize.QuadPart);
		}
		CloseHandle(hFile);
	}

	switch (m_eFilterCriteria)
	{
	case ESizeLessThan:
		if(nSize < m_nSize)
			return TRUE;
		break;
	case ESizeEqual:
		if(nSize == m_nSize)
			return TRUE;
		break;
	case ESizeGreaterThan:
		if(nSize > m_nSize)
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
	__int64 nSize = 0;
	
	if(handle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER liSize = {0};
		if(GetFileSizeEx(handle, &liSize)) {
			nSize = static_cast<__int64>(liSize.QuadPart);
		}
	}

	switch (m_eFilterCriteria)
	{
	case ESizeLessThan:
		if(nSize < m_nSize)
			return TRUE;
		break;
	case ESizeEqual:
		if(nSize == m_nSize)
			return TRUE;
		break;
	case ESizeGreaterThan:
		if(nSize > m_nSize)
			return TRUE;

		break;
	default:
		return FALSE;
		break;
	}

	return FALSE;
}