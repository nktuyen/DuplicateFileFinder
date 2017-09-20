// ScanThread.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "ScanThread.h"
#include "WorkerDialog.h"

CScanThread* CScanThread::m_sInstance = nullptr;

CScanThread::CScanThread(CWnd* pOwner /* = nullptr */, CWorkerDialog* pProgressDlg /* = nullptr */)
{
	CScanThread::m_sInstance = this;
	m_pOwnerWnd = pOwner;
	m_pProgressDlg = pProgressDlg;
	m_arrPaths.RemoveAll();
	m_bRunning = FALSE;
	m_bAutoDelete = FALSE;
}

CScanThread::~CScanThread()
{
	
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

BOOL CScanThread::Initialize(const CStringArray& strPath)
{
	m_arrPaths.Copy(strPath);

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

		m_pProgressDlg->SetMessageContent(_T("XXX"));
		Sleep(100);
	}
	LeaveCriticalSection(&m_RunSection);
	return ExitInstance();
}