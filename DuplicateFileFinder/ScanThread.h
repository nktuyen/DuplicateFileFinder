#pragma once

#include "PatternFilter.h"
#include "SizeFilter.h"
#include "AttributesFilter.h"
#include <cstdint>

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
	void SetDuplicateMask(UINT nMask) { m_nDuplicateMask = nMask; }
private:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	void EnableAllControls(BOOL bEnabled);
	void RestoreAllControlsEnableState();
	void WalkDir(const CString& strPath, BOOL bRecursive, __int64& iTotalSize);
	BOOL OnEnumChild(HWND hChild);
	BOOL IsExcludeFile(const CString& strPath);
	UINT FindDuplicate(const CString& strPath, CMapStringToString& arrDuplicate, struct _SDuplicateInfo* pDuplicateInfo, CMapStringToString* pTotalDuplicateFiles = nullptr);
	static BOOL CALLBACK EnumChildProc(HWND   hwnd, LPARAM lParam);
	void crc8PushByte(uint8_t *crc, uint8_t ch);
	uint8_t CRC(uint8_t *pcrc, uint8_t *block, uint16_t count);
	bool CRCFile(const CString& strPath,CArray<uint16_t>& vecCRC, CArray<uint16_t>* vecCRCCmp = nullptr, struct _SDuplicateInfo* pDuplicateInfo = nullptr);
	__int64 SizeOfFile(const CString& strPath);
private:
	CWorkerDialog* m_pProgressDlg;
	CStringArray m_arrScannedFiles;
	CMapStringToString m_arrDuplicateChecks;
	UINT m_nDuplicateMask;
	CMapStringToString m_arrScannedFolder;
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
	CMap<HWND, HWND, BOOL, BOOL> m_Enables;
	CCriticalSection m_CriticalSection;
};


