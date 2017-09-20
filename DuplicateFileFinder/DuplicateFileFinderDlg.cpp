
// DuplicateFileFinderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "DuplicateFileFinderDlg.h"
#include "afxdialogex.h"
#include "WorkerDialog.h"
#include "ScanThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MIN_WIDTH			1000
#define MIN_HEIGHT			700
#define LEFT_PANEL_WIDTH	460
#define HORIZONTAL_PADDING	4
#define VERTICAL_PADDING	6

UINT CDuplicateFileFinderDlg::m_sThreadMessage = 0U;

CDuplicateFileFinderDlg::CDuplicateFileFinderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDuplicateFileFinderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWorkerDlg = new CWorkerDialog(this);
	CString	strMsg;
	strMsg.Format(_T("%s_%d_%d"), AfxGetAppName(), reinterpret_cast<int>(AfxGetInstanceHandle()), static_cast<int>(GetCurrentProcessId()));
	CDuplicateFileFinderDlg::m_sThreadMessage = RegisterWindowMessage(strMsg);
	m_pScanThread = new CScanThread(this, m_pWorkerDlg, m_sThreadMessage);
	m_pWorkerDlg->SetWorkerThread(m_pScanThread);
}

void CDuplicateFileFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_FILES, m_lstFiles);
	DDX_Control(pDX, IDC_LVW_DETAIL, m_lvwDetail);
	DDX_Control(pDX, IDC_BTN_SCAN, m_btnScan);
	DDX_Control(pDX, IDC_STT_FOLDER_BROWSER, m_sttFolderTree);
	DDX_Control(pDX, IDC_STT_FILES, m_sttDuplicatedFiles);
	DDX_Control(pDX, IDC_STT_SCAN_OPTION, m_sttDuplicateCriteria);
	DDX_Control(pDX, IDC_STT_DETAIL, m_sttDuplicationDetail);
	DDX_Control(pDX, IDC_BTN_PROCESS, m_btnProcess);
	DDX_Control(pDX, IDC_BTN_PROCESS_ALL, m_btnProcessAll);
	DDX_Control(pDX, IDC_CHK_NAME, m_chkName);
	DDX_Control(pDX, IDC_CHK_SIZE, m_chkSize);
	DDX_Control(pDX, IDC_CHK_ATTRIBUTE, m_chkAttributes);
	DDX_Control(pDX, IDC_CHK_CONTENT, m_chkContent);
	DDX_Control(pDX, IDC_CHK_CTIME, m_chkCreateTime);
	DDX_Control(pDX, IDC_CHK_ATIME, m_chkAccessTime);
	DDX_Control(pDX, IDC_CHK_WTIME, m_chkWriteTime);
	DDX_Control(pDX, IDC_STT_CHECKBOX_MESSAGE, m_sttCheckboxMessage);
	DDX_Control(pDX, IDC_LVW_FOLDERS, m_lvwFolders);
	DDX_Control(pDX, IDC_BTN_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_STT_EXCLUDE, m_sttExclude);
	DDX_Control(pDX, IDC_BTN_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_PATTERN, m_chkExcludePattern);
	DDX_Control(pDX, IDC_EDT_PATTERN, m_edtPattern);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_SIZE, m_chkExcludeSize);
	DDX_Control(pDX, IDC_EDT_EXCLUDE_SIZE, m_edtExcludeSize);
	DDX_Control(pDX, IDC_CB_SIZE_UNIT, m_cbSizeUnit);
	DDX_Control(pDX, IDC_CB_SIZE_CRITERIA, m_cboSizeCriteria);
	DDX_Control(pDX, IDC_STT_EXCLUDE_ATTRIBUTES, m_sttExcludeAttr);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_ATTR_READONLY, m_chkExcludeROnly);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_ATTR_HIDDEN, m_chkExcludeHidden);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_ATTR_SYSTEM, m_chkExcludeSystem);
	DDX_Control(pDX, IDC_STT_PATTER_TIPS, m_sttPatternsTips);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_ATTR_TEMP, m_chkExcludeTemp);
	DDX_Control(pDX, IDC_CHK_EXCLUDE_ATTR_ARCHIVE, m_chkExcludeArchive);
	DDX_Control(pDX, IDC_CHK_SCAN_RECURSIVE, m_chkScanRecursive);
	DDX_Control(pDX, IDC_CBO_DUPLICATED_FILE_TYPES, m_cboDuplicatedFileTypes);
}

BEGIN_MESSAGE_MAP(CDuplicateFileFinderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDuplicateFileFinderDlg::OnHdnItemclickLvwDetail)
	ON_BN_CLICKED(IDC_CHK_NAME, &CDuplicateFileFinderDlg::OnBnClickedChkName)
	ON_BN_CLICKED(IDC_CHK_SIZE, &CDuplicateFileFinderDlg::OnBnClickedChkSize)
	ON_BN_CLICKED(IDC_CHK_ATTRIBUTE, &CDuplicateFileFinderDlg::OnBnClickedChkAttribute)
	ON_BN_CLICKED(IDC_CHK_CONTENT, &CDuplicateFileFinderDlg::OnBnClickedChkContent)
	ON_BN_CLICKED(IDC_CHK_CTIME, &CDuplicateFileFinderDlg::OnBnClickedChkCtime)
	ON_BN_CLICKED(IDC_CHK_ATIME, &CDuplicateFileFinderDlg::OnBnClickedChkAtime)
	ON_BN_CLICKED(IDC_CHK_WTIME, &CDuplicateFileFinderDlg::OnBnClickedChkWtime)
	ON_BN_CLICKED(IDC_BTN_SCAN, &CDuplicateFileFinderDlg::OnBnClickedBtnScan)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CDuplicateFileFinderDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CDuplicateFileFinderDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CDuplicateFileFinderDlg::OnBnClickedBtnRemove)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LVW_FOLDERS, &CDuplicateFileFinderDlg::OnLvnItemchangedLvwFolders)
	ON_BN_CLICKED(IDC_CHK_EXCLUDE_PATTERN, &CDuplicateFileFinderDlg::OnBnClickedChkExcludePattern)
	ON_BN_CLICKED(IDC_CHK_EXCLUDE_SIZE, &CDuplicateFileFinderDlg::OnBnClickedChkExcludeSize)
	ON_REGISTERED_MESSAGE(CDuplicateFileFinderDlg::m_sThreadMessage, &CDuplicateFileFinderDlg::OnScanThreadMessage)
END_MESSAGE_MAP()


BOOL CDuplicateFileFinderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitUI();
	ShowWindow(SW_MAXIMIZE);

	return TRUE;
}


void CDuplicateFileFinderDlg::OnDestroy()
{
	delete m_pScanThread;
	delete m_pWorkerDlg;
	CDialogEx::OnDestroy();
}

void CDuplicateFileFinderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CDuplicateFileFinderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDuplicateFileFinderDlg::OnSize(UINT nType, int cx, int cy)
{
	if(nType <= SIZE_MAXHIDE) {
		CDialogEx::OnSize(nType, cx, cy);
	}

	if(GetSafeHwnd()) {

		CRect	rcDialog;
		CRect	rcTemp;
		CRect	rcGroupBox;
		CRect	rcButton;
		CRect	rcName;
		CRect	rcLabel;
		CRect	rcFolderTree;
		CRect	rcListBox;
		CRect	rcExclude;
		CRect	rcBrowse;
		CRect	rcScan;
		CRect	rcPattern;
		CRect	rcExcludeSize;
		CRect	rcExcludeAttr;

		GetClientRect(rcDialog);
		rcDialog.NormalizeRect();
		//rcDialog.DeflateRect(HORIZONTAL_PADDING, VERTICAL_PADDING, HORIZONTAL_PADDING, VERTICAL_PADDING);

		if(m_sttDuplicateCriteria.GetSafeHwnd()) {
			if(m_btnScan.GetSafeHwnd()) {
				m_btnScan.GetWindowRect(rcScan);
			}
			m_sttDuplicateCriteria.GetWindowRect(rcGroupBox);
			rcTemp.bottom = (rcDialog.bottom - VERTICAL_PADDING);
			rcTemp.left = (rcDialog.left + HORIZONTAL_PADDING);
			rcTemp.right = (LEFT_PANEL_WIDTH - rcScan.Width() - HORIZONTAL_PADDING);
			rcTemp.top = (rcTemp.bottom - rcGroupBox.Height());
			m_sttDuplicateCriteria.MoveWindow(rcTemp);

			rcGroupBox = rcTemp;
			rcExclude = rcGroupBox;
		}

		rcExclude.OffsetRect(0, -rcGroupBox.Height() - VERTICAL_PADDING);
		if(m_sttExclude.GetSafeHwnd()) {
			m_sttExclude.GetWindowRect(rcTemp);
			rcExclude.top = (rcExclude.bottom - rcTemp.Height());
			m_sttExclude.MoveWindow(rcExclude);
		}

		rcPattern = rcExclude;
		if(m_chkExcludePattern.GetSafeHwnd()) {
			m_chkExcludePattern.GetWindowRect(rcTemp);
			rcPattern.OffsetRect(HORIZONTAL_PADDING*3, VERTICAL_PADDING*3);
			rcPattern.right = (rcPattern.left + rcTemp.Width());
			rcPattern.bottom = (rcPattern.top + rcTemp.Height());
			m_chkExcludePattern.MoveWindow(rcPattern);
			rcPattern.OffsetRect(rcPattern.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_edtPattern.GetSafeHwnd()) {
			m_edtPattern.GetWindowRect(rcTemp);
			rcPattern.right = (rcPattern.left + rcTemp.Width());
			rcPattern.bottom = (rcPattern.top + rcTemp.Height());
			m_edtPattern.MoveWindow(rcPattern);
			rcPattern.OffsetRect(rcPattern.Width() + HORIZONTAL_PADDING, 0);
		}
		
		if(m_sttPatternsTips.GetSafeHwnd()) {
			m_sttPatternsTips.GetWindowRect(rcTemp);
			rcPattern.right = (rcPattern.left + rcTemp.Width());
			rcPattern.bottom = (rcPattern.top + rcTemp.Height());
			m_sttPatternsTips.MoveWindow(rcPattern);
		}

		rcExcludeSize = rcExclude;
		if(m_chkExcludeSize.GetSafeHwnd()) {
			m_chkExcludeSize.GetWindowRect(rcTemp);
			rcExcludeSize.OffsetRect(HORIZONTAL_PADDING*3, VERTICAL_PADDING*3);
			rcExcludeSize.right = (rcExcludeSize.left + rcTemp.Width());
			rcExcludeSize.top = (rcPattern.bottom + VERTICAL_PADDING);
			rcExcludeSize.bottom = (rcExcludeSize.top + rcTemp.Height());
			m_chkExcludeSize.MoveWindow(rcExcludeSize);

			rcExcludeSize.OffsetRect(rcExcludeSize.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_cboSizeCriteria.GetSafeHwnd()) {
			m_cboSizeCriteria.GetWindowRect(rcTemp);
			rcExcludeSize.bottom = (rcExcludeSize.top + rcTemp.Height());
			rcExcludeSize.right = (rcExcludeSize.left + rcTemp.Width());
			m_cboSizeCriteria.MoveWindow(rcExcludeSize);

			rcExcludeSize.OffsetRect(rcExcludeSize.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_edtExcludeSize.GetSafeHwnd()) {
			m_edtExcludeSize.GetWindowRect(rcTemp);
			rcExcludeSize.right = (rcExcludeSize.left + rcTemp.Width());
			rcExcludeSize.bottom = (rcExcludeSize.top + rcTemp.Height());
			m_edtExcludeSize.MoveWindow(rcExcludeSize);
			rcExcludeSize.OffsetRect(rcExcludeSize.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_cbSizeUnit.GetSafeHwnd()) {
			m_cbSizeUnit.GetWindowRect(rcTemp);
			rcExcludeSize.right = (rcExcludeSize.left + rcTemp.Width());
			rcExcludeSize.bottom = (rcExcludeSize.top + rcTemp.Height());
			m_cbSizeUnit.MoveWindow(rcExcludeSize);
		}

		rcExcludeAttr = rcExclude;
		if(m_sttExcludeAttr.GetSafeHwnd()) {
			m_sttExcludeAttr.GetWindowRect(rcTemp);
			rcExcludeAttr.OffsetRect(HORIZONTAL_PADDING*3, VERTICAL_PADDING*3);
			rcExcludeAttr.top = (rcExcludeSize.bottom + VERTICAL_PADDING);
			rcExcludeAttr.right = (LEFT_PANEL_WIDTH - rcScan.Width() - HORIZONTAL_PADDING *4);
			rcExcludeAttr.bottom = (rcExcludeAttr.top + rcTemp.Height());
			m_sttExcludeAttr.MoveWindow(rcExcludeAttr);

			rcExcludeAttr.OffsetRect(HORIZONTAL_PADDING*3, VERTICAL_PADDING*3);
		}

		if(m_chkExcludeROnly.GetSafeHwnd()) {
			m_chkExcludeROnly.GetWindowRect(rcTemp);
			rcExcludeAttr.right = (rcExcludeAttr.left + rcTemp.Width());
			rcExcludeAttr.bottom = (rcExcludeAttr.top + rcTemp.Height());
			m_chkExcludeROnly.MoveWindow(rcExcludeAttr);

			rcExcludeAttr.OffsetRect(rcExcludeAttr.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_chkExcludeHidden.GetSafeHwnd()) {
			m_chkExcludeHidden.GetWindowRect(rcTemp);
			rcExcludeAttr.right = (rcExcludeAttr.left + rcTemp.Width());
			rcExcludeAttr.bottom = (rcExcludeAttr.top + rcTemp.Height());
			m_chkExcludeHidden.MoveWindow(rcExcludeAttr);
			rcExcludeAttr.OffsetRect(rcExcludeAttr.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_chkExcludeSystem.GetSafeHwnd()) {
			m_chkExcludeSystem.GetWindowRect(rcTemp);
			rcExcludeAttr.right = (rcExcludeAttr.left + rcTemp.Width());
			rcExcludeAttr.bottom = (rcExcludeAttr.top + rcTemp.Height());
			m_chkExcludeSystem.MoveWindow(rcExcludeAttr);
			rcExcludeAttr.OffsetRect(rcExcludeAttr.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_chkExcludeTemp.GetSafeHwnd()) {
			m_chkExcludeTemp.GetWindowRect(rcTemp);
			rcExcludeAttr.right = (rcExcludeAttr.left + rcTemp.Width());
			rcExcludeAttr.bottom = (rcExcludeAttr.top + rcTemp.Height());
			m_chkExcludeTemp.MoveWindow(rcExcludeAttr);
			rcExcludeAttr.OffsetRect(rcExcludeAttr.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_chkExcludeArchive.GetSafeHwnd()) {
			m_chkExcludeArchive.GetWindowRect(rcTemp);
			rcExcludeAttr.right = (rcExcludeAttr.left + rcTemp.Width());
			rcExcludeAttr.bottom = (rcExcludeAttr.top + rcTemp.Height());
			m_chkExcludeArchive.MoveWindow(rcExcludeAttr);
			//rcExcludeAttr.OffsetRect(rcExcludeAttr.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_chkName.GetSafeHwnd()) {
			m_chkName.GetWindowRect(rcButton);
			rcTemp.left = (rcGroupBox.left + HORIZONTAL_PADDING*3);
			rcTemp.right = (rcTemp.left + rcButton.Width());
			rcTemp.top = (rcGroupBox.top + VERTICAL_PADDING*3);
			rcTemp.bottom = (rcTemp.top + rcButton.Height());

			m_chkName.MoveWindow(rcTemp);
			rcButton = rcTemp;
			rcName = rcTemp;
			rcButton.OffsetRect(0, rcButton.Height() + VERTICAL_PADDING);
		}

		
		if(m_chkSize.GetSafeHwnd()) {
			m_chkSize.MoveWindow(rcButton);
			rcButton.OffsetRect(0, rcButton.Height() + VERTICAL_PADDING);
		}

		if(m_chkAttributes.GetSafeHwnd()) {
			m_chkAttributes.MoveWindow(rcButton);
			rcButton.OffsetRect(0, rcButton.Height() + VERTICAL_PADDING);
		}

		if(m_chkContent.GetSafeHwnd()) {
			m_chkContent.MoveWindow(rcButton);
			rcButton.OffsetRect(0, rcButton.Height() + VERTICAL_PADDING);
		}

		if(m_sttCheckboxMessage.GetSafeHwnd()) {
			m_sttCheckboxMessage.GetWindowRect(rcTemp);
			rcLabel = rcButton;
			rcLabel.right = (rcLabel.left + rcTemp.Width());
			rcLabel.bottom = (rcGroupBox.bottom - VERTICAL_PADDING);
			rcLabel.top = (rcLabel.bottom - rcTemp.Height());

			m_sttCheckboxMessage.MoveWindow(rcLabel);
		}

		rcButton = rcName;
		rcButton.OffsetRect(rcName.Width() + HORIZONTAL_PADDING, 0);

		if(m_chkCreateTime.GetSafeHwnd()) {
			m_chkCreateTime.GetWindowRect(rcTemp);
			rcButton.right = (rcButton.left + rcTemp.Width());
			m_chkCreateTime.MoveWindow(rcButton);
			rcButton.OffsetRect(0, rcButton.Height() + VERTICAL_PADDING);
		}

		if(m_chkAccessTime.GetSafeHwnd()) {
			m_chkAccessTime.MoveWindow(rcButton);
			rcButton.OffsetRect(0, rcButton.Height() + VERTICAL_PADDING);
		}

		if(m_chkWriteTime.GetSafeHwnd()) {
			m_chkWriteTime.MoveWindow(rcButton);
		}

		rcButton = rcGroupBox;
		if(m_btnScan.GetSafeHwnd()) {
			m_btnScan.GetWindowRect(rcTemp);
			rcButton.OffsetRect(rcGroupBox.Width() + HORIZONTAL_PADDING, 0);
			rcButton.top = rcButton.top + (rcGroupBox.Height() - rcTemp.Height())/2;
			rcButton.bottom = (rcButton.top + rcTemp.Height());
			rcButton.right = LEFT_PANEL_WIDTH;
			rcButton.left = (rcButton.right - rcTemp.Width());
			rcScan = rcButton;
			m_btnScan.MoveWindow(rcScan);
			rcScan.OffsetRect(0, rcScan.Height()+VERTICAL_PADDING);
		}

		if(m_chkScanRecursive.GetSafeHwnd()) {
			m_chkScanRecursive.GetWindowRect(rcTemp);
			rcScan.right = (rcScan.left + rcTemp.Width());
			rcScan.bottom = (rcScan.top + rcTemp.Height());
			m_chkScanRecursive.MoveWindow(rcScan);
		}

		if(m_sttFolderTree.GetSafeHwnd()) {
			m_sttFolderTree.GetWindowRect(rcLabel);

			rcTemp.top = VERTICAL_PADDING;
			rcTemp.left = (rcDialog.left + HORIZONTAL_PADDING);
			rcTemp.right = (rcTemp.left + rcLabel.Width());
			rcTemp.bottom = (rcTemp.top + rcLabel.Height());

			m_sttFolderTree.MoveWindow(rcTemp);
			rcLabel = rcTemp;
			rcLabel.OffsetRect(0, rcLabel.Height() + VERTICAL_PADDING);
			rcFolderTree = rcLabel;
		}

		if(m_btnBrowse.GetSafeHwnd()) {
			
			m_btnBrowse.GetWindowRect(rcButton);
			rcBrowse.bottom = (rcExclude.top - VERTICAL_PADDING);
			rcBrowse.left = (rcDialog.left + HORIZONTAL_PADDING);
			rcBrowse.right = (rcBrowse.left + rcButton.Width());
			rcBrowse.top = (rcBrowse.bottom - rcButton.Height());
			m_btnBrowse.MoveWindow(rcBrowse);

			rcBrowse.OffsetRect(rcBrowse.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_btnRemove.GetSafeHwnd()) {
			m_btnRemove.MoveWindow(rcBrowse);
			rcBrowse.OffsetRect(rcBrowse.Width() + HORIZONTAL_PADDING, 0);
		}

		if(m_btnClear.GetSafeHwnd()) {
			m_btnClear.MoveWindow(rcBrowse);
		}

		if(m_lvwFolders.GetSafeHwnd()) {
			rcFolderTree.right = LEFT_PANEL_WIDTH;
			rcFolderTree.bottom = (rcBrowse.top - VERTICAL_PADDING);
			m_lvwFolders.MoveWindow(rcFolderTree);
		}

		if(m_sttDuplicatedFiles.GetSafeHwnd()) {
			m_sttDuplicatedFiles.GetWindowRect(rcTemp);
			rcLabel.left = (LEFT_PANEL_WIDTH + HORIZONTAL_PADDING);
			rcLabel.right = (rcLabel.left + rcTemp.Width());
			rcLabel.top = (rcDialog.top + VERTICAL_PADDING);
			rcLabel.bottom = (rcLabel.top + rcTemp.Height());
			m_sttDuplicatedFiles.MoveWindow(rcLabel);
			rcLabel.OffsetRect(0, rcLabel.Height() + VERTICAL_PADDING);
		}

		if(m_cboDuplicatedFileTypes.GetSafeHwnd()) {
			m_cboDuplicatedFileTypes.GetWindowRect(rcTemp);
			CRect	rcFileTypes;
			rcFileTypes.right = (rcDialog.right - HORIZONTAL_PADDING);
			rcFileTypes.left = (rcFileTypes.right - rcTemp.Width());
			rcFileTypes.top = (rcDialog.top + VERTICAL_PADDING);
			rcFileTypes.bottom = (rcFileTypes.top + rcTemp.Height());
			m_cboDuplicatedFileTypes.MoveWindow(rcFileTypes);
		}

		if(m_lstFiles.GetSafeHwnd()) {

			rcButton = CRect(0,0,0,0);
			if(m_btnProcessAll.GetSafeHwnd()) {
				m_btnProcessAll.GetWindowRect(rcButton);
			}

			rcTemp = rcLabel;
			rcTemp.bottom = (rcExclude.bottom - rcButton.Height() - VERTICAL_PADDING);
			rcTemp.right = (rcDialog.right - HORIZONTAL_PADDING);
			rcListBox = rcTemp;
			m_lstFiles.MoveWindow(rcTemp);

			if(m_btnProcessAll.GetSafeHwnd()) {
				rcTemp.left = (rcListBox.left + (rcListBox.Width()-rcButton.Width())/2);
				rcTemp.right = (rcTemp.left + rcButton.Width());
				rcTemp.bottom = rcExclude.bottom;
				rcTemp.top = (rcTemp.bottom - rcButton.Height());

				m_btnProcessAll.MoveWindow(rcTemp);
			}
		}

		rcLabel.left = rcListBox.left;
		rcLabel.top = rcGroupBox.top;
		if(m_sttDuplicationDetail.GetSafeHwnd()) {
			m_sttDuplicationDetail.GetWindowRect(rcTemp);
			rcLabel.right = (rcLabel.left + rcTemp.Width());
			rcLabel.bottom = (rcLabel.top + rcTemp.Height());
			m_sttDuplicationDetail.MoveWindow(rcLabel);

			rcLabel.OffsetRect(0, rcLabel.Height() + VERTICAL_PADDING);
		}

		if(m_lvwDetail.GetSafeHwnd()) {
			rcButton = CRect(0,0,0,0);
			if(m_btnProcess.GetSafeHwnd()) {
				m_btnProcess.GetWindowRect(rcButton);

			}
			rcListBox = rcLabel;
			rcListBox.right = (rcDialog.right - HORIZONTAL_PADDING);
			rcListBox.bottom = (rcDialog.bottom - rcButton.Height() - VERTICAL_PADDING*2);
			m_lvwDetail.MoveWindow(rcListBox);

			if(m_btnProcess.GetSafeHwnd()) {
				rcTemp = rcButton;
				rcButton.left = rcListBox.left + (rcListBox.Width()-rcTemp.Width())/2;
				rcButton.right = (rcButton.left + rcTemp.Width());
				rcButton.bottom = (rcDialog.bottom - VERTICAL_PADDING);
				rcButton.top = (rcButton.bottom - rcTemp.Height());
				m_btnProcess.MoveWindow(rcButton);
			}
		}
	}
}

void CDuplicateFileFinderDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if(lpMMI->ptMinTrackSize.x < MIN_WIDTH) {
		lpMMI->ptMinTrackSize.x = MIN_WIDTH;
	}

	if(lpMMI->ptMinTrackSize.y < MIN_HEIGHT) {
		lpMMI->ptMinTrackSize.y = MIN_HEIGHT;
	}

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CDuplicateFileFinderDlg::InitUI()
{
	CFont* pFont = GetFont();
	if(pFont && pFont->GetSafeHandle()) {
		LOGFONT lf={0};
		pFont->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		m_fntBold.CreateFontIndirect(&lf);

		m_sttFolderTree.SetFont(&m_fntBold);
		m_sttDuplicateCriteria.SetFont(&m_fntBold);
		m_sttDuplicatedFiles.SetFont(&m_fntBold);
		m_sttDuplicationDetail.SetFont(&m_fntBold);

		m_btnProcess.SetFont(&m_fntBold);
		m_btnProcessAll.SetFont(&m_fntBold);
		m_btnScan.SetFont(&m_fntBold);

		lf.lfWeight = FW_REGULAR;
		lf.lfItalic = 1;
		m_fntItalic.CreateFontIndirect(&lf);
		m_sttCheckboxMessage.SetFont(&m_fntItalic);

		m_sttExclude.SetFont(&m_fntBold);
		m_btnBrowse.SetFont(&m_fntBold);
		m_btnRemove.SetFont(&m_fntBold);
		m_btnClear.SetFont(&m_fntBold);

		m_sttPatternsTips.SetFont(&m_fntItalic);
	}

	m_chkContent.SetCheck(BST_CHECKED);
	m_chkSize.SetCheck(BST_CHECKED);
	OnBnClickedChkSize();

	m_lvwFolders.InsertColumn(0, _T("Path"), LVCFMT_LEFT, 500);
	m_lvwFolders.GetHeaderCtrl()->SetFont(&m_fntBold);
	m_lvwFolders.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

	m_lvwDetail.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);
	m_lvwDetail.GetHeaderCtrl()->SetFont(&m_fntBold);

	m_cboSizeCriteria.AddString(_T("<"));
	m_cboSizeCriteria.AddString(_T("="));
	m_cboSizeCriteria.AddString(_T(">"));
	m_cboSizeCriteria.SetCurSel(ESizeLessThan);

	m_cbSizeUnit.AddString(_T("Bytes"));
	m_cbSizeUnit.AddString(_T("KB"));
	m_cbSizeUnit.AddString(_T("MB"));
	m_cbSizeUnit.AddString(_T("GB"));
	m_cbSizeUnit.AddString(_T("TB"));
	m_cbSizeUnit.AddString(_T("PB"));
	m_cbSizeUnit.SetCurSel(eByte);
	SetDlgItemInt(IDC_EDT_EXCLUDE_SIZE, 0);

	m_chkScanRecursive.SetCheck(BST_CHECKED);
}


void CDuplicateFileFinderDlg::OnHdnItemclickLvwDetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	m_btnRemove.EnableWindow(m_lvwFolders.GetSelectedCount()>0);
	*pResult = 0;
}


void CDuplicateFileFinderDlg::OnTvnSelchangedFolderBrowser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

BOOL CDuplicateFileFinderDlg::AtLeastOneChecked()
{
	if( (m_chkAccessTime.GetCheck() == BST_CHECKED) ||
		(m_chkAttributes.GetCheck() == BST_CHECKED) ||
		(m_chkContent.GetCheck() == BST_CHECKED) ||
		(m_chkCreateTime.GetCheck() == BST_CHECKED) ||
		(m_chkName.GetCheck() == BST_CHECKED) ||
		(m_chkSize.GetCheck() == BST_CHECKED) ||
		(m_chkWriteTime.GetCheck() == BST_CHECKED) )

		return TRUE;

	else
		return FALSE;
}

void CDuplicateFileFinderDlg::ResetDetailList()
{
	while (m_lvwDetail.GetHeaderCtrl()->GetItemCount() > 0)
	{
		m_lvwDetail.DeleteColumn(0);
	}

	m_lvwDetail.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 200);
	m_lvwDetail.InsertColumn(1, _T("Parent Folder"), LVCFMT_LEFT, 300);

	if(m_chkSize.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Size"), LVCFMT_LEFT, 60);
	}
	if(m_chkAttributes.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Attributes"), LVCFMT_LEFT, 70);
	}
	if(m_chkContent.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("CRC"), LVCFMT_LEFT, 70);
	}
	if(m_chkCreateTime.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Create Time"), LVCFMT_LEFT, 100);
	}
	if(m_chkAccessTime.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Last Access Time"), LVCFMT_LEFT, 100);
	}
	if(m_chkWriteTime.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Last Write Time"), LVCFMT_LEFT, 100);
	}

	m_lvwDetail.DeleteAllItems();
}

void CDuplicateFileFinderDlg::OnBnClickedChkName()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedChkSize()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedChkAttribute()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedChkContent()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedChkCtime()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedChkAtime()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedChkWtime()
{
	m_btnScan.EnableWindow(AtLeastOneChecked());
	m_sttCheckboxMessage.ShowWindow(AtLeastOneChecked()?SW_HIDE:SW_SHOW);

	ResetDetailList();
}


void CDuplicateFileFinderDlg::OnBnClickedBtnScan()
{
	if(!ReadyToScan())
		return;

	m_lstFiles.ResetContent();
	m_lvwDetail.DeleteAllItems();
	m_cboDuplicatedFileTypes.ResetContent();
	m_arrFileTypes.RemoveAll();
	m_cboDuplicatedFileTypes.SetItemDataPtr(m_cboDuplicatedFileTypes.AddString(_T(" -------------------- All -------------------- ")), nullptr);
	m_cboDuplicatedFileTypes.SetCurSel(0);

	POSITION pos = m_arrPaths.GetStartPosition();
	CString	strPath;
	CStringArray arrPaths;
	int nItem = -1;
	while (pos)
	{
		m_arrPaths.GetNextAssoc(pos, strPath, reinterpret_cast<void*&>(nItem));
		arrPaths.Add(strPath);
	}

	if(!m_pScanThread->Initialize(arrPaths, (m_chkScanRecursive.GetCheck() == BST_CHECKED))) {
		AfxMessageBox(_T("An error occred!"), MB_ICONEXCLAMATION);
		return;
	}
	m_pScanThread->RemoveAllFilters();
	if( (m_chkExcludePattern.GetCheck() == BST_CHECKED) && (m_edtPattern.GetWindowTextLength() > 0) ) {
		CString	strPatterns;
		m_edtPattern.GetWindowText(strPatterns);
		CPatternFilter* pFilter = new CPatternFilter(_T("Patterns"), strPatterns);
		m_pScanThread->AddFilter(pFilter);
	}

	if(m_chkExcludeSize.GetCheck() == BST_CHECKED) {
		ESizeFilterCriteria eCriteria = static_cast<ESizeFilterCriteria>(m_cboSizeCriteria.GetCurSel());
		__int64 nSize = static_cast<__int64>(GetDlgItemInt(IDC_EDT_EXCLUDE_SIZE));
		ESizeUnit eUnit = static_cast<ESizeUnit>(m_cbSizeUnit.GetCurSel());

		CSizeFilter* pSizeFilter = new CSizeFilter(_T("Size"), eCriteria, nSize, eUnit);
		m_pScanThread->AddFilter(pSizeFilter);
	}

	UINT nAttrs = 0;
	if(m_chkExcludeROnly.GetCheck() == BST_CHECKED) {
		nAttrs |= FILE_ATTRIBUTE_READONLY;
	}
	if(m_chkExcludeHidden.GetCheck() == BST_CHECKED) {
		nAttrs |= FILE_ATTRIBUTE_HIDDEN;
	}
	if(m_chkExcludeSystem.GetCheck() == BST_CHECKED) {
		nAttrs |= FILE_ATTRIBUTE_SYSTEM;
	}
	if(m_chkExcludeTemp.GetCheck() == BST_CHECKED) {
		nAttrs |= FILE_ATTRIBUTE_TEMPORARY;
	}
	if(m_chkExcludeArchive.GetCheck() == BST_CHECKED) {
		nAttrs |= FILE_ATTRIBUTE_ARCHIVE;
	}
	CAttributesFilter* pAttrFilter = new CAttributesFilter(_T("Attributes"), nAttrs);
	m_pScanThread->AddFilter(pAttrFilter);

	if(m_pScanThread->m_hThread != nullptr) {
		CloseHandle(m_pScanThread->m_hThread);
		m_pScanThread->m_hThread = nullptr;
	}
	if(m_pWorkerDlg->GetSafeHwnd()) 
		m_pWorkerDlg->DestroyWindow();

	m_pWorkerDlg->Create(CWorkerDialog::IDD, this);
	m_pWorkerDlg->ShowWindow(SW_SHOW);
	
	m_pScanThread->CreateThread();
}


BOOL CDuplicateFileFinderDlg::ReadyToScan()
{
	if(m_lvwFolders.GetSafeHwnd() == NULL)
	{
		AfxMessageBox(_T("An error occured!"), MB_ICONEXCLAMATION);
		return FALSE;
	}

	UINT nSekected = EnumCheckedPaths();
	if(0 >= nSekected) {
		AfxMessageBox(_T("No folder to scan!"), MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

UINT CDuplicateFileFinderDlg::EnumCheckedPaths()
{
	return m_arrPaths.GetCount();
}

void CDuplicateFileFinderDlg::EnumCheckedChildNode(HTREEITEM hNode)
{
	
}

void CDuplicateFileFinderDlg::OnBnClickedBtnBrowse()
{
	BROWSEINFO bif={0};
	bif.hwndOwner = GetSafeHwnd();
	bif.ulFlags = BIF_RETURNONLYFSDIRS | BIF_BROWSEFORCOMPUTER;

	LPITEMIDLIST folder = SHBrowseForFolder(&bif);
	if(nullptr != folder) {
		CString	strPath;
		if(SHGetPathFromIDList(folder, strPath.GetBuffer(MAX_PATH))) {
			strPath.ReleaseBuffer();
			int nItem = -1;
			if(m_arrPaths.Lookup(strPath, reinterpret_cast<void*&>(nItem))) {
				if(nItem != -1) {
					CString	strMsg;
					strMsg.Format(_T("Folder \"%s\" is already exist in the list at index %d"), strPath, nItem);
					AfxMessageBox(strMsg, MB_ICONINFORMATION);
					return;
				}
			}
			nItem = m_lvwFolders.InsertItem(m_lvwFolders.GetItemCount(), strPath);
			m_arrPaths.SetAt(strPath,  reinterpret_cast<void*>(nItem));
			m_btnClear.EnableWindow(m_lvwFolders.GetItemCount()>0);
		}
	}
}


void CDuplicateFileFinderDlg::OnDropFiles(HDROP hDropInfo)
{
	DragAcceptFiles(TRUE);
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);
	if(0< nFiles) {
		CString	strPath;
		UINT nBuf = 1024;
		CString	strMsg;
		int nItem = -1;
		for(UINT n=0;n<nFiles;n++) {
			DragQueryFile(hDropInfo, n, strPath.GetBuffer(nBuf), nBuf);
			strPath.ReleaseBuffer();
			if(!strPath.IsEmpty()) {
				if(PathIsDirectory(strPath)) {
					nItem = -1;
					if(m_arrPaths.Lookup(strPath, reinterpret_cast<void*&>(nItem))) {
						if(nItem != -1) {
							strMsg.Format(_T("Folder \"%s\" is already exist in the list at index %d"), strPath, nItem);
							AfxMessageBox(strMsg, MB_ICONINFORMATION);
							continue;
						}
					}
					nItem = m_lvwFolders.InsertItem(m_lvwFolders.GetItemCount(), strPath);
					m_arrPaths.SetAt(strPath,  reinterpret_cast<void*>(nItem));
					m_btnClear.EnableWindow(m_lvwFolders.GetItemCount()>0);
				}
				else {
					strMsg.Format(_T("Path \"%s\" is not a folder!"), strPath);
					AfxMessageBox(strMsg, MB_ICONINFORMATION);
					continue;
				}
			}
		}
	}
	DragFinish(hDropInfo);
}

void CDuplicateFileFinderDlg::OnBnClickedBtnClear()
{
	if(m_lvwFolders.GetItemCount() > 0) {
		int nConfirm = AfxMessageBox(_T("Are you sure to remove all folders in the list?"), MB_YESNO | MB_ICONQUESTION);
		if(nConfirm != IDYES)
			return;
	}
	m_lvwFolders.DeleteAllItems();
	m_arrPaths.RemoveAll();
	m_btnClear.EnableWindow(m_lvwFolders.GetItemCount()>0);
	m_btnRemove.EnableWindow(m_lvwFolders.GetSelectedCount()>0);
}


void CDuplicateFileFinderDlg::OnBnClickedBtnRemove()
{
	UINT nState = 0;
	CString	strPath;
	if(m_lvwFolders.GetItemCount() > 0) {
		int nConfirm = AfxMessageBox(_T("Are you sure to remove all selected folders?"), MB_YESNO | MB_ICONQUESTION);
		if(nConfirm != IDYES)
			return;
	}

	for(int i=m_lvwFolders.GetItemCount()-1;i>=0;i--) {
		nState = m_lvwFolders.GetItemState(i, LVIS_SELECTED);
		if(nState & LVIS_SELECTED) {
			strPath = m_lvwFolders.GetItemText(i, 0);
			if(m_lvwFolders.DeleteItem(i)) {
				m_arrPaths.RemoveKey(strPath);
			}
		}
	}
	m_btnClear.EnableWindow(m_lvwFolders.GetItemCount()>0);
	m_btnRemove.EnableWindow(m_lvwFolders.GetSelectedCount()>0);
}


void CDuplicateFileFinderDlg::OnLvnItemchangedLvwFolders(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_btnRemove.EnableWindow(m_lvwFolders.GetSelectedCount()>0);
	*pResult = 0;
}


void CDuplicateFileFinderDlg::OnBnClickedChkExcludePattern()
{
	m_edtPattern.EnableWindow(m_chkExcludePattern.GetCheck() == BST_CHECKED);
	m_sttPatternsTips.ShowWindow((m_chkExcludePattern.GetCheck() == BST_CHECKED)?TRUE:FALSE);
}


void CDuplicateFileFinderDlg::OnBnClickedChkExcludeSize()
{
	m_cboSizeCriteria.EnableWindow(m_chkExcludeSize.GetCheck() == BST_CHECKED);
	m_edtExcludeSize.EnableWindow(m_chkExcludeSize.GetCheck() == BST_CHECKED);
	m_cbSizeUnit.EnableWindow(m_chkExcludeSize.GetCheck() == BST_CHECKED);
}


LRESULT CDuplicateFileFinderDlg::OnScanThreadMessage(WPARAM wparam, LPARAM lParam)
{
	CString* pStr = (CString*)lParam;
	if(pStr != nullptr) {
		CString	strPath = (pStr->GetBuffer());
		m_lstFiles.AddString(strPath);

		SHFILEINFO si = {0};
		CString	strDisplayName;
		CString	strNameOut;
		if(SHGetFileInfo(strPath, 0, &si, sizeof(SHFILEINFO), SHGFI_TYPENAME ) > 0)
		{
			strDisplayName = si.szTypeName;
			if( (!m_arrFileTypes.Lookup(strDisplayName, strNameOut)) || (strNameOut.IsEmpty()) ) {
				m_arrFileTypes.SetAt(strDisplayName, strDisplayName);
				m_cboDuplicatedFileTypes.AddString(strDisplayName);
			}

		}
	}

	return (LRESULT)0;
}

void CDuplicateFileFinderDlg::OnCancel()
{
	if(m_pWorkerDlg && m_pWorkerDlg->GetSafeHwnd() &&  m_pWorkerDlg->IsWindowVisible())
		return;

	CDialogEx::OnCancel();
}