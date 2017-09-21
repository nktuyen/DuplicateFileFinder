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
	if(m_nAttributes == 0)
		return FALSE;

	DWORD dwSrcAttrs = GetFileAttributes(strPath);
	DWORD dwDstAttrs = (dwSrcAttrs | m_nAttributes);
	
	if(dwDstAttrs == dwSrcAttrs)
		return TRUE;

	return FALSE;
}


BOOL CAttributesFilter::OnFilter(HANDLE handle)
{
	return FALSE;
}