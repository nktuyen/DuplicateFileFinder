#pragma once

#include "FileFolderFilter.h"

enum ESizeFilterCriteria
{
	ESizeLessThan,
	ESizeEqual,
	ESizeGreaterThan
};

enum ESizeUnit
{
	eByte,
	eKiloByte,
	eMegaByte,
	eGigaByte,
	eTeraByte,
	ePetaByte
};

class CSizeFilter : public CFileFolderFilter
{
public:
	CSizeFilter(LPCTSTR lpszName = nullptr, ESizeFilterCriteria eSizeCriteria = ESizeLessThan, __int64 nSize = 0, ESizeUnit eUnit = eByte);
	virtual ~CSizeFilter(void);
public:
	inline void SetFilterCriteria(ESizeFilterCriteria eCSizeCriteria) {m_eFilterCriteria = eCSizeCriteria;}
	inline void SetFilterSize(__int64 nSize) { m_nSize = nSize;}
	inline void SetFilterUnit(ESizeUnit eUnit) { m_eUnit = eUnit;}
	inline ESizeFilterCriteria GetFilterCriteria() { return m_eFilterCriteria;}
	inline __int64 getFilterSize() { return m_nSize;}
	inline ESizeUnit GetFilterUnit() { return m_eUnit; }
protected:
	virtual BOOL OnFilter(const CString& strPath);
	virtual BOOL OnFilter(HANDLE handle);
protected:
	ESizeFilterCriteria m_eFilterCriteria;
	__int64 m_nSize;
	ESizeUnit m_eUnit;
};

