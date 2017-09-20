#pragma once

class CWorkerDialog;

class CScanThread : public CWinThread
{
public:
	CScanThread(CWnd* pOwner = nullptr, CWorkerDialog* pProgressDlg = nullptr);
	virtual ~CScanThread();
	BOOL Initialize(const CStringArray& strPath);
	void Finalize();
private:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
private:
	CWorkerDialog* m_pProgressDlg;
	CWnd*	m_pOwnerWnd;
	CStringArray m_arrPaths;
	BOOL m_bRunning;
	CRITICAL_SECTION  m_RunSection;
};


