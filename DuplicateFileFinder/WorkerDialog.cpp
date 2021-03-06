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
	m_nTimerID = 0;
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
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CWorkerDialog::OnBnClickedBtnClose)
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

	if(m_pParentWnd) {
		CRect	rcClient;
		CRect	rcParent;

		GetClientRect(rcClient);
		m_pParentWnd->GetClientRect(rcParent);

		SetWindowPos(&wndNoTopMost, rcParent.Width()/2-rcClient.Width()/2, rcParent.Height()/2-rcClient.Height()/2, 0, 0, SWP_NOSIZE);
	}

	return TRUE;
}

void CWorkerDialog::OnCancel()
{
	CDialog::OnCancel();
}

void CWorkerDialog::OnDestroy()
{
	m_fntBold.DeleteObject();
	if(m_nTimerID != 0) {
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	CDialog::OnDestroy();
}

void CWorkerDialog::OnTimer(UINT_PTR nIDEvent)
{
	if(GetSafeHwnd()) {
		if(nIDEvent == reinterpret_cast<UINT_PTR>(GetSafeHwnd())) {
			int nLow, nHi, nVal;

			m_prgbProgress.GetRange(nLow, nHi);
			nVal = m_prgbProgress.GetPos();

			if(nVal >= nHi)
				nVal = nLow;
			else
				nVal+=10;

			m_prgbProgress.SetPos(nVal);
		}
	}
}

BOOL CWorkerDialog::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN) {
		if((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)) {
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CWorkerDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if( (!bMinimized) && (nState == WA_INACTIVE) ) {
		SendMessage(WM_NCACTIVATE, 1);
		return;
	}
}

BOOL CWorkerDialog::OnNcActivate(BOOL bActive)
{
	
	if(!bActive) {
		SendMessage(WM_NCACTIVATE, 1);
		return TRUE;
	}

	return CDialog::OnNcActivate(bActive);
}

void CWorkerDialog::OnBnClickedBtnClose()
{
	if(m_pWorkerThread) {
		m_pWorkerThread->DisableLock();
		Sleep(10);
		m_pWorkerThread->SuspendThread();
	}

	if(m_nTimerID != 0) {
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	int nRes = AfxMessageBox(_T("Are you sure to abort current operation?"), MB_YESNO | MB_ICONQUESTION);
	if(nRes != IDYES) {

		if(m_eType == eMARQUEE) {
			m_nTimerID = SetTimer(reinterpret_cast<UINT_PTR>(GetSafeHwnd()), TIMER_INTERVAL, nullptr);
		}

		if(m_pWorkerThread) {
			m_pWorkerThread->EnableLock();
			m_pWorkerThread->ResumeThread();
		}
		return;
	}

	DestroyWindow();

	if(m_pWorkerThread) {
		m_pWorkerThread->EnableLock();
		m_pWorkerThread->Finalize();
		m_pWorkerThread->ResumeThread();

		if(nullptr != m_pWorkerThread->m_hThread) {
			CloseHandle(m_pWorkerThread->m_hThread);
			m_pWorkerThread->m_hThread = nullptr;
		}
	}
}
