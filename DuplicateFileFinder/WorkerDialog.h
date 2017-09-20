#pragma once
#include "afxwin.h"
#include "afxcmn.h"
class CScanThread;

#define		PM_EXIT (WM_USER + 101)

class CWorkerDialog : public CDialog
{
	DECLARE_DYNAMIC(CWorkerDialog)
public:
	CWorkerDialog(CWnd* pParent = NULL);
	virtual ~CWorkerDialog();
	enum { IDD = IDD_PROGRESS };
	enum ProgressType{ eMARQUEE, eFILL};

	void SetWorkerThread(CScanThread* pThread) {
		m_pWorkerThread = pThread;
	}

	void SetJobName(const CString& strName) {
		m_strName = strName;
		if(GetSafeHwnd()) {
			SetWindowText(m_strName);
		}
	}

	void SetMessageTitle(const CString& strTitle) {
		m_strTitle = strTitle;
		if(m_sttTitle.GetSafeHwnd()) {
			m_sttTitle.SetWindowText(m_strTitle);
		}
	}

	void SetMessageContent(const CString& strMessage) {
		m_strMessage = strMessage;
		if(m_sttMessage.GetSafeHwnd()) {
			m_sttMessage.SetWindowText(m_strMessage);
		}
	}

	void SetProgressType(ProgressType eType) {
		m_eType = eType;
		if(m_prgbProgress.GetSafeHwnd()) {
			if(m_eType == eMARQUEE) {
				m_prgbProgress.ModifyStyle(0, PBS_MARQUEE);
			}
			else {
				m_prgbProgress.ModifyStyle(PBS_MARQUEE, 0);
			}
		}
	}

	void SetProgressRange(int nLow, int nHi) {
		m_nLow = nLow;
		m_nHi = nHi;
		if(m_nHi < m_nLow) {
			m_nHi = m_nLow;
		}
		if(m_nValue > m_nHi) {
			m_nValue = m_nHi;
		}
		
		if(m_prgbProgress.GetSafeHwnd()) {
			m_prgbProgress.SetRange32(m_nLow, m_nHi);
			m_prgbProgress.SetPos(m_nValue);
		}
	}

	void SetProgressValue(int nValue) {
		m_nValue = nValue;
		if(m_nValue > m_nHi) {
			m_nValue = m_nHi;
		}
		if(m_prgbProgress.GetSafeHwnd()) {
			m_prgbProgress.SetPos(m_nValue);
		}
	}
	
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);
private:
	CString	m_strName;
	CString m_strTitle;
	CString m_strMessage;
	ProgressType m_eType;
	int m_nLow;
	int m_nHi;
	int m_nValue;
	CFont m_fntBold;
	CScanThread* m_pWorkerThread;
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_sttTitle;
	CStatic m_sttMessage;
	CProgressCtrl m_prgbProgress;
};
