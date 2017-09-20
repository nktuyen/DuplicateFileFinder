#pragma once

#include "FileFolderFilter.h"


class CAttributesFilter : public CFileFolderFilter
{
public:
	CAttributesFilter(LPCTSTR lpszName = nullptr, UINT nAttributes = 0x0);
	virtual ~CAttributesFilter(void);
	inline void SetFilterAttributes(UINT nAttrs) { m_nAttributes = nAttrs;}
	inline UINT GetFilterAttributes() { return m_nAttributes;}
protected:
	virtual BOOL OnFilter(const CString& strPath);
	virtual BOOL OnFilter(HANDLE handle);
protected:
	UINT m_nAttributes;
};

