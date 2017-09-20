#include "stdafx.h"
#include "AttributesFilter.h"


CAttributesFilter::CAttributesFilter(LPCTSTR lpszName /* = nullptr */, UINT nAttributes /* = 0x0 */)
	: CFileFolderFilter(EFilter_Attributes, lpszName)
{
	m_nAttributes = nAttributes;
}


CAttributesFilter::~CAttributesFilter(void)
{
}

BOOL CAttributesFilter::OnFilter(const CString& strPath)
{
	DWORD dwAttrs = GetFileAttributes(strPath);

	if((dwAttrs & m_nAttributes) != 0)
		return TRUE;

	return FALSE;
}


BOOL CAttributesFilter::OnFilter(HANDLE handle)
{
	return FALSE;
}