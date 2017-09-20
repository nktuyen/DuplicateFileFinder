// ScanThread.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "ScanThread.h"
#include "WorkerDialog.h"

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
}

CScanThread::~CScanThread()
{
	RemoveAllFilters();
}

BOOL CScanThread::InitInstance()
{
	InitializeCriticalSection(&m_RunSection	);
	return TRUE;
}

int CScanThread::ExitInstance()
{
	EnterCriticalSection(&m_RunSection);
	m_bRunning = FALSE;
	LeaveCriticalSection(&m_RunSection);
	CloseHandle(m_hThread);
	m_hThread = nullptr;

	RestoreAllControlsEnableState();

	if(m_pProgressDlg) {
		if(m_pProgressDlg->GetSafeHwnd())
			m_pProgressDlg->SendMessage(WM_CLOSE, 0, 0);
	}
	
	DeleteCriticalSection(&m_RunSection);
	m_arrPaths.RemoveAll();
	return CWinThread::ExitInstance();
}

void CScanThread::Finalize()
{
	if(m_bRunning) {
		EnterCriticalSection(&m_RunSection);
		m_bRunning = FALSE;
		LeaveCriticalSection(&m_RunSection);
	}
}

BOOL CScanThread::Initialize(const CStringArray& strPath, BOOL bRecursive)
{
	m_bRecursive = bRecursive;
	m_arrPaths.Copy(strPath);
	m_arrFoundPaths.RemoveAll();

	if(m_arrPaths.GetCount() <= 0)
		return FALSE;

	if(m_pProgressDlg) {
		m_pProgressDlg->SetProgressRange(0, 100);
		m_pProgressDlg->SetProgressType(CWorkerDialog::eMARQUEE);
		m_pProgressDlg->SetProgressValue(0);
	}

	m_bRunning = TRUE;
	
	return TRUE;
}

int CScanThread::Run()
{
	EnableAllControls(FALSE);
	m_pProgressDlg->SetMessageTitle(_T("Current File:"));
	CString	strMessage;
	CString	strCurrentPath;
	for(INT_PTR i=0;i<m_arrPaths.GetCount();i++)
	{
		strCurrentPath = m_arrPaths.GetAt(i);
		strMessage.Format(_T("Scanning [%s]"), strCurrentPath);
		m_pProgressDlg->SetJobName(strMessage);

		EnterCriticalSection(&m_RunSection);
		if(!m_bRunning)
			break;
		LeaveCriticalSection(&m_RunSection);

		WalkDir(strCurrentPath, m_bRecursive);
	}
	LeaveCriticalSection(&m_RunSection);
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
		return TRUE;

	POSITION pos = m_ExcludeFilters.GetStartPosition();
	EFileFolderFilterCriteria eCriteria;
	CFileFolderFilter* pFilter = nullptr;
	UINT nCount = 0;
	while (pos)
	{
		m_ExcludeFilters.GetNextAssoc(pos, eCriteria, pFilter);
		if(pFilter != nullptr) {
			if(pFilter->Filter(strPath))
				nCount++;

			pFilter = nullptr;
		}
	}
	
	return (nCount == m_ExcludeFilters.GetCount());
}

void CScanThread::WalkDir(const CString& strPath, BOOL bRecursive)
{
	WIN32_FIND_DATA fd={0};

	
	CString	strFileName;
	CString	strOriginPath = strPath;
	CString	strFullPath;
	CString	strPattern;

	if(strOriginPath.Right(1) != '\\') {
		strOriginPath.Append(_T("\\"));
	}

	strPattern = strOriginPath + _T("*");

	HANDLE hSearch = ::FindFirstFile(strPattern, &fd);
	if(hSearch != INVALID_HANDLE_VALUE) {
		do 
		{
			EnterCriticalSection(&m_RunSection);
			if(!m_bRunning)
				break;
			LeaveCriticalSection(&m_RunSection);

			strFileName = fd.cFileName;
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if( (strFileName != _T(".")) && (strFileName != _T("..")) ) {
					strFullPath = strOriginPath + strFileName;
					WalkDir(strFullPath, bRecursive);
				}
			}
			else {
				strFullPath = strOriginPath + strFileName;
				if(m_pProgressDlg)
					m_pProgressDlg->SetMessageContent(strFullPath);

				if(!IsExcludeFile(strFullPath)) {
					if(m_pOwnerWnd && m_pOwnerWnd->GetSafeHwnd()) {
						m_strCurrentFile = strFullPath;
						m_arrFoundPaths.Add(m_strCurrentFile);
						m_pOwnerWnd->SendMessage(m_nMessageID, (WPARAM)m_strCurrentFile.GetLength(), (LPARAM)&m_strCurrentFile);
					}
				}
			}

			memset(&fd, 0, sizeof(WIN32_FIND_DATA));
		} while (::FindNextFile(hSearch, &fd));
	}
	::FindClose(hSearch);
}