#pragma once

#include "PatternFilter.h"
#include "SizeFilter.h"
#include "AttributesFilter.h"


class CWorkerDialog;

class CScanThread : public CWinThread
{
public:
	CScanThread(CWnd* pOwner = nullptr, CWorkerDialog* pProgressDlg = nullptr);
	virtual ~CScanThread();
	BOOL Initialize(const CStringArray& strPath, BOOL bRecursive = FALSE);
	void Finalize();
	void RemoveAllFilters();
	void AddFilter(CFileFolderFilter* pFilter) { m_Filters.SetAt(pFilter->GetCriteria(), pFilter); }
private:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	void WalkDir(const CString& strPath);
private:
	CWorkerDialog* m_pProgressDlg;
	CWnd*	m_pOwnerWnd;
	static CScanThread* m_sInstance;
	CMap<EFileFolderFilterCriteria,EFileFolderFilterCriteria,CFileFolderFilter*,CFileFolderFilter*> m_Filters;
	CStringArray m_arrPaths;
	BOOL m_bRunning;
	CRITICAL_SECTION  m_RunSection;
	BOOL m_bRecursive;
};


