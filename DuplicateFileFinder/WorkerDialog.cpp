#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "WorkerDialog.h"
#include "afxdialogex.h"
#include "ScanThread.h"


IMPLEMENT_DYNAMIC(CWorkerDialog, CDialog)

CWorkerDialog::CWorkerDialog(CWnd* pParent /* = NULL */)
	: CDialog(CWorkerDialog::IDD, pParent)
{
	m_pWorkerThread = nullptr;
	m_strName = _T("Working in-progress");
	m_strTitle = _T("Current job:");
	m_strMessage = _T("");
	m_nLow = 0;
	m_nHi = 100;
	m_nValue = 0;
	m_eType = eFILL;
}

CWorkerDialog::~CWorkerDialog()
{
}

void CWorkerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STT_TITLE, m_sttTitle);
	DDX_Control(pDX, IDC_STT_MESSAGE, m_sttMessage);
	DDX_Control(pDX, IDC_PRGB_PROGRESS, m_prgbProgress);
}

BEGIN_MESSAGE_MAP(CWorkerDialog, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CWorkerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetFont();
	if(pFont && pFont->GetSafeHandle()) {
		LOGFONT lf={0};
		pFont->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;

		m_fntBold.CreateFontIndirect(&lf);
		m_sttTitle.SetFont(&m_fntBold);
	}

	m_prgbProgress.ModifyStyle(0, PBS_SMOOTH);

	SetJobName(m_strName);
	SetMessageTitle(m_strTitle);
	SetMessageContent(m_strMessage);
	SetProgressType(m_eType);
	SetProgressRange(m_nLow, m_nHi);

	return TRUE;
}

void CWorkerDialog::OnCancel()
{
	DWORD dwCode = 0;
	GetExitCodeThread(m_pWorkerThread->m_hThread, &dwCode);

	if(STILL_ACTIVE == dwCode) {
		m_pWorkerThread->Finalize();
	}
	else {
		CDialog::OnCancel();
		return;
	}

	m_pWorkerThread->SuspendThread();
	int nRes = AfxMessageBox(_T("Are you sure to abort current operation?"), MB_YESNO | MB_ICONQUESTION);
	if(nRes != IDYES) {
		m_pWorkerThread->ResumeThread();
		return;
	}

	m_pWorkerThread->ResumeThread();
	if(GetExitCodeThread(m_pWorkerThread->m_hThread, &dwCode)) {
		while (STILL_ACTIVE == dwCode)
		{
			WaitForSingleObject(m_pWorkerThread->m_hThread, 1000U);
			m_pWorkerThread->Finalize();
			if(!GetExitCodeThread(m_pWorkerThread->m_hThread, &dwCode))
				dwCode = 0;
		}
		CloseHandle(m_pWorkerThread->m_hThread);
		m_pWorkerThread->m_hThread = nullptr;
	}

	CDialog::OnCancel();
}

void CWorkerDialog::OnDestroy()
{
	m_fntBold.DeleteObject();
	CDialog::OnDestroy();
}