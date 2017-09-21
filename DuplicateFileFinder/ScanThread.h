#pragma once

#include "PatternFilter.h"
#include "SizeFilter.h"
#include "AttributesFilter.h"
#include <cstdint>
#include "CommonDef.h"
#include "FileInformation.h"

class CWorkerDialog;

class CScanThread : public CWinThread
{
	enum EEnumChildFlag {EEnumChildSetEnable, EEnumChildRestoreEnable};
public:
	CScanThread(CWnd* pOwner = nullptr, CWorkerDialog* pProgressDlg = nullptr, UINT nMessageID = 0U, CCriticalSection* pCriticlSection = nullptr);
	virtual ~CScanThread();
	BOOL Initialize(const CStringArray& strPath, BOOL bRecursive = FALSE);
	void Finalize();
	void DisableLock();
	void EnableLock();
	void RemoveAllFilters();
	void AddFilter(CFileFolderFilter* pFilter) { m_ExcludeFilters.SetAt(pFilter->GetCriteria(), pFilter); }
	void SetDuplicateMask(UINT nMask) { m_nDuplicateMask = nMask; }
	void SetBufferSize(UINT nSize) { m_nBufSize = nSize;}
private:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	void EnableAllControls(BOOL bEnabled);
	void RestoreAllControlsEnableState();
	void WalkDir(const CString& strPath, BOOL bRecursive, __int64& iTotalSize);
	BOOL OnEnumChild(HWND hChild);
	BOOL IsExcludeFile(const CString& strPath);
	UINT IsDuplicateFile(const CString& strPath, CMapStringToString* arrIdenticalFiles, CFileInformation* pDuplicateInfo = nullptr);
	static BOOL CALLBACK EnumChildProc(HWND   hwnd, LPARAM lParam);
	void RemoveAllScannedFiles();
	void RemoveIdenticalFiles(CMapStringToString* pIdenticalFiles);
private:
	CWorkerDialog* m_pProgressDlg;
	UINT m_nDuplicateMask;
	CMapStringToPtr		m_arrScannedFiles;
	CMapStringToString	m_arrFilesToCheck;
	CMapStringToString	m_arrScannedFolder;
	CWnd*	m_pOwnerWnd;
	UINT m_nMessageID;
	CString	m_strCurrentFile;
	static CScanThread* m_sInstance;
	CMap<EFileFolderFilterCriteria,EFileFolderFilterCriteria,CFileFolderFilter*,CFileFolderFilter*> m_ExcludeFilters;
	CStringArray m_arrPaths;
	BOOL m_bRunning;
	BOOL m_bRecursive;
	EEnumChildFlag m_EnumFlag;
	BOOL m_bEnumVal;
	UINT m_nBufSize;
	CMap<HWND, HWND, BOOL, BOOL> m_Enables;
	CCriticalSection* m_pCriticalSection;
	CCriticalSection* m_pOriginCriticalSection;
};


