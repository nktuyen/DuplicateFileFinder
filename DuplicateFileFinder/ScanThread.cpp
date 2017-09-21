// ScanThread.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "ScanThread.h"
#include "WorkerDialog.h"
#include "CommonDef.h"

#define BUFFER_SIZE	0xFA00

CScanThread* CScanThread::m_sInstance = nullptr;

CScanThread::CScanThread(CWnd* pOwner /* = nullptr */, CWorkerDialog* pProgressDlg /* = nullptr */, UINT nMessageID /* = 0U */)
{
	CScanThread::m_sInstance = this;
	m_pOwnerWnd = pOwner;
	m_pProgressDlg = pProgressDlg;
	m_arrPaths.RemoveAll();
	m_bRunning = FALSE;
	m_bAutoDelete = FALSE;
	m_nMessageID = nMessageID;
	m_nDuplicateMask = (DUPLICATE_CRITERIA_CONTENT | DUPLICATE_CRITERIA_SIZE);
}

CScanThread::~CScanThread()
{
	RemoveAllFilters();
	RemoveAllScannedFiles();
}

BOOL CScanThread::InitInstance()
{
	return TRUE;
}

int CScanThread::ExitInstance()
{
	CSingleLock locker(&m_CriticalSection);
	locker.Lock();
	m_bRunning = FALSE;
	CloseHandle(m_hThread);
	m_hThread = nullptr;
	RestoreAllControlsEnableState();

	if(m_pOwnerWnd && m_pOwnerWnd->GetSafeHwnd())
		m_pOwnerWnd->SendMessage(m_nMessageID, (WPARAM)TM_EXIT);

	locker.Unlock();

	if(m_pProgressDlg) {
		if(m_pProgressDlg->GetSafeHwnd())
			m_pProgressDlg->SendMessage(WM_CLOSE, 0, 0);
	}
	RemoveAllScannedFiles();
	m_arrPaths.RemoveAll();
	return CWinThread::ExitInstance();
}

void CScanThread::Finalize()
{
	CSingleLock locker(&m_CriticalSection);
	locker.Lock();
	m_bRunning = FALSE;
	locker.Unlock();
}

void CScanThread::RemoveAllScannedFiles() 
{
	if(m_arrScannedFiles.GetCount() > 0) {
		POSITION pos = m_arrScannedFiles.GetStartPosition();
		CString	strPath;
		CFileInformation* pInfo = nullptr;

		while (pos) {
			m_arrScannedFiles.GetNextAssoc(pos, strPath, (void*&)pInfo);
			if(nullptr != pInfo) {
				delete pInfo;
				pInfo = nullptr;
			}
		}
		m_arrScannedFiles.RemoveAll();
	}
}

BOOL CScanThread::Initialize(const CStringArray& strPath, BOOL bRecursive)
{
	CSingleLock locker(&m_CriticalSection);
	locker.Lock();
	m_bRecursive = bRecursive;
	m_arrPaths.Copy(strPath);
	

	if(m_arrPaths.GetCount() <= 0)
		return FALSE;

	if(m_pProgressDlg) {
		m_pProgressDlg->SetProgressRange(0, 100);
		m_pProgressDlg->SetProgressType(CWorkerDialog::eMARQUEE);
		m_pProgressDlg->SetProgressValue(0);
	}

	
	m_bRunning = TRUE;
	locker.Unlock();
	return TRUE;
}

int CScanThread::Run()
{
	RemoveAllScannedFiles();
	m_arrScannedFolder.RemoveAll();
	m_arrFilesToCheck.RemoveAll();
	EnableAllControls(FALSE);

	bool bLocked = false;
	m_pProgressDlg->SetJobName(_T("Scan Files..."));
	m_pProgressDlg->SetMessageTitle(_T("Current File:"));
	CString	strCurrentPath;
	CString	strMessage;
	__int64 iTotalSize = 0;
	CSingleLock locker(&m_CriticalSection);
	for(INT_PTR i=0;i<m_arrPaths.GetCount();i++)
	{
		strCurrentPath = m_arrPaths.GetAt(i);

		locker.Lock();
		if(!m_bRunning) {
			locker.Unlock();
			break;
		}
		locker.Unlock();

		m_arrScannedFolder.SetAt(strCurrentPath, strCurrentPath);
		WalkDir(strCurrentPath, m_bRecursive, iTotalSize);
	}
	
	if(m_arrScannedFiles.GetCount() > 0) {
		if(m_pProgressDlg) {
			m_pProgressDlg->SetProgressType(CWorkerDialog::eFILL);
			m_pProgressDlg->SetProgressValue(0);
			m_pProgressDlg->SetProgressRange(0, 100);
			m_pProgressDlg->SetJobName(_T("Analyze Files..."));
		}

		POSITION pos = m_arrScannedFiles.GetStartPosition();
		CString	strCurPath;
		CFileInformation* pInfo = nullptr;
		CMapStringToString arrIdenticalFiles;
		INT_PTR nCurFile = 0;
		INT_PTR nTotalFiles = m_arrScannedFiles.GetCount();

		while (pos) {
			arrIdenticalFiles.RemoveAll();
			m_arrScannedFiles.GetNextAssoc(pos, strCurPath, (void*&)pInfo);
			m_arrFilesToCheck.RemoveKey(strCurPath);

			if(IsDuplicateFile(strCurPath, arrIdenticalFiles, pInfo)) {

			}
			//
			nCurFile++;
			if(m_pProgressDlg) {
				m_pProgressDlg->SetProgressValue(static_cast<int>( static_cast<double>(nCurFile) / static_cast<double>(nTotalFiles) * 100.0 ));
			}
		}
	}

	RestoreAllControlsEnableState();
	return ExitInstance();
}

void CScanThread::RemoveAllFilters()
{
	if(m_ExcludeFilters.GetCount() > 0) {
		POSITION pos = m_ExcludeFilters.GetStartPosition();
		EFileFolderFilterCriteria eCriteria;
		CFileFolderFilter* pFilter = nullptr;
		m_ExcludeFilters.GetNextAssoc(pos, eCriteria, pFilter);
		if(pFilter != nullptr) {
			delete pFilter;
			pFilter = nullptr;
		}
		m_ExcludeFilters.RemoveAll();
	}
}


void CScanThread::EnableAllControls(BOOL bEnabled)
{
	m_Enables.RemoveAll();

	if(m_pOwnerWnd) {
		m_EnumFlag = EEnumChildSetEnable;
		m_bEnumVal = bEnabled;
		EnumChildWindows(m_pOwnerWnd->GetSafeHwnd(), EnumChildProc, reinterpret_cast<LPARAM>(this));
	}
}

void CScanThread::RestoreAllControlsEnableState()
{
	if(m_pOwnerWnd) {
		m_EnumFlag = EEnumChildRestoreEnable;
		EnumChildWindows(m_pOwnerWnd->GetSafeHwnd(), EnumChildProc, reinterpret_cast<LPARAM>(this));
	}
}

BOOL CALLBACK CScanThread::EnumChildProc(HWND hwnd, LPARAM lParam)
{
	CScanThread* instance = reinterpret_cast<CScanThread*>(lParam);
	if(instance != nullptr)
		return instance->OnEnumChild(hwnd);

	return TRUE;
}

BOOL CScanThread::OnEnumChild(HWND hChild)
{
	if(m_EnumFlag == EEnumChildSetEnable) {
		m_Enables.SetAt(hChild, ::IsWindowEnabled(hChild));
		::EnableWindow(hChild, m_bEnumVal);
	}
	else if(m_EnumFlag == EEnumChildRestoreEnable)
	{
		BOOL bEnabled = TRUE;
		m_Enables.Lookup(hChild, bEnabled);
		::EnableWindow(hChild, bEnabled);
	}

	return TRUE;
}

BOOL CScanThread::IsExcludeFile(const CString& strPath)
{
	if(m_ExcludeFilters.GetCount() <= 0)
		return FALSE;

	POSITION pos = m_ExcludeFilters.GetStartPosition();
	EFileFolderFilterCriteria eCriteria;
	CFileFolderFilter* pFilter = nullptr;
	UINT nCount = 0;
	CSingleLock locker(&m_CriticalSection);

	while (pos)
	{
		locker.Lock();
		if(!m_bRunning) {
			locker.Unlock();
			break;
		}
		locker.Unlock();

		m_ExcludeFilters.GetNextAssoc(pos, eCriteria, pFilter);
		if(pFilter != nullptr) {
			if(pFilter->Filter(strPath))
				nCount++;

			pFilter = nullptr;
		}
	}
	
	return (nCount == m_ExcludeFilters.GetCount());
}

UINT CScanThread::IsDuplicateFile(const CString& strPath, CMapStringToString& arrIdenticalFiles, CFileInformation* pDuplicateInfo /* = nullptr */)
{
	UINT nDup = 0U;

	if(m_arrScannedFiles.GetCount() > 0) {
		POSITION pos = m_arrScannedFiles.GetStartPosition();
	}

	return nDup;
}

void CScanThread::WalkDir(const CString& strPath, BOOL bRecursive, __int64& iTotalSize)
{
	WIN32_FIND_DATA fd={0};

	
	CString	strFileName;
	CString	strOriginPath = strPath;
	CString	strFullPath;
	CString	strPattern;
	CString	strLookup;
	if(strOriginPath.Right(1) != '\\') {
		strOriginPath.Append(_T("\\"));
	}

	strPattern = strOriginPath + _T("*");
	LARGE_INTEGER li={0};
	HANDLE hSearch = ::FindFirstFile(strPattern, &fd);
	bool bLocked = false;
	CSingleLock locker(&m_CriticalSection);
	if(hSearch != INVALID_HANDLE_VALUE) {
		do 
		{
			locker.Lock();
			if(!m_bRunning) {
				locker.Unlock();
				break;
			}
			locker.Unlock();

			strFileName = fd.cFileName;
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if( (strFileName != _T(".")) && (strFileName != _T("..")) ) {
					strFullPath = strOriginPath + strFileName;
					if(!m_arrScannedFolder.Lookup(strFullPath, strLookup)) {
						m_arrScannedFolder.SetAt(strFullPath, strFullPath);
						if(bRecursive) {
							WalkDir(strFullPath, bRecursive, iTotalSize);
						}
					}
				}
			}
			else {
				strFullPath = strOriginPath + strFileName;
				if(m_pProgressDlg)
					m_pProgressDlg->SetMessageContent(strFullPath);

				if(!IsExcludeFile(strFullPath)){
					if(m_pOwnerWnd && m_pOwnerWnd->GetSafeHwnd()) {
						m_arrScannedFiles.SetAt(strFullPath, new CFileInformation(strFullPath,  &m_CriticalSection, &m_bRunning, m_nDuplicateMask));
						li.LowPart = fd.nFileSizeLow;
						li.HighPart = fd.nFileSizeHigh;
						iTotalSize += static_cast<__int64>(li.QuadPart);
					}
				}
			}

			memset(&fd, 0, sizeof(WIN32_FIND_DATA));
		} while (::FindNextFile(hSearch, &fd) && m_bRunning);

	}

	::FindClose(hSearch);
}