#pragma once

#include "FileFolderFilter.h"
class CPatternFilter : public CFileFolderFilter
{
public:
	CPatternFilter(LPCTSTR lpszName = nullptr, LPCTSTR lpszPatterns = nullptr);
	virtual ~CPatternFilter(void);
	void SetPatterns(const CString& strPatterns);
	inline const CString& GetPattens() { return m_strPatterns;}
protected:
	virtual BOOL OnFilter(const CString& strPath);
	virtual BOOL OnFilter(HANDLE handle) { return TRUE; }
private:
	CString	m_strPatterns;
};

