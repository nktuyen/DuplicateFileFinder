
// DuplicateFileFinderDlg.h : header file
//

#pragma once
#include "afxshelltreectrl.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "WorkerDialog.h"

class CScanThread;

// CDuplicateFileFinderDlg dialog
class CDuplicateFileFinderDlg : public CDialogEx
{
// Construction
public:
	CDuplicateFileFinderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DUPLICATEFILEFINDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CFont m_fntBold;
	CFont m_fntItalic;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void InitUI();
	BOOL AtLeastOneChecked();
	UINT EnumCheckedPaths();
	void EnumCheckedChildNode(HTREEITEM hNode);
	BOOL ReadyToScan();
	void ResetDetailList();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
public:
	CWorkerDialog* m_pWorkerDlg;
	CScanThread* m_pScanThread;
	CListBox m_lstFiles;
	CMapStringToPtr m_arrPaths;
	CListCtrl m_lvwDetail;
	CListCtrl m_lvwFolders;
	CButton m_btnScan;
	CStatic m_sttFolderTree;
	CStatic m_sttDuplicatedFiles;
	CStatic m_sttDuplicateCriteria;
	CStatic m_sttDuplicationDetail;
	CButton m_btnProcess;
	CButton m_btnProcessAll;
	CButton m_chkName;
	CButton m_chkSize;
	CButton m_chkAttributes;
	CButton m_chkContent;
	CButton m_chkCreateTime;
	CButton m_chkAccessTime;
	CButton m_chkWriteTime;
	afx_msg void OnHdnItemclickLvwDetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedFolderBrowser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkName();
	afx_msg void OnBnClickedChkSize();
	afx_msg void OnBnClickedChkAttribute();
	afx_msg void OnBnClickedChkContent();
	CStatic m_sttCheckboxMessage;
	afx_msg void OnBnClickedChkCtime();
	afx_msg void OnBnClickedChkAtime();
	afx_msg void OnBnClickedChkWtime();
	afx_msg void OnBnClickedBtnScan();
	
	CButton m_btnBrowse;
	afx_msg void OnBnClickedBtnBrowse();
	CStatic m_sttExclude;
	CButton m_btnRemove;
	CButton m_btnClear;
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnLvnItemchangedLvwFolders(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_chkExcludePattern;
	CEdit m_edtPattern;
	CButton m_chkExcludeSize;
	CEdit m_edtExcludeSize;
	CComboBox m_cbSizeUnit;
	CComboBox m_cboSizeCriteria;
	CStatic m_sttExcludeAttr;
	CButton m_chkExcludeROnly;
	CButton m_chkExcludeHidden;
	CButton m_chkExcludeSystem;
	afx_msg void OnBnClickedChkExcludePattern();
	afx_msg void OnBnClickedChkExcludeSize();
	CStatic m_sttPatternsTips;
	CButton m_chkExcludeTemp;
	CButton m_chkExcludeArchive;
	CButton m_chkScanRecursive;
};
