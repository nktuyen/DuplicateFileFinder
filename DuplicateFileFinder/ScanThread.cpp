// ScanThread.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "ScanThread.h"
#include "WorkerDialog.h"

CScanThread::CScanThread(CWnd* pOwner /* = nullptr */, CWorkerDialog* pProgressDlg /* = nullptr */)
{
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
		m_pProgressDlg->SetJobName(_T(""));
		m_pProgressDlg->SetProgressRange(0, 100);
		m_pProgressDlg->SetProgressType(CWorkerDialog::eFILL);
		m_pProgressDlg->SetProgressValue(0);
	}

	MSG msg={0};
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	m_bRunning = TRUE;
	
	return TRUE;
}

int CScanThread::Run()
{
	int i = 0;

	m_pProgressDlg->SetMessageTitle(_T("Current Value:"));
	CString	strMessage;

	while (i < 100)
	{
		MSG msg={0};
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

		EnterCriticalSection(&m_RunSection);

		if(!m_bRunning)
			break;
		LeaveCriticalSection(&m_RunSection);
		
		i++;
		strMessage.Format(_T("%d"), i);
		m_pProgressDlg->SetMessageContent(strMessage);
		m_pProgressDlg->SetProgressValue(i);
		Sleep(100);
	}
	LeaveCriticalSection(&m_RunSection);
	return ExitInstance();
}