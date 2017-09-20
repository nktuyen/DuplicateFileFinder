#include "stdafx.h"
#include "PatternFilter.h"


CPatternFilter::CPatternFilter(LPCTSTR lpszName /* = nullptr */, LPCTSTR lpszPatterns /* = nullptr */)
	: CFileFolderFilter(EFilter_Pattern, lpszName)
{
	m_strPatterns = lpszPatterns;
}


CPatternFilter::~CPatternFilter(void)
{
}

BOOL CPatternFilter::OnFilter(const CString& strPath)
{
	if(m_strPatterns.IsEmpty() || m_strPatterns.CompareNoCase(_T("*"))) {
		return TRUE;
	}

	if(strPath.GetLength() < m_strPatterns.GetLength()) 
		return FALSE;

	CString	strSub;
	for(int n=0;n<strPath.GetLength()-m_strPatterns.GetLength();n++){
		strSub = strPath.Mid(n, m_strPatterns.GetLength());
		if(strSub.CompareNoCase(m_strPatterns) == 0)
			return TRUE;
	}

	return FALSE;
}