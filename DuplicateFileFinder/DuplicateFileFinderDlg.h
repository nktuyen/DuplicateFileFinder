
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
	afx_msg void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	BOOL PreTranslateMessage(MSG* pMsg);
	void InitUI();
	BOOL AtLeastOneChecked();
	INT_PTR EnumCheckedPaths();
	void EnumCheckedChildNode(HTREEITEM hNode);
	BOOL ReadyToScan();
	void ResetDetailList();
	void DeleteDuplicateInfo();
	void DeleteDuplicateFilesTypes();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
public:
	CWorkerDialog* m_pWorkerDlg;
	CScanThread* m_pScanThread;
	CCriticalSection m_CriticalSection;
	CListBox m_lstFiles;
	CMapStringToPtr m_arrPaths;
	CListCtrl m_lvwDetail;
	CMenu m_Popup;
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
	afx_msg void OnBnClickedChkName();
	afx_msg void OnBnClickedChkSize();
	afx_msg void OnBnClickedChkAttribute();
	afx_msg void OnBnClickedChkContent();
	CStatic m_sttCheckboxMessage;
	afx_msg void OnBnClickedChkCtime();
	afx_msg void OnBnClickedChkAtime();
	afx_msg void OnBnClickedChkWtime();
	afx_msg void OnBnClickedBtnScan();
	afx_msg LRESULT OnScanThreadMessage(WPARAM wparam, LPARAM lParam);
	static UINT m_sThreadMessage;
	CButton m_btnBrowse;
	afx_msg void OnBnClickedBtnBrowse();
	int AddFolder(const CString& strPath);
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
	CString FormatSize(__int64 iSize);
	CStatic m_sttPatternsTips;
	CButton m_chkExcludeTemp;
	CButton m_chkExcludeArchive;
	CButton m_chkScanRecursive;
	CComboBox m_cboDuplicatedFileTypes;
	CStatic m_sttFolderCount;
	CMapStringToPtr mapDuplicateFiles;
	CMapStringToPtr	mapDuplicateFilesTypes;
	afx_msg void OnLbnSelchangeLstFiles();
	afx_msg void OnBnClickedBtnProcessAll();
	afx_msg void OnBnClickedBtnProcess();
	afx_msg void OnNMDblclkLvwDetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLvwDetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedLvwDetail(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_sttKeepTips;
	afx_msg void OnCbnSelchangeCboDuplicatedFileTypes();
	CStatic m_sttProcessAllTips;
	CButton m_chkTypeCriteria;
	afx_msg void OnBnClickedChkType();
	afx_msg void OnPopupSelectall();
	afx_msg void OnPopupDeleteall();
	afx_msg void OnPopupOpen();
	afx_msg void OnPopupExplore();
	afx_msg void OnPopupDelete();
	CStatic m_sttScanBuffer;
	CComboBox m_cboScanBuffer;
};
