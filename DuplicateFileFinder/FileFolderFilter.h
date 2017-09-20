#pragma once

enum EFileFolderFilterCriteria
{
	EFilter_Pattern,
	EFilter_Size,
	EFilter_Attributes,
};

class CFileFolderFilter
{
public:
	CFileFolderFilter(EFileFolderFilterCriteria eCriteria, LPCTSTR lpszName = nullptr);
	virtual ~CFileFolderFilter(void);
	BOOL Filter(const CString& strPath) { return this->OnFilter(strPath);}
	BOOL Filter(HANDLE handle) { return this->OnFilter(handle);}

public:
	inline const CString& GetName() { return m_strName;}
	inline EFileFolderFilterCriteria GetCriteria() { return m_eCriteria; }
protected:
	virtual BOOL OnFilter(const CString& strPath) = 0;
	virtual BOOL OnFilter(HANDLE handle) = 0;
protected:
	CString	m_strName;
	EFileFolderFilterCriteria m_eCriteria;
};

