#include "stdafx.h"
#include "FileFolderFilter.h"


CFileFolderFilter::CFileFolderFilter(EFileFolderFilterCriteria eCriteria, LPCTSTR lpszName /* = nullptr */)
{
	m_strName = lpszName;
	m_eCriteria = eCriteria;
}


CFileFolderFilter::~CFileFolderFilter(void)
{
}
