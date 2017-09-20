
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

#define MIN_WIDTH			800
#define MIN_HEIGHT			450
#define LEFT_PANEL_WIDTH	250
#define HORIZONTAL_PADDING	6
#define VERTICAL_PADDING	6

CDuplicateFileFinderDlg::CDuplicateFileFinderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDuplicateFileFinderDlg::IDD, pParent)
	, m_WorkerDlg(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_pWorkerDlg->Create(CWorkerDialog::IDD, this);
	m_pScanThread = new CScanThread(this, &m_WorkerDlg);
	m_WorkerDlg.SetWorkerThread(m_pScanThread);
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

		GetClientRect(rcDialog);
		rcDialog.NormalizeRect();
		//rcDialog.DeflateRect(HORIZONTAL_PADDING, VERTICAL_PADDING, HORIZONTAL_PADDING, VERTICAL_PADDING);

		if(m_sttDuplicateCriteria.GetSafeHwnd()) {
			m_sttDuplicateCriteria.GetWindowRect(rcGroupBox);
			rcTemp.bottom = (rcDialog.bottom - VERTICAL_PADDING);
			rcTemp.left = (rcDialog.left + HORIZONTAL_PADDING);
			rcTemp.right = (rcTemp.left + LEFT_PANEL_WIDTH);
			rcTemp.top = (rcTemp.bottom - rcGroupBox.Height());
			m_sttDuplicateCriteria.MoveWindow(rcTemp);

			rcGroupBox = rcTemp;
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
			rcButton.right = (rcButton.left + rcTemp.Width());

			m_btnScan.MoveWindow(rcButton);
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
		}

		if(m_lvwFolders.GetSafeHwnd()) {
			rcLabel.right = rcButton.right;
			rcLabel.bottom = (rcGroupBox.top - VERTICAL_PADDING);
			rcFolderTree = rcLabel;
			m_lvwFolders.MoveWindow(rcLabel);
		}

		if(m_btnBrowse.GetSafeHwnd()) {
			CRect	rcBrowse;
			m_btnBrowse.GetWindowRect(rcButton);
			rcBrowse.top = (rcDialog.top + VERTICAL_PADDING/2);
			rcBrowse.right = rcLabel.right;
			rcBrowse.left = (rcBrowse.right - rcButton.Width());
			rcBrowse.bottom = (rcBrowse.top + rcButton.Height());
			m_btnBrowse.MoveWindow(rcBrowse);
		}

		if(m_sttDuplicatedFiles.GetSafeHwnd()) {
			rcTemp.OffsetRect(rcLabel.Width() + HORIZONTAL_PADDING, 0);
			m_sttDuplicatedFiles.MoveWindow(rcTemp);
			rcLabel = rcTemp;

			rcLabel.OffsetRect(0, rcTemp.Height() + VERTICAL_PADDING);
		}

		if(m_lstFiles.GetSafeHwnd()) {

			rcButton = CRect(0,0,0,0);
			if(m_btnProcessAll.GetSafeHwnd()) {
				m_btnProcessAll.GetWindowRect(rcButton);
			}

			rcTemp = rcLabel;
			rcTemp.bottom = (rcFolderTree.bottom - rcButton.Height() - VERTICAL_PADDING);
			rcTemp.right = (rcDialog.right - HORIZONTAL_PADDING);
			rcListBox = rcTemp;
			m_lstFiles.MoveWindow(rcTemp);

			if(m_btnProcessAll.GetSafeHwnd()) {
				rcTemp.left = (rcListBox.left + (rcListBox.Width()-rcButton.Width())/2);
				rcTemp.right = (rcTemp.left + rcButton.Width());
				rcTemp.bottom = rcFolderTree.bottom;
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
	}

	m_chkName.SetCheck(BST_CHECKED);
	m_chkSize.SetCheck(BST_CHECKED);
	OnBnClickedChkSize();

	m_lvwFolders.InsertColumn(0, _T("Path"), LVCFMT_LEFT, 500);
	m_lvwFolders.GetHeaderCtrl()->SetFont(&m_fntBold);

	m_lvwDetail.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);
	m_lvwDetail.GetHeaderCtrl()->SetFont(&m_fntBold);
}

void CDuplicateFileFinderDlg::OnHdnItemclickLvwDetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
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
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Access Time"), LVCFMT_LEFT, 100);
	}
	if(m_chkWriteTime.GetCheck() == BST_CHECKED) {
		m_lvwDetail.InsertColumn(m_lvwDetail.GetHeaderCtrl()->GetItemCount(), _T("Write Time"), LVCFMT_LEFT, 100);
	}
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

	POSITION pos = m_arrPaths.GetStartPosition();
	CString	strPath;
	CStringArray arrPaths;
	int nItem = -1;
	while (pos)
	{
		m_arrPaths.GetNextAssoc(pos, strPath, reinterpret_cast<void*&>(nItem));
		arrPaths.Add(strPath);
	}

	if(!m_pScanThread->Initialize(arrPaths)) {
		AfxMessageBox(_T("An error occred!"), MB_ICONEXCLAMATION);
		return;
	}

	if(m_pScanThread->m_hThread != nullptr) {
		CloseHandle(m_pScanThread->m_hThread);
		m_pScanThread->m_hThread = nullptr;
	}
	m_pScanThread->CreateThread();
	m_WorkerDlg.DoModal();
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