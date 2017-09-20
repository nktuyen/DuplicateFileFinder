#pragma once

#include "PatternFilter.h"
#include "SizeFilter.h"
#include "AttributesFilter.h"


class CWorkerDialog;

class CScanThread : public CWinThread
{
	enum EEnumChildFlag {EEnumChildSetEnable, EEnumChildRestoreEnable};
public:
	CScanThread(CWnd* pOwner = nullptr, CWorkerDialog* pProgressDlg = nullptr, UINT nMessageID = 0U);
	virtual ~CScanThread();
	BOOL Initialize(const CStringArray& strPath, BOOL bRecursive = FALSE);
	void Finalize();
	void RemoveAllFilters();
	void AddFilter(CFileFolderFilter* pFilter) { m_ExcludeFilters.SetAt(pFilter->GetCriteria(), pFilter); }
private:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	void EnableAllControls(BOOL bEnabled);
	void RestoreAllControlsEnableState();
	void WalkDir(const CString& strPath, BOOL bRecursive);
	BOOL OnEnumChild(HWND hChild);
	BOOL IsExcludeFile(const CString& strPath);
	static BOOL CALLBACK EnumChildProc(HWND   hwnd, LPARAM lParam);
private:
	CWorkerDialog* m_pProgressDlg;
	CStringArray m_arrFoundPaths;
	CWnd*	m_pOwnerWnd;
	UINT m_nMessageID;
	CString	m_strCurrentFile;
	static CScanThread* m_sInstance;
	CMap<EFileFolderFilterCriteria,EFileFolderFilterCriteria,CFileFolderFilter*,CFileFolderFilter*> m_ExcludeFilters;
	CStringArray m_arrPaths;
	BOOL m_bRunning;
	CRITICAL_SECTION  m_RunSection;
	BOOL m_bRecursive;
	EEnumChildFlag m_EnumFlag;
	BOOL m_bEnumVal;
	CMap<HWND, HWND, BOOL, BOOL> m_Enables;
};


