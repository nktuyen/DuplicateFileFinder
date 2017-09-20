
// DuplicateFileFinder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "DuplicateFileFinderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDuplicateFileFinderApp

BEGIN_MESSAGE_MAP(CDuplicateFileFinderApp, CWinApp)
	
END_MESSAGE_MAP()


// CDuplicateFileFinderApp construction

CDuplicateFileFinderApp::CDuplicateFileFinderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDuplicateFileFinderApp object

CDuplicateFileFinderApp theApp;


// CDuplicateFileFinderApp initialization

BOOL CDuplicateFileFinderApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	
	CShellManager* afxShellManager = new CShellManager();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("NKTUYEN"));

	CDuplicateFileFinderDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	if(nullptr != afxShellManager) {
		delete afxShellManager;
		afxShellManager = nullptr;
	}
	
	return FALSE;
}

